#include "scheduler.h"
#include "pico/stdlib.h"
#include "hardware/timer.h"
#include <string.h>
#include <stdio.h>

static task_t   tasks[MAX_TASKS];
static int      task_count   = 0;
static int      current_task = 0;

void scheduler_init() {
    memset(tasks, 0, sizeof(tasks));
    task_count   = 0;
    current_task = 0;
}

int scheduler_add_task(const char* name, void (*func)(void)) {
    if (task_count >= MAX_TASKS) return -1;

    task_t* t   = &tasks[task_count];
    t->id        = task_count;
    t->name      = name;
    t->func      = func;
    t->state     = TASK_READY;
    t->runtime_ms = 0;

    task_count++;
    return t->id;
}

void scheduler_yield() {
    // Aktuellen Task als Ready markieren
    tasks[current_task].state = TASK_READY;

    // Nächsten Ready-Task finden (Round-Robin)
    int next = (current_task + 1) % task_count;
    int checked = 0;

    while (checked < task_count) {
        if (tasks[next].state == TASK_READY) {
            current_task = next;
            return;
        }
        next = (next + 1) % task_count;
        checked++;
    }
}

void scheduler_run() {
    if (task_count == 0) return;

    while (1) {
        task_t* t = &tasks[current_task];

        if (t->state == TASK_READY || t->state == TASK_RUNNING) {
            t->state = TASK_RUNNING;
            uint32_t start = time_us_32();

            // Task ausführen
            t->func();

            // Laufzeit tracken
            t->runtime_ms += (time_us_32() - start) / 1000;
        }

        // Nächster Task
        scheduler_yield();
    }
}

void scheduler_print_tasks() {
    printf("\nAktive Tasks:\n");
    for (int i = 0; i < task_count; i++) {
        task_t* t = &tasks[i];
        const char* state = "?";
        if (t->state == TASK_READY)   state = "READY";
        if (t->state == TASK_RUNNING) state = "RUN";
        if (t->state == TASK_BLOCKED) state = "BLOCKED";
        if (t->state == TASK_DEAD)    state = "DEAD";
        printf("[%d] %-12s %s  %lums\n", t->id, t->name, state, t->runtime_ms);
    }
}

int scheduler_task_count() {
    return task_count;
}