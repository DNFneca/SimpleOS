#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

// Define the starting address and size of the kernel heap
// NOTE: These values MUST be determined by your kernel's memory map
#define HEAP_START_ADDRESS 0x100000 // 1MB - where kernel code typically ends
#define HEAP_SIZE          0x400000 // 4MB heap space (adjust as needed)

// Heap Block Header structure
typedef struct heap_block {
    struct heap_block* next; // Pointer to the next free block in the linked list
    struct heap_block* prev; // Pointer to the previous free block in the linked list
    size_t size;             // Size of the block (header + usable space)
    bool is_free;            // True if the block is currently free
} heap_block_t;

// Public function prototypes
void heap_init();
void* malloc(size_t size);
void free(void* ptr);