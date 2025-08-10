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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/* RP2040 Headers */
#include <hardware/spi.h>
#include <hardware/clocks.h>
#include <hardware/vreg.h>
#include <pico/stdio.h>
#include <pico/stdlib.h>

/* Project headers */
#include "mk_st7789.h"

/* GPIO Connections for ST7789 */
#define GPIO_CS		17
#define GPIO_CLK	18
#define GPIO_MOSI	19
#define GPIO_DC		20
#define GPIO_RST	21
#define GPIO_LED	22

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

/* Test patterns */
void test_pattern_1(void)
{
    printf("Test Pattern 1: Solid colors\n");
    
    // Red
    mk_st7789_fill(0xF800);
    sleep_ms(1000);
    
    // Green
    mk_st7789_fill(0x07E0);
    sleep_ms(1000);
    
    // Blue
    mk_st7789_fill(0x001F);
    sleep_ms(1000);
    
    // White
    mk_st7789_fill(0xFFFF);
    sleep_ms(1000);
    
    // Black
    mk_st7789_fill(0x0000);
    sleep_ms(1000);
}

void test_pattern_2(void)
{
    printf("Test Pattern 2: Color bars\n");
    
    uint16_t colors[] = {0xF800, 0x07E0, 0x001F, 0xFFFF, 0x0000};
    int num_colors = sizeof(colors) / sizeof(colors[0]);
    int bar_width = SCREEN_SIZE_X / num_colors;
    
    for (int i = 0; i < num_colors; i++) {
        int x = i * bar_width;
        mk_st7789_fill_rect(x, 0, bar_width, SCREEN_SIZE_Y, colors[i]);
    }
    
    sleep_ms(3000);
}

void test_pattern_3(void)
{
    printf("Test Pattern 3: Checkerboard\n");
    
    int square_size = 20;
    bool color_flag = true;
    
    for (int y = 0; y < SCREEN_SIZE_Y; y += square_size) {
        for (int x = 0; x < SCREEN_SIZE_X; x += square_size) {
            uint16_t color = color_flag ? 0xFFFF : 0x0000;
            mk_st7789_fill_rect(x, y, square_size, square_size, color);
            color_flag = !color_flag;
        }
        color_flag = !color_flag;  // Alternate row pattern
    }
    
    sleep_ms(3000);
}

void test_pattern_4(void)
{
    printf("Test Pattern 4: Gradient\n");
    
    for (int y = 0; y < SCREEN_SIZE_Y; y++) {
        uint16_t r = (y * 31) / SCREEN_SIZE_Y;
        uint16_t g = ((SCREEN_SIZE_Y - y) * 63) / SCREEN_SIZE_Y;
        uint16_t b = (y * 31) / SCREEN_SIZE_Y;
        
        uint16_t color = (r << 11) | (g << 5) | b;
        
        for (int x = 0; x < SCREEN_SIZE_X; x++) {
            mk_st7789_pixel(x, y, color);
        }
    }
    
    sleep_ms(3000);
}

void test_pattern_5(void)
{
    printf("Test Pattern 5: Text\n");
    
    // Clear screen
    mk_st7789_fill(0x0000);
    
    // Display some text
    mk_st7789_text("ST7789 Test", 10, 10, 0xFFFF, 0x0000);
    mk_st7789_text("Display Working!", 10, 30, 0x07E0, 0x0000);
    mk_st7789_text("Pico + ST7789", 10, 50, 0xF800, 0x0000);
    
    sleep_ms(3000);
}

void test_pattern_6(void)
{
    printf("Test Pattern 6: Animated pattern\n");
    
    for (int frame = 0; frame < 50; frame++) {
        // Clear screen
        mk_st7789_fill(0x0000);
        
        // Draw moving circles
        for (int i = 0; i < 5; i++) {
            int x = (frame * 5 + i * 50) % SCREEN_SIZE_X;
            int y = (frame * 3 + i * 40) % SCREEN_SIZE_Y;
            int radius = 15;
            
            // Simple circle drawing
            for (int dy = -radius; dy <= radius; dy++) {
                for (int dx = -radius; dx <= radius; dx++) {
                    if (dx * dx + dy * dy <= radius * radius) {
                        int px = x + dx;
                        int py = y + dy;
                        if (px >= 0 && px < SCREEN_SIZE_X && py >= 0 && py < SCREEN_SIZE_Y) {
                            uint16_t color = 0xFFFF - (i * 0x3333);
                            mk_st7789_pixel(px, py, color);
                        }
                    }
                }
            }
        }
        
        sleep_ms(100);
    }
}

int main(void)
{
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
    printf("ST7789 Test Program Starting...\n");

    /* Initialise GPIO pins. */
    gpio_set_function(GPIO_CS, GPIO_FUNC_SIO);
    gpio_set_function(GPIO_CLK, GPIO_FUNC_SPI);
    gpio_set_function(GPIO_MOSI, GPIO_FUNC_SPI);
    gpio_set_function(GPIO_DC, GPIO_FUNC_SIO);
    gpio_set_function(GPIO_RST, GPIO_FUNC_SIO);
    gpio_set_function(GPIO_LED, GPIO_FUNC_SIO);

    gpio_set_dir(GPIO_CS, true);
    gpio_set_dir(GPIO_DC, true);
    gpio_set_dir(GPIO_RST, true);
    gpio_set_dir(GPIO_LED, true);

    /* Set SPI clock to use high frequency. */
    clock_configure(clk_peri, 0,
            CLOCKS_CLK_PERI_CTRL_AUXSRC_VALUE_CLK_SYS,
            125 * 1000 * 1000, 125 * 1000 * 1000);
    spi_init(spi0, 30*1000*1000);
    spi_set_format(spi0, 8, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);

    printf("Initializing ST7789 display...\n");
    
    /* Initialize the display */
    unsigned ret = mk_st7789_init();
    if (ret != 0) {
        printf("Failed to initialize ST7789 display: %d\n", ret);
        return -1;
    }
    
    printf("ST7789 display initialized successfully!\n");
    printf("Starting test patterns...\n");

    /* Run test patterns */
    while (1) {
        test_pattern_1();
        test_pattern_2();
        test_pattern_3();
        test_pattern_4();
        test_pattern_5();
        test_pattern_6();
        
        printf("All test patterns completed. Restarting...\n");
    }

    return 0;
}