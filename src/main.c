#include "pico/stdlib.h"
#include "drivers/st7789.h"
#include "kernel/shell.h"

int main() {
    // stdio via USB initialisieren
    stdio_init_all();
    sleep_ms(2000); // Warten bis USB bereit ist

    // Display initialisieren
    st7789_init();

    // Shell starten
    shell_run();

    return 0;
}