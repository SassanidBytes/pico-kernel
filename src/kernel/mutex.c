#include "mutex.h"

mutex_t display_mutex;

void mutex_display_init() {
    mutex_init(&display_mutex);
}

void display_lock() {
    mutex_enter_blocking(&display_mutex);
}

void display_unlock() {
    mutex_exit(&display_mutex);
}