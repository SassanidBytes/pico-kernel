#ifndef PASSMANAGER_H
#define PASSMANAGER_H

#include <stdint.h>
#include <stdbool.h>

#define MAX_PASSWORDS    8
#define MAX_NAME_LEN     16
#define MAX_PASS_LEN     32
#define PASS_XOR_KEY     0x5A  // Einfache XOR Verschlüsselung

// Passwort Eintrag
typedef struct {
    char name[MAX_NAME_LEN];      // z.B. "GitHub"
    char username[MAX_NAME_LEN];  // z.B. "SassanidBytes"
    char password[MAX_PASS_LEN];  // Verschlüsselt gespeichert
    bool active;
} password_entry_t;

// Funktionen
void passmanager_run();

#endif