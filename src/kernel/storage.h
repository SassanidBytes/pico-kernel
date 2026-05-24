#ifndef STORAGE_H
#define STORAGE_H

#include <stdint.h>
#include <stdbool.h>
#include "passmanager.h"

// Flash memory layout
// The Pico has 2 MB of flash; i use the last 4 KB.
#define FLASH_TARGET_OFFSET  (1984 * 1024)  // 1984KB offset
#define FLASH_MAGIC          0xDEADBEEF    // Detection of whether the data is valid

typedef struct {
    uint32_t magic;
    uint32_t count;
    password_entry_t entries[MAX_PASSWORDS];
    uint32_t checksum;
} flash_storage_t;

// Functions
void     storage_init();
bool     storage_save(password_entry_t* entries, int count);
bool     storage_load(password_entry_t* entries, int* count);
void     storage_get_info(uint32_t* used_kb, uint32_t* free_kb);
uint32_t storage_get_password_count();

#endif