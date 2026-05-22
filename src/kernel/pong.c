#include "pong.h"
#include "st7789.h"
#include "mutex.h"
#include "pico/stdlib.h"
#include "hardware/gpio.h"

// Button Pins (Pimoroni Pico Display)
#define BTN_A  12
#define BTN_B  13
#define BTN_X  14
#define BTN_Y  15

// Spielzustand
static int ball_x, ball_y;
static int ball_dx, ball_dy;
static int pad_l_y, pad_r_y;
static int score_l, score_r;

// ─── Zeichnen ────────────────────────────────────────────

static void draw_rect(int x, int y, int w, int h, uint16_t color) {
    display_lock();
    st7789_fill_rect(x, y, w, h, color);
    display_unlock();
}

static void draw_middleline() {
    for (int y = PONG_TOP; y < PONG_BOT; y += 6) {
        display_lock();
        st7789_fill_rect(118, y, 4, 3, PONG_GRAY);
        display_unlock();
    }
}

static void draw_score() {
    // Score Hintergrund
    display_lock();
    st7789_fill_rect(0, 0, PONG_W, PONG_TOP, PONG_BG);
    display_unlock();

    // Zahlen als einfache Balken
    // Links
    for (int i = 0; i < score_l && i < 9; i++) {
        display_lock();
        st7789_fill_rect(90 + i * 6, 1, 4, 6, PONG_FG);
        display_unlock();
    }
    // Rechts
    for (int i = 0; i < score_r && i < 9; i++) {
        display_lock();
        st7789_fill_rect(130 + i * 6, 1, 4, 6, PONG_FG);
        display_unlock();
    }
}

static void reset_ball() {
    ball_x  = PONG_W / 2;
    ball_y  = PONG_H / 2;
    ball_dx = (score_l + score_r) % 2 == 0 ? 2 : -2;
    ball_dy = 2;
}

static void init_game() {
    pad_l_y = PONG_H / 2 - PAD_H / 2;
    pad_r_y = PONG_H / 2 - PAD_H / 2;
    score_l = 0;
    score_r = 0;
    reset_ball();
}

static void draw_field() {
    display_lock();
    st7789_fill(PONG_BG);
    display_unlock();
    draw_middleline();
    draw_score();
}

// ─── KI ──────────────────────────────────────────────────

static void update_ai() {
    int center = pad_r_y + PAD_H / 2;
    if (ball_dx > 0) {  // Ball kommt auf KI zu
        if (center < ball_y - 2) pad_r_y += 2;
        else if (center > ball_y + 2) pad_r_y -= 2;
    } else {
        // Ball geht weg → zurück zur Mitte
        if (center < PONG_H/2 - 2) pad_r_y += 1;
        else if (center > PONG_H/2 + 2) pad_r_y -= 1;
    }
    // Grenzen
    if (pad_r_y < PONG_TOP) pad_r_y = PONG_TOP;
    if (pad_r_y + PAD_H > PONG_BOT) pad_r_y = PONG_BOT - PAD_H;
}

// ─── Haupt-Loop ──────────────────────────────────────────

void pong_run() {
    // Buttons initialisieren
    gpio_init(BTN_A); gpio_set_dir(BTN_A, GPIO_IN); gpio_pull_up(BTN_A);
    gpio_init(BTN_B); gpio_set_dir(BTN_B, GPIO_IN); gpio_pull_up(BTN_B);
    gpio_init(BTN_X); gpio_set_dir(BTN_X, GPIO_IN); gpio_pull_up(BTN_X);
    gpio_init(BTN_Y); gpio_set_dir(BTN_Y, GPIO_IN); gpio_pull_up(BTN_Y);

    init_game();
    draw_field();

    while (1) {
        // ── Input ──
        // A = hoch, B = runter
        if (!gpio_get(BTN_A)) pad_l_y -= PAD_SPEED;
        if (!gpio_get(BTN_B)) pad_l_y += PAD_SPEED;

        // X+Y gleichzeitig = zurück zur Shell
        if (!gpio_get(BTN_X) && !gpio_get(BTN_Y)) return;

        // Grenzen linker Schläger
        if (pad_l_y < PONG_TOP) pad_l_y = PONG_TOP;
        if (pad_l_y + PAD_H > PONG_BOT) pad_l_y = PONG_BOT - PAD_H;

        // ── KI ──
        update_ai();

        // ── Ball bewegen ──
        // Alte Positionen löschen
        draw_rect(PAD_L_X, PONG_TOP, PAD_W, PONG_BOT - PONG_TOP, PONG_BG);
        draw_rect(PAD_R_X, PONG_TOP, PAD_W, PONG_BOT - PONG_TOP, PONG_BG);
        draw_rect(ball_x, ball_y, BALL_SIZE, BALL_SIZE, PONG_BG);

        ball_x += ball_dx;
        ball_y += ball_dy;

        // Oben/Unten abprallen
        if (ball_y <= PONG_TOP) { ball_dy =  2; ball_y = PONG_TOP; }
        if (ball_y + BALL_SIZE >= PONG_BOT) { ball_dy = -2; ball_y = PONG_BOT - BALL_SIZE; }

        // Linker Schläger
        if (ball_x <= PAD_L_X + PAD_W &&
            ball_y + BALL_SIZE >= pad_l_y &&
            ball_y <= pad_l_y + PAD_H) {
            ball_dx =  2;
            ball_x  = PAD_L_X + PAD_W + 1;
        }

        // Rechter Schläger
        if (ball_x + BALL_SIZE >= PAD_R_X &&
            ball_y + BALL_SIZE >= pad_r_y &&
            ball_y <= pad_r_y + PAD_H) {
            ball_dx = -2;
            ball_x  = PAD_R_X - BALL_SIZE - 1;
        }

        // Punkt links
        if (ball_x < 0) {
            score_r++;
            draw_score();
            reset_ball();
            sleep_ms(500);
            draw_middleline();
        }

        // Punkt rechts
        if (ball_x > PONG_W) {
            score_l++;
            draw_score();
            reset_ball();
            sleep_ms(500);
            draw_middleline();
        }

        // ── Zeichnen ──
        draw_rect(PAD_L_X, pad_l_y, PAD_W, PAD_H, PONG_FG);
        draw_rect(PAD_R_X, pad_r_y, PAD_W, PAD_H, PONG_FG);
        draw_rect(ball_x,  ball_y,  BALL_SIZE, BALL_SIZE, PONG_FG);

        sleep_ms(16); // ~60fps
    }
}