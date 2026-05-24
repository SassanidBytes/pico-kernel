#include "passmanager.h"
#include "st7789.h"
#include "shell.h"
#include "mutex.h"
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include <string.h>
#include <stdio.h>
#include "storage.h"

extern volatile bool pong_active;

// Button Pins
#define BTN_A  12
#define BTN_B  13
#define BTN_X  14
#define BTN_Y  15

// Farben
#define PM_BG    0x0000  // Schwarz
#define PM_FG    0xFFFF  // Weiss
#define PM_GREEN 0x07E0  // Grün
#define PM_RED   0xF800  // Rot
#define PM_GOLD  0xFFE0  // Gold

// Demo Passwörter (XOR verschlüsselt)
static password_entry_t passwords[MAX_PASSWORDS] = {
    {"GitHub",    "SassanidBytes", "\x3d\x2a\x1f\x4b\x2c\x3e\x1a", true},
    {"Gmail",     "sassanid@mail", "\x2b\x1c\x4a\x3f\x2e\x1b\x4c", true},
    {"Discord",   "SassanidBytes", "\x4e\x2d\x1a\x3b\x2c\x4f\x1e", true},
    {"Instagram", "sassanid_byte", "\x1f\x3c\x2a\x4d\x1b\x3e\x2c", true},
    {""          , "",             "",                               false},
    {""          , "",             "",                               false},
    {""          , "",             "",                               false},
    {""          , "",             "",                               false},
};

static int selected = 0;

// XOR Verschlüsselung/Entschlüsselung
static void xor_crypt(const char* in, char* out, int len) {
    for (int i = 0; i < len; i++)
        out[i] = in[i] ^ PASS_XOR_KEY;
    out[len] = '\0';
}

static int count_passwords() {
    int n = 0;
    for (int i = 0; i < MAX_PASSWORDS; i++)
        if (passwords[i].active) n++;
    return n;
}

static void draw_passmanager(int sel) {
    display_lock();
    st7789_fill(PM_BG);
    display_unlock();

    // Titel
    display_lock();
    st7789_fill_rect(0, 0, 240, 12, PM_GOLD);
    display_unlock();
    shell_draw_string("PASSWORD MANAGER", 1, 0, PM_BG, PM_GOLD);

    // Passwort Liste
    int row = 2;
    for (int i = 0; i < MAX_PASSWORDS; i++) {
        if (!passwords[i].active) continue;
        uint16_t fg = PM_FG;
        uint16_t bg = PM_BG;
        if (i == sel) {
            fg = PM_BG;
            bg = PM_GREEN;
            display_lock();
            st7789_fill_rect(0, row * 10, 240, 10, PM_GREEN);
            display_unlock();
        }
        shell_draw_string(passwords[i].name,     1,  row, fg, bg);
        shell_draw_string(passwords[i].username, 10, row, fg, bg);
        row++;
    }

    // Steuerung unten
    shell_draw_string("A/B:Scroll X:Select Y:Exit", 0, 12, PM_GOLD, PM_BG);
}

void passmanager_add() {
    char name[MAX_NAME_LEN]  = {0};
    char user[MAX_NAME_LEN]  = {0};
    char pass[MAX_PASS_LEN]  = {0};
    int  pos = 0;

    // Name eingeben
    display_lock();
    st7789_fill(PM_BG);
    display_unlock();
    shell_draw_string("Neues Passwort", 0, 0, PM_GOLD, PM_BG);
    shell_draw_string("Name:", 0, 2, PM_GREEN, PM_BG);
    shell_print("> ");

    pos = 0;
    while (1) {
        int c = getchar_timeout_us(0);
        if (c == PICO_ERROR_TIMEOUT) continue;
        if (c == '\r' || c == '\n') break;
        if (c == 127 && pos > 0) { pos--; continue; }
        if (pos < MAX_NAME_LEN - 1) name[pos++] = (char)c;
    }
    name[pos] = '\0';

    // Username eingeben
    shell_draw_string("User:", 0, 4, PM_GREEN, PM_BG);
    shell_print("> ");
    pos = 0;
    while (1) {
        int c = getchar_timeout_us(0);
        if (c == PICO_ERROR_TIMEOUT) continue;
        if (c == '\r' || c == '\n') break;
        if (c == 127 && pos > 0) { pos--; continue; }
        if (pos < MAX_NAME_LEN - 1) user[pos++] = (char)c;
    }
    user[pos] = '\0';

    // Passwort eingeben
    shell_draw_string("Pass:", 0, 6, PM_GREEN, PM_BG);
    shell_print("> ");
    pos = 0;
    while (1) {
        int c = getchar_timeout_us(0);
        if (c == PICO_ERROR_TIMEOUT) continue;
        if (c == '\r' || c == '\n') break;
        if (c == 127 && pos > 0) { pos--; continue; }
        if (pos < MAX_PASS_LEN - 1) pass[pos++] = (char)c;
    }
    pass[pos] = '\0';

    // Speichern
    int count = 0;
    storage_load(passwords, &count);
    if (count < MAX_PASSWORDS) {
        strncpy(passwords[count].name,     name, MAX_NAME_LEN);
        strncpy(passwords[count].username, user, MAX_NAME_LEN);
        // XOR verschlüsseln
        for (int i = 0; i < (int)strlen(pass); i++)
            passwords[count].password[i] = pass[i] ^ PASS_XOR_KEY;
        passwords[count].active = true;
        count++;
        storage_save(passwords, count);
        shell_draw_string("Gespeichert!", 0, 9, PM_GREEN, PM_BG);
    } else {
        shell_draw_string("Voll! Max 8", 0, 9, PM_RED, PM_BG);
    }
    sleep_ms(1500);
}

void passmanager_delete() {
    int count = 0;
    storage_load(passwords, &count);
    if (count == 0) {
        shell_println("Keine Passwoerter!");
        return;
    }

    display_lock();
    st7789_fill(PM_BG);
    display_unlock();
    shell_draw_string("Loeschen: A/B=Scroll", 0, 0, PM_GOLD, PM_BG);
    shell_draw_string("X=Loeschen Y=Abbruch", 0, 1, PM_GOLD, PM_BG);

    int sel = 0;
    draw_passmanager(sel);

    while (1) {
        if (!gpio_get(BTN_Y)) { sleep_ms(200); return; }
        if (!gpio_get(BTN_A)) {
            sel = (sel - 1 + count) % count;
            draw_passmanager(sel);
            sleep_ms(200);
        }
        if (!gpio_get(BTN_B)) {
            sel = (sel + 1) % count;
            draw_passmanager(sel);
            sleep_ms(200);
        }
        if (!gpio_get(BTN_X)) {
            // Eintrag löschen
            for (int i = sel; i < count - 1; i++)
                passwords[i] = passwords[i + 1];
            memset(&passwords[count-1], 0, sizeof(password_entry_t));
            count--;
            storage_save(passwords, count);
            shell_draw_string("Geloescht!", 0, 10, PM_RED, PM_BG);
            sleep_ms(1000);
            return;
        }
        sleep_ms(50);
    }
}

static void show_password(int idx) {
    char decrypted[MAX_PASS_LEN];
    xor_crypt(passwords[idx].password,
              decrypted,
              strlen(passwords[idx].password));

    display_lock();
    st7789_fill(PM_BG);
    display_unlock();

    shell_draw_string(passwords[idx].name,     0, 0, PM_GOLD, PM_BG);
    shell_draw_string("User:", 0, 2, PM_GREEN, PM_BG);
    shell_draw_string(passwords[idx].username, 5, 2, PM_FG,   PM_BG);
    shell_draw_string("Pass:", 0, 4, PM_RED,   PM_BG);
    shell_draw_string(decrypted,               5, 4, PM_FG,   PM_BG);
    shell_draw_string("X=Tippen Y=Zurueck",    0, 12, PM_GOLD, PM_BG);

    // Warten auf Input
    while (1) {
        if (!gpio_get(BTN_Y)) {
            sleep_ms(200);
            return;
        }
        if (!gpio_get(BTN_X)) {
            // Passwort via USB "tippen"
            sleep_ms(200);
            printf("%s", decrypted);
            shell_draw_string("Gesendet!   ", 5, 6, PM_GREEN, PM_BG);
            sleep_ms(1000);
            return;
        }
        sleep_ms(50);
    } 
}

void passmanager_run() {
    pong_active = true;

    gpio_init(BTN_A); gpio_set_dir(BTN_A, GPIO_IN); gpio_pull_up(BTN_A);
    gpio_init(BTN_B); gpio_set_dir(BTN_B, GPIO_IN); gpio_pull_up(BTN_B);
    gpio_init(BTN_X); gpio_set_dir(BTN_X, GPIO_IN); gpio_pull_up(BTN_X);
    gpio_init(BTN_Y); gpio_set_dir(BTN_Y, GPIO_IN); gpio_pull_up(BTN_Y);
    sleep_ms(300);

    // Passwörter aus Flash laden
    int count = 0;
    if (!storage_load(passwords, &count)) {
        // Kein Flash Inhalt → Demo Passwörter bleiben
    }

    selected = 0;
    draw_passmanager(selected);

    while (1) {
        // Y = zurück
        if (!gpio_get(BTN_Y)) {
            sleep_ms(200);
            pong_active = false;
            return;
        }

        // A = hoch
        if (!gpio_get(BTN_A)) {
            selected--;
            if (selected < 0) selected = count_passwords() - 1;
            draw_passmanager(selected);
            sleep_ms(200);
        }

        // B = runter
        if (!gpio_get(BTN_B)) {
            selected++;
            if (selected >= count_passwords()) selected = 0;
            draw_passmanager(selected);
            sleep_ms(200);
        }

        // X = auswählen
        if (!gpio_get(BTN_X)) {
            sleep_ms(200);
            show_password(selected);
            draw_passmanager(selected);
        }

        sleep_ms(50);
    }
}