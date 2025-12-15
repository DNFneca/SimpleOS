#pragma once
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/* Alignment required by x86-64 ABI */
#define HEAP_ALIGNMENT 16

/* You MUST set these correctly */
#define HEAP_START_ADDRESS 0x100000
#define HEAP_SIZE          (1024 * 1024) /* 1 MiB */

/*
 * Heap block header.
 *
 * IMPORTANT RULE:
 * - size == usable payload size (NOT including header)
 */
typedef struct heap_block {
    size_t size;                 // usable bytes
    bool is_free;

    struct heap_block* next;     // free list only
    struct heap_block* prev;
} heap_block_t;

/* Public API */
void  heap_init(void);
void* malloc(size_t size);
void  free(void* ptr);
