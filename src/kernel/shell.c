#include "shell.h"
#include "st7789.h"
#include "font8x8.h"
#include "mutex.h"
#include "scheduler.h"
#include "pong.h"
#include "pico/stdlib.h"
#include <string.h>
#include <stdio.h>
#include "invaders.h"

static int cursor_col = 0;
static int cursor_row = 0;

void shell_draw_char(char c, int col, int row, uint16_t fg, uint16_t bg) {
    if (c < 32 || c > 126) c = 32;
    const uint8_t* glyph = font8x8[c - 32];
    int px = col * 9;
    int py = row * 10;
    display_lock();
    for (int y = 0; y < 8; y++)
        for (int x = 0; x < 8; x++) {
            uint16_t color = (glyph[y] & (1 << x)) ? fg : bg;
            st7789_set_pixel(px + x, py + y, color);
        }
    display_unlock();
}

void shell_draw_string(const char* str, int col, int row, uint16_t fg, uint16_t bg) {
    int c = col;
    while (*str) {
        shell_draw_char(*str++, c++, row, fg, bg);
        if (c >= COLS) break;
    }
}

void shell_clear() {
    st7789_fill(SHELL_BG);
    cursor_col = 0;
    cursor_row = 0;
}

void shell_print(const char* str) {
    while (*str) {
        if (*str == '\n') {
            cursor_col = 0;
            cursor_row++;
        } else {
            shell_draw_char(*str, cursor_col, cursor_row, SHELL_FG, SHELL_BG);
            cursor_col++;
            if (cursor_col >= COLS) {
                cursor_col = 0;
                cursor_row++;
            }
        }
        str++;
    }
}

void shell_println(const char* str) {
    shell_print(str);
    shell_print("\n");
}

static void handle_command(const char* cmd) {
    if (strcmp(cmd, "help") == 0) {
        shell_println("Befehle:");
        shell_println("  help    - Hilfe");
        shell_println("  clear   - Bildschirm");
        shell_println("  hello   - Begruessing");
        shell_println("  color   - Farb-Test");
        shell_println("  info    - System Info");
        shell_println("  tasks   - Task Manager");
        shell_println("  pong    - Pong Spiel");
        shell_println("  inv     - Space Invaders");
    } else if (strcmp(cmd, "clear") == 0) {
        shell_clear();
    } else if (strcmp(cmd, "hello") == 0) {
        shell_println("Hallo von SassanidBytes!");
        shell_println("RP2040 @ 125MHz");
    } else if (strcmp(cmd, "color") == 0) {
        st7789_fill_rect(0,   0,  80, 135, COLOR_RED);
        st7789_fill_rect(80,  0,  80, 135, COLOR_GREEN);
        st7789_fill_rect(160, 0,  80, 135, COLOR_BLUE);
        sleep_ms(2000);
        shell_clear();
        shell_println("Farb-Test fertig!");
    } else if (strcmp(cmd, "info") == 0) {
        shell_println("PicoKernel v2.0");
        shell_println("Display: ST7789");
        shell_println("240x135 RGB565");
        shell_println("SPI @ 62.5MHz");
    } else if (strcmp(cmd, "tasks") == 0) {
        shell_println("Laufende Tasks:");
        shell_println("  [0] shell    RUNNING  Core0");
        shell_println("  [1] heartbt  RUNNING  Core1");
        char buf[32];
        snprintf(buf, sizeof(buf), "  Tasks: %d/%d",
                 scheduler_task_count(), MAX_TASKS);
        shell_println(buf);
    } else if (strcmp(cmd, "pong") == 0) {
        pong_run();
        shell_clear();
        shell_println("PicoKernel v2.0");
        shell_println("---------------");
        shell_println("'help' fuer Hilfe");
        shell_println("");
        shell_print("> ");
    } else if (strlen(cmd) == 0) {
        // nichts tun

    } else if (strcmp(cmd, "inv") == 0) {
        invaders_run();
        shell_clear();
        shell_println("PicoKernel v2.0");
        shell_println("---------------");
        shell_println("'help' fuer Hilfe");
        shell_println("");
        shell_print("> ");
    } else {
        shell_print("Unbekannt: ");
        shell_println(cmd);
    }
}

void shell_run() {
    char buf[64];
    int  pos = 0;

    shell_clear();
    shell_println("PicoKernel v2.0");
    shell_println("---------------");
    shell_println("'help' fuer Hilfe");
    shell_println("");
    shell_print("> ");

    while (1) {
        int c = getchar_timeout_us(0);
        if (c == PICO_ERROR_TIMEOUT) continue;

        if (c == '\r' || c == '\n') {
            buf[pos] = '\0';
            shell_print("\n");
            handle_command(buf);
            pos = 0;
            shell_print("> ");
        } else if (c == 127 && pos > 0) {
            pos--;
            cursor_col--;
            shell_draw_char(' ', cursor_col, cursor_row, SHELL_FG, SHELL_BG);
        } else if (pos < 63) {
            buf[pos++] = (char)c;
            shell_draw_char((char)c, cursor_col, cursor_row, SHELL_FG, SHELL_BG);
            cursor_col++;
        }
    }
}