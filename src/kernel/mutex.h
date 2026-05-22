#ifndef MUTEX_H
#define MUTEX_H

#include "pico/mutex.h"

// Globaler Display Mutex
extern mutex_t display_mutex;

// Initialisieren
void mutex_display_init();

// Display sperren (warten bis frei)
void display_lock();

// Display freigeben
void display_unlock();

#endif