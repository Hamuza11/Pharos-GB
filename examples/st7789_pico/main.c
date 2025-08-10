/**
 * Copyright (C) 2024 by AI Assistant
 * 
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH
 * REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT,
 * INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
 * LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */

// Peanut-GB emulator settings
#define ENABLE_LCD	1
#define ENABLE_SOUND	0  // Disable sound for simplicity
#define ENABLE_SDCARD	0  // Disable SD card for simplicity
#define PEANUT_GB_HIGH_LCD_ACCURACY 1
#define PEANUT_GB_USE_BIOS 0

/* Use DMA for all drawing to LCD. Benefits aren't fully realised at the moment
 * due to busy loops waiting for DMA completion. */
#define USE_DMA		0

/* C Headers */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* RP2040 Headers */
#include <hardware/pio.h>
#include <hardware/clocks.h>
#include <hardware/dma.h>
#include <hardware/spi.h>
#include <hardware/sync.h>
#include <hardware/flash.h>
#include <hardware/timer.h>
#include <hardware/vreg.h>
#include <pico/bootrom.h>
#include <pico/stdio.h>
#include <pico/stdlib.h>
#include <pico/multicore.h>
#include <sys/unistd.h>
#include <hardware/irq.h>

/* Project headers */
#include "peanut_gb.h"
#include "mk_st7789.h"

/* GPIO Connections for ST7789 */
#define GPIO_UP		2
#define GPIO_DOWN	3
#define GPIO_LEFT	4
#define GPIO_RIGHT	5
#define GPIO_A		6
#define GPIO_B		7
#define GPIO_SELECT	8
#define GPIO_START	9
#define GPIO_CS		17
#define GPIO_CLK	18
#define GPIO_MOSI	19
#define GPIO_DC		20
#define GPIO_RST	21
#define GPIO_LED	22

/* Game Boy LCD dimensions */
#define LCD_WIDTH	160
#define LCD_HEIGHT	144

/* Game Boy color palette (RGB565 format) */
typedef uint16_t palette_t[4][4];

/* Global variables */
static uint8_t ram[32768];
static int lcd_line_busy = 0;
static palette_t palette;	// Colour palette
static uint8_t manual_palette_selected = 0;

/* Multicore command structure */
union core_cmd {
    struct {
        /* Does nothing. */
        #define CORE_CMD_NOP		0
        /* Set line "data" on the LCD. Pixel data is in pixels_buffer. */
        #define CORE_CMD_LCD_LINE	1
        /* Control idle mode on the LCD. Limits colours to 2 bits. */
        #define CORE_CMD_IDLE_SET	2
        /* Set a specific pixel. For debugging. */
        #define CORE_CMD_SET_PIXEL	3
        uint8_t cmd;
        uint8_t unused1;
        uint8_t unused2;
        uint8_t data;
    };
    uint32_t full;
};

/* Pixel data is stored in here. */
static uint8_t pixels_buffer[LCD_WIDTH];

/* Functions required for communication with the ST7789 */
void mk_st7789_set_rst(bool state)
{
    gpio_put(GPIO_RST, state);
}

void mk_st7789_set_dc(bool state)
{
    gpio_put(GPIO_DC, state);
}

void mk_st7789_set_cs(bool state)
{
    gpio_put(GPIO_CS, state);
}

void mk_st7789_set_led(bool state)
{
    gpio_put(GPIO_LED, state);
}

void mk_st7789_spi_write(const uint8_t *data, size_t len)
{
    spi_write_blocking(spi0, data, len);
}

void mk_st7789_spi_write16(const uint16_t *halfwords, size_t len)
{
    spi_write16_blocking(spi0, halfwords, len);
}

void mk_st7789_delay_ms(unsigned ms)
{
    sleep_ms(ms);
}

/* Functions required by Peanut-GB */
uint8_t gb_rom_read(struct gb_s *gb, const uint_fast32_t addr)
{
    // For this example, we'll use a simple ROM stored in flash
    // In a real implementation, you'd load this from SD card or flash
    static const uint8_t test_rom[] = {
        // This would be your actual Game Boy ROM data
        // For now, just return a simple pattern
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };
    
    if (addr < sizeof(test_rom)) {
        return test_rom[addr];
    }
    return 0xFF;
}

uint8_t gb_cart_ram_read(struct gb_s *gb, const uint_fast32_t addr)
{
    if (addr < sizeof(ram)) {
        return ram[addr];
    }
    return 0xFF;
}

void gb_cart_ram_write(struct gb_s *gb, const uint_fast32_t addr,
                       const uint8_t val)
{
    if (addr < sizeof(ram)) {
        ram[addr] = val;
    }
}

void gb_error(struct gb_s *gb, const enum gb_error_e gb_err, const uint16_t addr)
{
    printf("GB Error: %d at address 0x%04X\n", gb_err, addr);
}

/* LCD drawing functions */
void core1_lcd_draw_line(const uint_fast8_t line)
{
    static uint16_t fb[LCD_WIDTH];
    
    // Convert Game Boy pixels to RGB565 colors
    for (unsigned int x = 0; x < LCD_WIDTH; x++) {
        uint8_t pixel_data = pixels_buffer[x];
        uint8_t shade = pixel_data & 3;  // Lower 2 bits for shade
        uint8_t layer = (pixel_data >> 4) & 3;  // Upper 2 bits for layer
        
        // Use the palette to get the actual color
        fb[x] = palette[layer][shade];
    }
    
    // Calculate display position (center the 160x144 Game Boy screen on 240x240 display)
    uint16_t display_x = (SCREEN_SIZE_X - LCD_WIDTH) / 2;  // 40
    uint16_t display_y = (SCREEN_SIZE_Y - LCD_HEIGHT) / 2; // 48
    
    // Set the window for this line
    mk_st7789_set_window(display_x, display_x + LCD_WIDTH - 1, 
                         display_y + line, display_y + line);
    
    // Write the pixels
    mk_st7789_write_pixels(fb, LCD_WIDTH);
    
    __atomic_store_n(&lcd_line_busy, 0, __ATOMIC_SEQ_CST);
}

_Noreturn
void main_core1(void)
{
    union core_cmd cmd;

    /* Initialise and control LCD on core 1. */
    mk_st7789_init();

    /* Clear LCD screen. */
    mk_st7789_fill(0x0000);

    /* Set LCD window to Game Boy size and center it */
    uint16_t display_x = (SCREEN_SIZE_X - LCD_WIDTH) / 2;
    uint16_t display_y = (SCREEN_SIZE_Y - LCD_HEIGHT) / 2;
    mk_st7789_fill_rect(display_x, display_y, LCD_WIDTH, LCD_HEIGHT, 0x0000);

    /* Handle commands coming from core0. */
    while(1) {
        cmd.full = multicore_fifo_pop_blocking();
        switch(cmd.cmd) {
        case CORE_CMD_LCD_LINE:
            core1_lcd_draw_line(cmd.data);
            break;

        case CORE_CMD_IDLE_SET:
            mk_st7789_display_control(true, cmd.data);
            break;

        case CORE_CMD_NOP:
        default:
            break;
        }
    }

    __builtin_unreachable();
}

#if ENABLE_LCD
void lcd_draw_line(struct gb_s *gb, const uint8_t pixels[LCD_WIDTH],
                   const uint_fast8_t line)
{
    union core_cmd cmd;

    /* Wait until previous line is sent. */
    while(__atomic_load_n(&lcd_line_busy, __ATOMIC_SEQ_CST))
        tight_loop_contents();

    memcpy(pixels_buffer, pixels, LCD_WIDTH);
    
    /* Populate command. */
    cmd.cmd = CORE_CMD_LCD_LINE;
    cmd.data = line;

    __atomic_store_n(&lcd_line_busy, 1, __ATOMIC_SEQ_CST);
    multicore_fifo_push_blocking(cmd.full);
}
#endif

/* Initialize color palette */
void init_palette(void)
{
    // Classic Game Boy green palette (RGB565 format)
    palette[0][0] = 0xE7F3;  // White
    palette[0][1] = 0xAD55;  // Light green
    palette[0][2] = 0x52AA;  // Dark green
    palette[0][3] = 0x0000;  // Black
    
    // Copy to other layers
    for (int i = 1; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            palette[i][j] = palette[0][j];
        }
    }
}

int main(void)
{
    static struct gb_s gb;
    enum gb_init_error_e ret;
    
    /* Overclock for better performance */
    {
        const unsigned vco = 1596*1000*1000;	/* 266MHz */
        const unsigned div1 = 6, div2 = 1;

        vreg_set_voltage(VREG_VOLTAGE_1_15);
        sleep_ms(2);
        set_sys_clock_pll(vco, div1, div2);
        sleep_ms(2);
    }

    /* Initialise USB serial connection for debugging. */
    stdio_init_all();
    time_init();
    sleep_ms(1000);
    printf("ST7789 Pico-GB Initializing...\n");

    /* Initialise GPIO pins. */
    gpio_set_function(GPIO_UP, GPIO_FUNC_SIO);
    gpio_set_function(GPIO_DOWN, GPIO_FUNC_SIO);
    gpio_set_function(GPIO_LEFT, GPIO_FUNC_SIO);
    gpio_set_function(GPIO_RIGHT, GPIO_FUNC_SIO);
    gpio_set_function(GPIO_A, GPIO_FUNC_SIO);
    gpio_set_function(GPIO_B, GPIO_FUNC_SIO);
    gpio_set_function(GPIO_SELECT, GPIO_FUNC_SIO);
    gpio_set_function(GPIO_START, GPIO_FUNC_SIO);
    gpio_set_function(GPIO_CS, GPIO_FUNC_SIO);
    gpio_set_function(GPIO_CLK, GPIO_FUNC_SPI);
    gpio_set_function(GPIO_MOSI, GPIO_FUNC_SPI);
    gpio_set_function(GPIO_DC, GPIO_FUNC_SIO);
    gpio_set_function(GPIO_RST, GPIO_FUNC_SIO);
    gpio_set_function(GPIO_LED, GPIO_FUNC_SIO);

    gpio_set_dir(GPIO_UP, false);
    gpio_set_dir(GPIO_DOWN, false);
    gpio_set_dir(GPIO_LEFT, false);
    gpio_set_dir(GPIO_RIGHT, false);
    gpio_set_dir(GPIO_A, false);
    gpio_set_dir(GPIO_B, false);
    gpio_set_dir(GPIO_SELECT, false);
    gpio_set_dir(GPIO_START, false);
    gpio_set_dir(GPIO_CS, true);
    gpio_set_dir(GPIO_DC, true);
    gpio_set_dir(GPIO_RST, true);
    gpio_set_dir(GPIO_LED, true);
    
    gpio_pull_up(GPIO_UP);
    gpio_pull_up(GPIO_DOWN);
    gpio_pull_up(GPIO_LEFT);
    gpio_pull_up(GPIO_RIGHT);
    gpio_pull_up(GPIO_A);
    gpio_pull_up(GPIO_B);
    gpio_pull_up(GPIO_SELECT);
    gpio_pull_up(GPIO_START);

    /* Set SPI clock to use high frequency. */
    clock_configure(clk_peri, 0,
            CLOCKS_CLK_PERI_CTRL_AUXSRC_VALUE_CLK_SYS,
            125 * 1000 * 1000, 125 * 1000 * 1000);
    spi_init(spi0, 30*1000*1000);
    spi_set_format(spi0, 8, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);

    /* Initialize color palette */
    init_palette();

    /* Launch core1 for LCD handling */
    multicore_launch_core1(main_core1);

    /* Initialise GB context. */
    ret = gb_init(&gb, &gb_rom_read, &gb_cart_ram_read,
                  &gb_cart_ram_write, &gb_error, NULL);
    
    if (ret != GB_INIT_NO_ERROR) {
        printf("GB Init Error: %d\n", ret);
        return -1;
    }

    /* Initialize LCD */
    ret = gb_init_lcd(&gb, &lcd_draw_line);
    if (ret != GB_INIT_NO_ERROR) {
        printf("GB LCD Init Error: %d\n", ret);
        return -1;
    }

    printf("ST7789 Pico-GB Ready!\n");

    /* Main emulation loop */
    while(1) {
        /* Run one frame */
        gb_run_frame(&gb);
        
        /* Handle input */
        // This is a simplified version - you'd want to add proper input handling
        if (!gpio_get(GPIO_A)) {
            // Button A pressed
        }
        if (!gpio_get(GPIO_B)) {
            // Button B pressed
        }
        // Add more input handling as needed
        
        /* Small delay to maintain frame rate */
        sleep_ms(16);  // ~60 FPS
    }

    return 0;
}