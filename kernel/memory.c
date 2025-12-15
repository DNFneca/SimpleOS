#include "../include/memory.h"
#include "../include/string.h"  // Assuming you have strcmp, you also need memset/memcpy

// Global pointer to the head of the FREE list
static heap_block_t* free_list_head = NULL;

// Helper to initialize the heap space (requires memset/memcpy)
void heap_init() {
    // 1. Point the free list head to the start of the reserved memory
    free_list_head = (heap_block_t*)HEAP_START_ADDRESS;

    // 2. Initialize the first and only block to cover the entire heap space
    free_list_head->size = HEAP_SIZE;
    free_list_head->is_free = true;
    free_list_head->next = NULL;
    free_list_head->prev = NULL;

    // Optional: Clear the entire heap memory to zero
//    memset((void*)HEAP_START_ADDRESS, 0, HEAP_SIZE);
}

// --- MALLOC Implementation (First-Fit Algorithm) ---
void* malloc(size_t size) {
    if (size == 0) return NULL;

    // The block must be large enough for the requested data PLUS the block header
    size_t total_required_size = size + sizeof(heap_block_t);

    // 1. Search the free list for a suitable block (First-Fit)
    heap_block_t *current = free_list_head;
    heap_block_t *best_fit = NULL;

    while (current != NULL) {
        if (current->is_free && current->size >= total_required_size) {
            best_fit = current;
            break;
        }
        current = current->next;
    }

    if (best_fit == NULL) {
        // No block large enough found
        return NULL;
    }

    // 2. Splitting the block if the remainder is large enough
    size_t remaining_size = best_fit->size - total_required_size;

    // Check if the remainder is large enough to hold a new header plus minimal data
    if (remaining_size <= sizeof(heap_block_t)) {
        // If no split occurs, simply remove the whole block from the free list
        if (best_fit->prev != NULL) {
            best_fit->prev->next = best_fit->next;
        } else {
            // The block was the head, so move the head pointer
            free_list_head = best_fit->next;
        }
        if (best_fit->next != NULL) {
            best_fit->next->prev = best_fit->prev;
        }
        // Size remains best_fit->size (it includes the small remainder)

        // 3. Mark the block as allocated
        best_fit->is_free = false;

        // 4. Return the usable memory address (just after the header)
        return (void*)((uintptr_t)best_fit + sizeof(heap_block_t));

    }

    // --- Split the block ---

    // 2a. Create the new free block header (the remainder)
    heap_block_t *new_free_block = (heap_block_t *) ((uintptr_t) best_fit + total_required_size);
    new_free_block->size = remaining_size;
    new_free_block->is_free = true;

    // 2b. Insert the new free block into the free list (replace best_fit)
    new_free_block->next = best_fit->next;
    new_free_block->prev = best_fit->prev;
    if (new_free_block->prev != NULL) {
        new_free_block->prev->next = new_free_block;
    } else {
        // New block is the new head of the free list
        free_list_head = new_free_block;
    }
    if (new_free_block->next != NULL) {
        new_free_block->next->prev = new_free_block;
    }

    // 2c. Adjust the allocated block's size
    best_fit->size = total_required_size;



    // 3. Mark the block as allocated
    best_fit->is_free = false;

    // 4. Return the usable memory address (just after the header)
    return (void *) ((uintptr_t) best_fit + sizeof(heap_block_t));
}

// --- FREE Implementation (Block Coalescing) ---
void free(void* ptr) {
    if (ptr == NULL) return;

    // 1. Get the block header address from the user-provided pointer
    heap_block_t* block_to_free = (heap_block_t*)((uintptr_t)ptr - sizeof(heap_block_t));

    if (block_to_free->is_free) {
        // Double-free detected (or corrupt header)
        return;
    }

    // 2. Mark the block as free
    block_to_free->is_free = true;

    // 3. Simple insertion into the free list (e.g., insert at head for simplicity)
    block_to_free->next = free_list_head;
    block_to_free->prev = NULL;

    if (free_list_head != NULL) {
        free_list_head->prev = block_to_free;
    }
    free_list_head = block_to_free;

    // 4. Coalescing (Merging adjacent free blocks - OPTIONAL but critical for performance)
    // The most basic version skips coalescing, but a proper implementation would check
    // the blocks immediately before and after this one in the *physical* memory layout
    // (not the free list) and merge them if they are also free.
}