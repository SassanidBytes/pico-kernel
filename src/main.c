#include "pico/stdlib.h"
#include "drivers/st7789.h"
#include "kernel/shell.h"
#include "logo.h"

void show_splash() {
    // Hintergrund schwarz
    st7789_fill(0x0000);

    // Logo zentriert anzeigen
    int x_offset = (DISPLAY_WIDTH - LOGO_W) / 2;
    int y_offset = (DISPLAY_HEIGHT - LOGO_HEIGHT) / 2;
    for (int y = 0; y < LOGO_HEIGHT; y++) {
        for (int x = 0; x < LOGO_W; x++) {
            uint16_t color = logo_data[y * LOGO_W + x];
            st7789_set_pixel(x + x_offset, y + y_offset, color);
        }
    }

    // 3 Sekunden anzeigen
    sleep_ms(3000);
}

int main() {
    stdio_init_all();
    sleep_ms(2000);

    st7789_init();

    // Splash Screen
    show_splash();

    // Shell starten
    shell_run();

    return 0;
}