#include "totp.h"
#include "st7789.h"
#include "shell.h"
#include "mutex.h"
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include <string.h>
#include <stdio.h>

extern volatile bool pong_active;

#define BTN_Y  15

// Demo TOTP Key (in echtem System geheim halten!)
static const uint8_t totp_key[] = {
    0x53, 0x61, 0x73, 0x73, 0x61, 0x6e, 0x69, 0x64
};

// Einfacher HMAC-SHA1 für TOTP
static uint32_t hmac_sha1_simple(const uint8_t* key, int klen, uint64_t counter) {
    // Vereinfachte Version für Demo
    uint32_t hash = 0x5A5A5A5A;
    for (int i = 0; i < klen; i++) {
        hash ^= key[i] << (i % 32);
        hash += counter & 0xFF;
        hash = (hash << 7) | (hash >> 25);
        counter >>= 8;
    }
    return hash % 1000000;  // 6-stelliger Code
}

uint32_t totp_generate(const uint8_t* key, int key_len, uint64_t timestamp) {
    uint64_t counter = timestamp / TOTP_INTERVAL;
    return hmac_sha1_simple(key, key_len, counter);
}

static void draw_totp(uint32_t code, uint32_t seconds_left) {
    display_lock();
    st7789_fill(0x0000);
    display_unlock();

    // Titel
    display_lock();
    st7789_fill_rect(0, 0, 240, 12, 0xFFE0);
    display_unlock();
    shell_draw_string("2FA TOKEN", 4, 0, 0x0000, 0xFFE0);

    // Code gross anzeigen
    char code_str[16];
    snprintf(code_str, sizeof(code_str), "%06lu", (unsigned long)code);

    // Jede Ziffer einzeln gross
    for (int i = 0; i < 6; i++) {
        char digit[2] = {code_str[i], '\0'};
        shell_draw_string(digit, 3 + i * 4, 4, 0x07E0, 0x0000);
    }

    // Timer Balken
    display_lock();
    st7789_fill_rect(0, 90, 240, 8, 0x1082);
    int bar_w = (seconds_left * 240) / TOTP_INTERVAL;
    uint16_t bar_color = seconds_left > 10 ? 0x07E0 : 0xF800;
    st7789_fill_rect(0, 90, bar_w, 8, bar_color);
    display_unlock();

    // Sekunden
    char sec_str[16];
    snprintf(sec_str, sizeof(sec_str), "Neu in: %lus", (unsigned long)seconds_left);
    shell_draw_string(sec_str, 0, 11, 0xFFFF, 0x0000);
    shell_draw_string("Y=Zurueck", 16, 12, 0xFFE0, 0x0000);
}

void totp_run() {
    pong_active = true;
    gpio_init(BTN_Y); gpio_set_dir(BTN_Y, GPIO_IN); gpio_pull_up(BTN_Y);
    sleep_ms(300);

    // Startzeit simulieren (echter Pico braucht RTC oder NTP)
    uint64_t base_time = 1700000000ULL;  // Unix timestamp
    uint64_t start_ms  = time_us_64() / 1000;

    while (1) {
        if (!gpio_get(BTN_Y)) {
            sleep_ms(200);
            pong_active = false;
            return;
        }

        uint64_t now      = base_time + (time_us_64() / 1000 - start_ms) / 1000;
        uint32_t code     = totp_generate(totp_key, sizeof(totp_key), now);
        uint32_t sec_left = TOTP_INTERVAL - (now % TOTP_INTERVAL);

        draw_totp(code, sec_left);
        sleep_ms(500);
    }
}