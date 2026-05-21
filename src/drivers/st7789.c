#include "st7789.h"
#include "hardware/spi.h"
#include "hardware/gpio.h"
#include "pico/stdlib.h"

static void write_cmd(uint8_t cmd) {
    gpio_put(PIN_DC, 0);
    gpio_put(PIN_CS, 0);
    spi_write_blocking(spi0, &cmd, 1);
    gpio_put(PIN_CS, 1);
}

static void write_data(uint8_t data) {
    gpio_put(PIN_DC, 1);
    gpio_put(PIN_CS, 0);
    spi_write_blocking(spi0, &data, 1);
    gpio_put(PIN_CS, 1);
}

void st7789_init() {
    spi_init(spi0, 62500000);
    gpio_set_function(PIN_SCK, GPIO_FUNC_SPI);
    gpio_set_function(PIN_MOSI, GPIO_FUNC_SPI);

    gpio_init(PIN_CS);  gpio_set_dir(PIN_CS, GPIO_OUT);
    gpio_init(PIN_DC);  gpio_set_dir(PIN_DC, GPIO_OUT);
    gpio_init(PIN_BL);  gpio_set_dir(PIN_BL, GPIO_OUT);
    gpio_put(PIN_BL, 1);

    write_cmd(0x01); sleep_ms(150);  // Software Reset
    write_cmd(0x11); sleep_ms(50);   // Sleep Out
    write_cmd(0x3A); write_data(0x05); // 16bit Farben
    write_cmd(0x36); write_data(0x70); // Landscape Rotation
    write_cmd(0x21);                   // Farb-Inversion an
    write_cmd(0x29); sleep_ms(10);     // Display On
    st7789_fill(0x0000);               // Screen leeren
}

void st7789_set_window(int x, int y, int w, int h) {
    write_cmd(0x2A);
    write_data((x+40) >> 8); write_data(x+40);
    write_data((x+40+w-1) >> 8); write_data(x+40+w-1);
    write_cmd(0x2B);
    write_data((y+53) >> 8); write_data(y+53);
    write_data((y+53+h-1) >> 8); write_data(y+53+h-1);
    write_cmd(0x2C);
}

void st7789_set_pixel(int x, int y, uint16_t color) {
    st7789_set_window(x, y, 1, 1);
    gpio_put(PIN_DC, 1);
    gpio_put(PIN_CS, 0);
    uint8_t data[2] = {color >> 8, color & 0xFF};
    spi_write_blocking(spi0, data, 2);
    gpio_put(PIN_CS, 1);
}

void st7789_fill(uint16_t color) {
    st7789_set_window(0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT);
    gpio_put(PIN_DC, 1);
    gpio_put(PIN_CS, 0);
    uint8_t data[2] = {color >> 8, color & 0xFF};
    for (int i = 0; i < DISPLAY_WIDTH * DISPLAY_HEIGHT; i++) {
        spi_write_blocking(spi0, data, 2);
    }
    gpio_put(PIN_CS, 1);
}

void st7789_fill_rect(int x, int y, int w, int h, uint16_t color) {
    st7789_set_window(x, y, w, h);
    gpio_put(PIN_DC, 1);
    gpio_put(PIN_CS, 0);
    uint8_t data[2] = {color >> 8, color & 0xFF};
    for (int i = 0; i < w * h; i++) {
        spi_write_blocking(spi0, data, 2);
    }
    gpio_put(PIN_CS, 1);
}