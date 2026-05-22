#ifndef INVADERS_H
#define INVADERS_H

#include <stdint.h>
#include <stdbool.h>

// Spielfeld
#define INV_W        240
#define INV_H        135

// Spieler
#define PLAYER_W     12
#define PLAYER_H     6
#define PLAYER_SPEED 3
#define PLAYER_Y     122

// Aliens
#define ALIEN_ROWS   3
#define ALIEN_COLS   8
#define ALIEN_W      10
#define ALIEN_H      7
#define ALIEN_PAD_X  4
#define ALIEN_PAD_Y  3

// Schüsse
#define BULLET_W     2
#define BULLET_H     5
#define BULLET_SPEED 4
#define MAX_ENEMY_BULLETS 3

// Farben
#define INV_BG       0x0000  // Schwarz
#define INV_FG       0xFFFF  // Weiss
#define INV_GREEN    0x07E0  // Aliens
#define INV_RED      0xF800  // Spieler Schuss
#define INV_YELLOW   0xFFE0  // Enemy Schuss
#define INV_CYAN     0x07FF  // Spieler

// Buttons
#define INV_BTN_A    12
#define INV_BTN_B    13
#define INV_BTN_X    14
#define INV_BTN_Y    15

// Funktion
void invaders_run();

#endif