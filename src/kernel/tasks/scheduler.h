#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "task.h"

// Scheduler initialisieren
void scheduler_init();

// Task hinzufügen
int scheduler_add_task(const char* name, void (*func)(void));

// Nächsten Task ausführen (Round-Robin)
void scheduler_run();

// Aktuellen Task blockieren (wartet)
void scheduler_yield();

// Task Info anzeigen
void scheduler_print_tasks();

// Anzahl aktiver Tasks
int scheduler_task_count();

#endif