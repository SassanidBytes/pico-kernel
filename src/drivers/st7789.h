#ifndef ST7789_H
#define ST7789_H

#include <stdint.h>

// Display Auflösung (Pimoroni Pico Display)
#define DISPLAY_WIDTH  240
#define DISPLAY_HEIGHT 135

// SPI Pins (Pimoroni Pico Display)
#define PIN_SCK   18
#define PIN_MOSI  19
#define PIN_CS    17
#define PIN_DC    16
#define PIN_BL    20
#define PIN_RST   -1  // Nicht verbunden 

// Farben (RGB565 Format)
#define COLOR_BLACK   0x0000
#define COLOR_WHITE   0xFFFF
#define COLOR_RED     0xF800
#define COLOR_GREEN   0x07E0
#define COLOR_BLUE    0x001F
#define COLOR_YELLOW  0xFFE0
#define COLOR_CYAN    0x07FF

// Funktionen
void st7789_init();
void st7789_set_pixel(int x, int y, uint16_t color);
void st7789_fill(uint16_t color);
void st7789_fill_rect(int x, int y, int w, int h, uint16_t color);

#endif