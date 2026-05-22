#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "drivers/st7789.h"
#include "kernel/shell.h"
#include "kernel/mutex.h"
#include "kernel/tasks/scheduler.h"
#include "logo.h"

// ─── Splash Screen ───────────────────────────────────────
void show_splash() {
    st7789_fill(0x0000);
    int x_offset = (DISPLAY_WIDTH  - LOGO_W)      / 2;
    int y_offset = (DISPLAY_HEIGHT - LOGO_HEIGHT)  / 2;
    for (int y = 0; y < LOGO_HEIGHT; y++)
        for (int x = 0; x < LOGO_W; x++)
            st7789_set_pixel(x + x_offset, y + y_offset,
                             logo_data[y * LOGO_W + x]);
    sleep_ms(3000);
}

// ─── Kern 1: Heartbeat ───────────────────────────────────
void core1_entry() {
    static uint8_t tick = 0;
    while (1) {
        uint16_t color = (tick++ % 2 == 0) ? 0x07E0 : 0x001F;

        display_lock();
        st7789_fill_rect(185, 120, 35, 10, color);
        display_unlock();

        sleep_ms(500);
    }
}

// ─── Tasks ───────────────────────────────────────────────
void task_shell(void) {
    shell_run();
}

// ─── Main ────────────────────────────────────────────────
int main() {
    stdio_init_all();
    sleep_ms(2000);

    st7789_init();
    
    // Mutex initialisieren
    mutex_display_init();

    show_splash();

    st7789_fill(0x001F);
    sleep_ms(100);

    // Kern 1 starten
    multicore_launch_core1(core1_entry);

    // Scheduler
    scheduler_init();
    scheduler_add_task("shell", task_shell);
    scheduler_run();

    return 0;
}