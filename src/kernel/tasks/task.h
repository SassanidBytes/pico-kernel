#ifndef TASK_H
#define TASK_H

#include <stdint.h>

#define MAX_TASKS     8
#define STACK_SIZE    1024

// Task Status
typedef enum {
    TASK_READY,
    TASK_RUNNING,
    TASK_BLOCKED,
    TASK_DEAD
} task_state_t;

// Task Struktur
typedef struct {
    uint32_t    id;
    const char* name;
    void        (*func)(void);   // Funktion die der Task ausführt
    task_state_t state;
    uint32_t    stack[STACK_SIZE];
    uint32_t    runtime_ms;      // Wie lange läuft der Task schon
} task_t;

#endif