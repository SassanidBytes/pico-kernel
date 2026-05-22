
#include "invaders.h"
#include "st7789.h"
#include "mutex.h"
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include <string.h>
extern volatile bool pong_active;

// ─── Spielzustand ────────────────────────────────────────
static int  player_x;
static int  player_lives;
static int  score;

static bool aliens[ALIEN_ROWS][ALIEN_COLS];
static int  alien_x;       // Offset der ganzen Alien-Gruppe
static int  alien_y;
static int  alien_dir;     // 1 = rechts, -1 = links
static int  alien_speed;   // ms zwischen Bewegungen
static int  alien_timer;

// Spieler Schuss
static int  bullet_x, bullet_y;
static bool bullet_active;

// Enemy Schüsse
static int  ebullet_x[MAX_ENEMY_BULLETS];
static int  ebullet_y[MAX_ENEMY_BULLETS];
static bool ebullet_active[MAX_ENEMY_BULLETS];
static int  ebullet_timer;

static bool game_over;
static bool player_won;

// ─── Hilfsfunktionen ─────────────────────────────────────
static void draw_rect(int x, int y, int w, int h, uint16_t color) {
    if (x < 0 || y < 0 || x + w > INV_W || y + h > INV_H) return;
    display_lock();
    st7789_fill_rect(x, y, w, h, color);
    display_unlock();
}

static int count_aliens() {
    int count = 0;
    for (int r = 0; r < ALIEN_ROWS; r++)
        for (int c = 0; c < ALIEN_COLS; c++)
            if (aliens[r][c]) count++;

    return count;
}

// ─── Zeichnen ────────────────────────────────────────────
static void draw_alien(int r, int c, uint16_t color) {
    int x = alien_x + c * (ALIEN_W + ALIEN_PAD_X);
    int y = alien_y + r * (ALIEN_H + ALIEN_PAD_Y);
    draw_rect(x, y, ALIEN_W, ALIEN_H, color);
}

static void draw_player(uint16_t color) {
    draw_rect(player_x, PLAYER_Y, PLAYER_W, PLAYER_H, color);
}

static void draw_hud() {
    draw_rect(0, 0, INV_W, 8, INV_BG);
    // Leben
    for (int i = 0; i < player_lives; i++)
        draw_rect(2 + i * 8, 1, 6, 6, INV_CYAN);
    // Score Balken
    int sw = score * 2;
    if (sw > 100) sw = 100;
    draw_rect(80, 2, sw, 4, INV_YELLOW);
}

static void draw_field() {
    display_lock();
    st7789_fill(INV_BG);
    display_unlock();
    draw_hud();
    draw_player(INV_CYAN);
    for (int r = 0; r < ALIEN_ROWS; r++)
        for (int c = 0; c < ALIEN_COLS; c++)
            if (aliens[r][c])
                draw_alien(r, c, INV_GREEN);
}

// ─── Init ────────────────────────────────────────────────
static void init_game() {
    player_x     = INV_W / 2 - PLAYER_W / 2;
    player_lives = 3;
    score        = 0;
    alien_x      = 10;
    alien_y      = 12;
    alien_dir    = 1;
    alien_speed  = 400;
    alien_timer  = 0;
    bullet_active = false;
    ebullet_timer = 0;
    game_over    = false;
    player_won   = false;

    for (int r = 0; r < ALIEN_ROWS; r++)
        for (int c = 0; c < ALIEN_COLS; c++)
            aliens[r][c] = true;

    memset(ebullet_active, 0, sizeof(ebullet_active));
}

// ─── KI Schuss ───────────────────────────────────────────
static void enemy_shoot() {
    // Zufälliger Alien schießt
    for (int i = 0; i < MAX_ENEMY_BULLETS; i++) {
        if (!ebullet_active[i]) {
            // Wähle zufällige Spalte mit lebendem Alien
            for (int c = 0; c < ALIEN_COLS; c++) {
                int col = (alien_timer + c + i) % ALIEN_COLS;
                for (int r = ALIEN_ROWS - 1; r >= 0; r--) {
                    if (aliens[r][col]) {
                        ebullet_x[i] = alien_x + col * (ALIEN_W + ALIEN_PAD_X) + ALIEN_W/2;
                        ebullet_y[i] = alien_y + r * (ALIEN_H + ALIEN_PAD_Y) + ALIEN_H;
                        ebullet_active[i] = true;
                        goto done;
                    }
                }
            }
            done:;
            break;
        }
    }
}

// ─── Haupt-Loop ──────────────────────────────────────────
void invaders_run() {
    pong_active = true;
    gpio_init(INV_BTN_A); gpio_set_dir(INV_BTN_A, GPIO_IN); gpio_pull_up(INV_BTN_A);
    gpio_init(INV_BTN_B); gpio_set_dir(INV_BTN_B, GPIO_IN); gpio_pull_up(INV_BTN_B);
    gpio_init(INV_BTN_X); gpio_set_dir(INV_BTN_X, GPIO_IN); gpio_pull_up(INV_BTN_X);
    gpio_init(INV_BTN_Y); gpio_set_dir(INV_BTN_Y, GPIO_IN); gpio_pull_up(INV_BTN_Y);
    sleep_ms(300);

    init_game();
    draw_field();

    while (!game_over) {
        // ── Beenden ──
        if (!gpio_get(INV_BTN_Y) && !gpio_get(INV_BTN_A)) {
            pong_active = false;
            sleep_ms(300);
            if (!gpio_get(INV_BTN_Y) && !gpio_get(INV_BTN_A)) return;
        }

        // ── Input ──
        if (!gpio_get(INV_BTN_A)) {
            draw_player(INV_BG);
            player_x -= PLAYER_SPEED;
            if (player_x < 0) player_x = 0;
            draw_player(INV_CYAN);
        }
        if (!gpio_get(INV_BTN_B)) {
            draw_player(INV_BG);
            player_x += PLAYER_SPEED;
            if (player_x + PLAYER_W > INV_W) player_x = INV_W - PLAYER_W;
            draw_player(INV_CYAN);
        }
        if (!gpio_get(INV_BTN_X) && !bullet_active) {
            bullet_x = player_x + PLAYER_W / 2;
            bullet_y = PLAYER_Y - BULLET_H;
            bullet_active = true;
        }

        // ── Spieler Schuss ──
        if (bullet_active) {
            draw_rect(bullet_x, bullet_y, BULLET_W, BULLET_H, INV_BG);
            bullet_y -= BULLET_SPEED;
            if (bullet_y < 8) {
                bullet_active = false;
            } else {
                // Treffer?
                for (int r = 0; r < ALIEN_ROWS; r++) {
                    for (int c = 0; c < ALIEN_COLS; c++) {
                        if (!aliens[r][c]) continue;
                        int ax = alien_x + c * (ALIEN_W + ALIEN_PAD_X);
                        int ay = alien_y + r * (ALIEN_H + ALIEN_PAD_Y);
                        if (bullet_x >= ax && bullet_x <= ax + ALIEN_W &&
                            bullet_y >= ay && bullet_y <= ay + ALIEN_H) {
                            draw_alien(r, c, INV_BG);
                            aliens[r][c] = false;
                            bullet_active = false;
                            score++;
                            draw_hud();
                            // Schneller werden
                            alien_speed = 400 - count_aliens() * 10;
                            if (alien_speed < 80) alien_speed = 80;
                            if (count_aliens() == 0) {
                                player_won = true;
                                game_over  = true;
                            }
                        }
                    }
                }
                if (bullet_active)
                    draw_rect(bullet_x, bullet_y, BULLET_W, BULLET_H, INV_RED);
            }
        }

        // ── Enemy Schüsse ──
        ebullet_timer++;
        if (ebullet_timer > 30) {
            ebullet_timer = 0;
            enemy_shoot();
        }
        for (int i = 0; i < MAX_ENEMY_BULLETS; i++) {
            if (!ebullet_active[i]) continue;
            draw_rect(ebullet_x[i], ebullet_y[i], BULLET_W, BULLET_H, INV_BG);
            ebullet_y[i] += 2;
            if (ebullet_y[i] > INV_H) {
                ebullet_active[i] = false;
                continue;
            }
            // Treffer Spieler?
            if (ebullet_x[i] >= player_x &&
                ebullet_x[i] <= player_x + PLAYER_W &&
                ebullet_y[i] >= PLAYER_Y) {
                ebullet_active[i] = false;
                draw_player(INV_BG);
                player_lives--;
                draw_hud();
                sleep_ms(500);
                draw_player(INV_CYAN);
                if (player_lives <= 0) game_over = true;
                continue;
            }
            draw_rect(ebullet_x[i], ebullet_y[i], BULLET_W, BULLET_H, INV_YELLOW);
        }

        // ── Aliens bewegen ──
        alien_timer++;
        if (alien_timer * 16 > alien_speed) {
            alien_timer = 0;

            // Alte Position löschen
            for (int r = 0; r < ALIEN_ROWS; r++)
                for (int c = 0; c < ALIEN_COLS; c++)
                    if (aliens[r][c]) draw_alien(r, c, INV_BG);

            alien_x += alien_dir * 3;

            // Rand erreicht?
            int min_x = alien_x;
            int max_x = alien_x + ALIEN_COLS * (ALIEN_W + ALIEN_PAD_X);
            if (max_x > INV_W - 5 || min_x < 5) {
                alien_dir *= -1;
                alien_y   += 4;
                // Aliens unten angekommen?
                if (alien_y + ALIEN_ROWS * (ALIEN_H + ALIEN_PAD_Y) > PLAYER_Y)
                    game_over = true;
            }

            // Neu zeichnen
            for (int r = 0; r < ALIEN_ROWS; r++)
                for (int c = 0; c < ALIEN_COLS; c++)
                    if (aliens[r][c]) draw_alien(r, c, INV_GREEN);
        }

        sleep_ms(16);
    }

    // ── Game Over / Win ──
    display_lock();
    st7789_fill(INV_BG);
    display_unlock();
    pong_active = false;
}