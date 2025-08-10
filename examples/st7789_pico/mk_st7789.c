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

#include <assert.h>
#include "mk_st7789.h"

// External functions that need to be implemented by the main application
extern void mk_st7789_set_rst(bool state);
extern void mk_st7789_set_dc(bool state);
extern void mk_st7789_set_cs(bool state);
extern void mk_st7789_set_led(bool state);
extern void mk_st7789_spi_write(const uint8_t *data, size_t len);
extern void mk_st7789_spi_write16(const uint16_t *halfwords, size_t len);
extern void mk_st7789_delay_ms(unsigned ms);

// Helper functions for register operations
static void write_command(uint8_t cmd)
{
    mk_st7789_set_dc(false);  // Command mode
    mk_st7789_set_cs(false);  // Select chip
    mk_st7789_spi_write(&cmd, 1);
    mk_st7789_set_cs(true);   // Deselect chip
}

static void write_data(uint8_t data)
{
    mk_st7789_set_dc(true);   // Data mode
    mk_st7789_set_cs(false);  // Select chip
    mk_st7789_spi_write(&data, 1);
    mk_st7789_set_cs(true);   // Deselect chip
}

static void write_data16(uint16_t data)
{
    mk_st7789_set_dc(true);   // Data mode
    mk_st7789_set_cs(false);  // Select chip
    mk_st7789_spi_write16(&data, 1);
    mk_st7789_set_cs(true);   // Deselect chip
}

static void set_register(uint8_t reg, uint8_t data)
{
    write_command(reg);
    write_data(data);
}

static void set_register16(uint8_t reg, uint16_t data)
{
    write_command(reg);
    write_data16(data);
}

unsigned mk_st7789_init(void)
{
    // Reset the display
    mk_st7789_set_rst(false);
    mk_st7789_delay_ms(100);
    mk_st7789_set_rst(true);
    mk_st7789_delay_ms(100);

    // Turn on backlight
    mk_st7789_set_led(true);

    // Software reset
    write_command(ST7789_SWRESET);
    mk_st7789_delay_ms(150);

    // Sleep out
    write_command(ST7789_SLPOUT);
    mk_st7789_delay_ms(120);

    // Set color mode to 16-bit per pixel (RGB565)
    set_register(ST7789_COLMOD, 0x55);

    // Memory access control
    // MY=1, MX=0, MV=0, ML=0, RGB=0, MH=0
    set_register(ST7789_MADCTL, MADCTL_MY);

    // Set display window for 240x240 display
    // Column address set
    write_command(ST7789_CASET);
    write_data16(0x0000);  // Start column
    write_data16(0x00EF);  // End column (239)

    // Row address set
    write_command(ST7789_RASET);
    write_data16(0x0000);  // Start row
    write_data16(0x00EF);  // End row (239)

    // Normal display mode on
    write_command(ST7789_NORON);
    mk_st7789_delay_ms(10);

    // Display on
    write_command(ST7789_DISPON);
    mk_st7789_delay_ms(120);

    return 0;  // Success
}

void mk_st7789_set_window(uint16_t hor_start, uint16_t hor_end,
    uint16_t vert_start, uint16_t vert_end)
{
    // Set column address
    write_command(ST7789_CASET);
    write_data16(hor_start);
    write_data16(hor_end);

    // Set row address
    write_command(ST7789_RASET);
    write_data16(vert_start);
    write_data16(vert_end);
}

void mk_st7789_set_address(uint16_t x, uint16_t y)
{
    // Set column address
    write_command(ST7789_CASET);
    write_data16(x);
    write_data16(x);

    // Set row address
    write_command(ST7789_RASET);
    write_data16(y);
    write_data16(y);
}

void mk_st7789_write_pixels_start(void)
{
    write_command(ST7789_RAMWR);
    mk_st7789_set_dc(true);   // Data mode
    mk_st7789_set_cs(false);  // Select chip
}

void mk_st7789_write_pixels_end(void)
{
    mk_st7789_set_cs(true);   // Deselect chip
}

void mk_st7789_write_pixels(const uint16_t *pixels, uint_fast16_t nmemb)
{
    mk_st7789_write_pixels_start();
    mk_st7789_spi_write16(pixels, nmemb);
    mk_st7789_write_pixels_end();
}

void mk_st7789_invert_display(bool invert)
{
    write_command(invert ? ST7789_INVON : ST7789_INVOFF);
}

void mk_st7789_display_control(bool invert, st7789_color_mode_e colour_mode)
{
    mk_st7789_invert_display(invert);
    // Note: ST7789 doesn't have an 8-color mode like ILI9225
    // This parameter is kept for compatibility
    (void)colour_mode;
}

void mk_st7789_power_control(bool sleep)
{
    write_command(sleep ? ST7789_SLPIN : ST7789_SLPOUT);
    if (!sleep) {
        mk_st7789_delay_ms(120);
    }
}

void mk_st7789_exit(void)
{
    // Currently does nothing, but could be extended for cleanup
}

void mk_st7789_fill_rect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color)
{
    mk_st7789_set_window(x, x + w - 1, y, y + h - 1);
    
    mk_st7789_write_pixels_start();
    for (uint16_t i = 0; i < w * h; i++) {
        mk_st7789_spi_write16(&color, 1);
    }
    mk_st7789_write_pixels_end();
}

void mk_st7789_fill(uint16_t color)
{
    mk_st7789_fill_rect(0, 0, SCREEN_SIZE_X, SCREEN_SIZE_Y, color);
}

void mk_st7789_pixel(uint16_t x, uint16_t y, uint16_t color)
{
    mk_st7789_set_address(x, y);
    mk_st7789_write_pixels(&color, 1);
}

void mk_st7789_blit(uint16_t *fbuf, uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{
    mk_st7789_set_window(x, x + w - 1, y, y + h - 1);
    mk_st7789_write_pixels(fbuf, w * h);
}

// Simple 8x8 font data (basic ASCII characters)
static const uint8_t font_8x8[128][8] = {
    // Space (0x20)
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    // ... other characters would go here
    // For brevity, just implementing a few key characters
};

void mk_st7789_get_letter(uint16_t *fbuf, char letter, uint16_t color, uint16_t bgcolor)
{
    uint8_t char_index = (uint8_t)letter;
    if (char_index < 32 || char_index > 126) {
        char_index = 32;  // Default to space for invalid characters
    }
    
    const uint8_t *char_data = font_8x8[char_index - 32];
    
    for (int row = 0; row < 8; row++) {
        uint8_t row_data = char_data[row];
        for (int col = 0; col < 8; col++) {
            bool pixel_on = (row_data >> (7 - col)) & 1;
            fbuf[row * 8 + col] = pixel_on ? color : bgcolor;
        }
    }
}

void mk_st7789_text(char *s, uint16_t x, uint16_t y, uint16_t color, uint16_t bgcolor)
{
    uint16_t current_x = x;
    uint16_t current_y = y;
    
    while (*s) {
        if (*s == '\n') {
            current_x = x;
            current_y += 8;
        } else {
            uint16_t char_buf[64];  // 8x8 = 64 pixels
            mk_st7789_get_letter(char_buf, *s, color, bgcolor);
            mk_st7789_blit(char_buf, current_x, current_y, 8, 8);
            current_x += 8;
        }
        s++;
    }
}