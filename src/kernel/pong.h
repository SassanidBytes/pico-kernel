#ifndef PONG_H
#define PONG_H

#include <stdint.h>
extern volatile bool pong_active;

// Spielfeld
#define PONG_W      240
#define PONG_HEIGHT 135
#define PONG_TOP    8    // Oben reserviert für Score
#define PONG_BOT    127  // Unten

// Schläger
#define PAD_W       4
#define PAD_H       20
#define PAD_SPEED   3
#define PAD_L_X     8
#define PAD_R_X     228

// Ball
#define BALL_SIZE   4

// Farben
#define PONG_BG     0x0000  // Schwarz
#define PONG_FG     0xFFFF  // Weiss
#define PONG_GRAY   0x8410  // Grau für Mittellinie

// Funktionen
void pong_run();

#endif