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

#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>

// ST7789 display dimensions (common 240x240 or 240x135 variants)
#define SCREEN_SIZE_X 240u
#define SCREEN_SIZE_Y 240u

// ST7789 specific registers
#define ST7789_NOP        0x00
#define ST7789_SWRESET    0x01
#define ST7789_RDDID      0x04
#define ST7789_RDDST      0x09
#define ST7789_SLPIN      0x10
#define ST7789_SLPOUT     0x11
#define ST7789_PTLON      0x12
#define ST7789_NORON      0x13
#define ST7789_INVOFF     0x20
#define ST7789_INVON      0x21
#define ST7789_DISPOFF    0x28
#define ST7789_DISPON     0x29
#define ST7789_CASET      0x2A
#define ST7789_RASET      0x2B
#define ST7789_RAMWR      0x2C
#define ST7789_RAMRD      0x2E
#define ST7789_PTLAR      0x30
#define ST7789_COLMOD     0x3A
#define ST7789_MADCTL     0x36
#define ST7789_FRMCTR1    0xB1
#define ST7789_FRMCTR2    0xB2
#define ST7789_FRMCTR3    0xB3
#define ST7789_INVCTR     0xB4
#define ST7789_DISSET5    0xB6
#define ST7789_PWCTR1     0xC0
#define ST7789_PWCTR2     0xC1
#define ST7789_PWCTR3     0xC2
#define ST7789_PWCTR4     0xC3
#define ST7789_PWCTR5     0xC4
#define ST7789_VMCTR1     0xC5
#define ST7789_RDID1      0xDA
#define ST7789_RDID2      0xDB
#define ST7789_RDID3      0xDC
#define ST7789_RDID4      0xDD
#define ST7789_GMCTRP1    0xE0
#define ST7789_GMCTRN1    0xE1
#define ST7789_PWCTR6     0xFC

// MADCTL bits
#define MADCTL_MY  0x80
#define MADCTL_MX  0x40
#define MADCTL_MV  0x20
#define MADCTL_ML  0x10
#define MADCTL_RGB 0x00
#define MADCTL_BGR 0x08
#define MADCTL_MH  0x04

typedef enum {
    ST7789_COLOR_MODE_FULL = 0,
    ST7789_COLOR_MODE_8COLOR = 1
} st7789_color_mode_e;

/**
 * Controls the reset pin of the ST7789.
 * \param state	Set to 0 on low output, else high.
 */
extern void mk_st7789_set_rst(bool state);

/**
 * Controls state of DC pin (Data/Command).
 * \param state	Set to 0 for command, 1 for data.
 */
extern void mk_st7789_set_dc(bool state);

/**
 * Controls state of CS pin.
 * \param state	Set to 0 on low output, else high.
 */
extern void mk_st7789_set_cs(bool state);

/**
 * Controls state of LED pin (backlight).
 * \param state	Set to 0 on low output, else high.
 */
extern void mk_st7789_set_led(bool state);

/**
 * Sends data to the ST7789 using SPI. Return only after sending data.
 * \param data Data to send.
 * \param len Length of data in bytes.
 */
void mk_st7789_spi_write(const uint8_t *data, size_t len);

/**
 * Sends 16-bit data to the ST7789 using SPI.
 * \param halfwords Data to send.
 * \param len Number of 16-bit words.
 */
void mk_st7789_spi_write16(const uint16_t *halfwords, size_t len);

/**
 * Delays execution in milliseconds.
 * \param ms Duration to sleep for.
 */
extern void mk_st7789_delay_ms(unsigned ms);

/**
 * Initialise ST7789 LCD with default settings.
 * \return 0 on success, else error due to invalid LCD identification.
 */
unsigned mk_st7789_init(void);

/**
 * Set the window that pixel will be written to. Address will loop within the
 * window.
 *
 * \param hor_start
 * \param hor_end
 * \param vert_start
 * \param vert_end
 */
void mk_st7789_set_window(uint16_t hor_start, uint16_t hor_end,
    uint16_t vert_start, uint16_t vert_end);

/**
 * Set address pointer in GRAM. Must be within window.
 * \param x
 * \param y
 */
void mk_st7789_set_address(uint16_t x, uint16_t y);

/**
 * Write pixels to ST7789 GRAM. These pixels will be displayed on screen at
 * next vsync.
 * \param pixels Pixel data in RGB565 format to write to LCD.
 * \param nmemb Number of pixels.
 */
void mk_st7789_write_pixels(const uint16_t *pixels, uint_fast16_t nmemb);

/**
 * Inverts the display.
 * @param invert
 */
void mk_st7789_invert_display(bool invert);

void mk_st7789_write_pixels_start(void);
void mk_st7789_write_pixels_end(void);

void mk_st7789_display_control(bool invert, st7789_color_mode_e colour_mode);

void mk_st7789_power_control(bool sleep);

/**
 * Exit and stop using LCD. Currently does nothing.
 */
void mk_st7789_exit(void);

/**
 * Fill a rectangle at the given location, size and color.
 */
void mk_st7789_fill_rect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);

/**
 * Fill the entire screen with the specified RGB565 color
 */
void mk_st7789_fill(uint16_t color);

/**
 * Set a specified pixel to the given color
 */
void mk_st7789_pixel(uint16_t x, uint16_t y, uint16_t color);

/**
 * Copy the specified framebuffer at the given coordinates
 */
void mk_st7789_blit(uint16_t *fbuf, uint16_t x, uint16_t y, uint16_t w, uint16_t h);

/**
 * Return an 8x8 framebuffer for the given letter and color / background color
 */
void mk_st7789_get_letter(uint16_t *fbuf, char letter, uint16_t color, uint16_t bgcolor);

/**
 * Write text to the screen using the the coordinates as the upper-left corner of the text.
 * All characters have dimensions of 8x8 pixels.
 */
void mk_st7789_text(char *s, uint16_t x, uint16_t y, uint16_t color, uint16_t bgcolor);