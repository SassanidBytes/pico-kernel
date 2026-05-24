#include "storage.h"
#include "hardware/flash.h"
#include "hardware/sync.h"
#include "pico/stdlib.h"
#include <string.h>
#include <stdio.h>

// Flash is XIP (Execute In Place) — read directly via pointer
#define FLASH_BASE    0x10000000
#define STORAGE_ADDR  (FLASH_BASE + FLASH_TARGET_OFFSET)

static uint32_t calc_checksum(password_entry_t* entries, int count) {
    uint32_t sum = 0;
    for (int i = 0; i < count; i++) {
        for (int j = 0; j < MAX_NAME_LEN; j++)
            sum += entries[i].name[j];
        for (int j = 0; j < MAX_PASS_LEN; j++)
            sum += entries[i].password[j];
    }
    return sum;
}

void storage_init() {
    // Do nothing — flash is always available
}

bool storage_save(password_entry_t* entries, int count) {
    flash_storage_t data;
    memset(&data, 0, sizeof(data));

    data.magic    = FLASH_MAGIC;
    data.count    = count;
    data.checksum = calc_checksum(entries, count);
    memcpy(data.entries, entries, sizeof(password_entry_t) * count);

    // Disable interrupts during flash write operation
    uint32_t interrupts = save_and_disable_interrupts();

    // Erase flash sector (minimum 4 KB)
    flash_range_erase(FLASH_TARGET_OFFSET, FLASH_SECTOR_SIZE);

    // Write data (256-byte blocks)
    flash_range_program(FLASH_TARGET_OFFSET,
                        (const uint8_t*)&data,
                        sizeof(flash_storage_t));

    restore_interrupts(interrupts);
    return true;
}

bool storage_load(password_entry_t* entries, int* count) {
    const flash_storage_t* data = (const flash_storage_t*)STORAGE_ADDR;

    // Check magic value
    if (data->magic != FLASH_MAGIC) {
        *count = 0;
        return false;
    }

    // Verify checksum
    uint32_t cs = calc_checksum((password_entry_t*)data->entries, data->count);
    if (cs != data->checksum) {
        *count = 0;
        return false;
    }

    *count = data->count;
    memcpy(entries, data->entries, sizeof(password_entry_t) * data->count);
    return true;
}

void storage_get_info(uint32_t* used_kb, uint32_t* free_kb) {
    // Estimate kernel size (~200 KB)
    *used_kb = 200;
    *free_kb = 2048 - 200;
}

uint32_t storage_get_password_count() {
    const flash_storage_t* data = (const flash_storage_t*)STORAGE_ADDR;
    if (data->magic != FLASH_MAGIC) return 0;
    return data->count;
}