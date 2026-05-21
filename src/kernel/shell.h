#ifndef SHELL_H
#define SHELL_H

#include <stdint.h>

// Text Farben
#define SHELL_FG  0xFFFF  // Weiss
#define SHELL_BG  0x001F  // Blau

// Terminal Groesse
#define COLS  26   // Zeichen pro Zeile  (240 / 9)
#define ROWS  13   // Zeilen             (135 / 10)

// Funktionen
void shell_init();
void shell_draw_char(char c, int col, int row, uint16_t fg, uint16_t bg);
void shell_draw_string(const char* str, int col, int row, uint16_t fg, uint16_t bg);
void shell_clear();
void shell_print(const char* str);
void shell_println(const char* str);
void shell_run();

#endif