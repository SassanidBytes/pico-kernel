#ifndef TOTP_H
#define TOTP_H

#include <stdint.h>

// TOTP Einstellungen
#define TOTP_INTERVAL  30  // Sekunden pro Code

// Funktionen
uint32_t totp_generate(const uint8_t* key, int key_len, uint64_t timestamp);
void     totp_run();

#endif