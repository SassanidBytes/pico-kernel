#include "pico/stdlib.h"
#include "drivers/st7789.h"
#include "kernel/shell.h"
#include "logo.h"
#include "pico/multicore.h"
#include "kernel/tasks/scheduler.h"




//----- Splah Screen -------------------
void show_splash() {
    // Hintergrund schwarz
    st7789_fill(0x0000);

    
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

//─── Tasks ───────────────── 
void task_shell(void){

shell_run();

}

void task_heartbeat(void)  {

// Kern 1: kleine Status-Anzeige oben rechts
    static uint8_t tick = 0; 
    uint16_t color = (tick++ % 2 == 0) ? 0x07E0 : 0x0000;
    st7789_fill_rect(255, 2, 10, 10, color); 
    sleep_ms(500); 
}

// ─── Kern 1 Entry ────────────────────────────────────────
void core1_entry(){

    while(1){
        task_heartbeat(); 
    }

}

// ─── Main ────────────────────────────────────────────────

int main() {
    stdio_init_all();
    sleep_ms(2000);

    st7789_init();
    show_splash();

    // Kern 1 starten (Heartbeat)
    multicore_launch_core1(core1_entry);
    scheduler_init();
    scheduler_add_task("shell", task_shell);
    
    //scheduler starten

    scheduler_run();

    return 0;
}