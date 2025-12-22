#include "../include/memory.h"
#include <stdbool.h>

// Static heap pointer - set once during heap_init
static void *g_heap_start = NULL;
static size_t g_heap_size = 0;

void *memcpy(void *dest, const void *src, size_t n) {
    uint8_t *d = dest;
    const uint8_t *s = src;
    while (n--) {
        *d++ = *s++;
    }
    return dest;
}

void *memset(void *dest, int value, size_t n) {
    uint8_t *d = dest;
    uint8_t v = (uint8_t)value;
    while (n--) {
        *d++ = v;
    }
    return dest;
}

void *memmove(void *dest, const void *src, size_t n) {
    uint8_t *d = dest;
    const uint8_t *s = src;

    if (d < s) {
        while (n--) *d++ = *s++;
    } else {
        d += n;
        s += n;
        while (n--) *--d = *--s;
    }

    return dest;
}

int memcmp(const void *a, const void *b, size_t n) {
    const uint8_t *x = a;
    const uint8_t *y = b;

    while (n--) {
        if (*x != *y)
            return *x - *y;
        x++;
        y++;
    }
    return 0;
}

void *slab_alloc(slab_t *slab) {
    if (!slab->free_list)
        return NULL;

    void *obj = slab->free_list;
    slab->free_list = *(void **)obj;
    return obj;
}

void slab_free(slab_t *slab, void *obj) {
    *(void **)obj = slab->free_list;
    slab->free_list = obj;
}

void heap_init(void *heap_start, size_t heap_size) {
    if (heap_start == NULL || heap_size < BLOCK_SIZE) {
        return; // Invalid parameters
    }

    // Store heap start globally
    g_heap_start = heap_start;
    g_heap_size = heap_size;

    heap_block_t *initial_block = (heap_block_t *)heap_start;
    initial_block->size = heap_size;
    initial_block->free = true;
    initial_block->next = NULL;
    initial_block->prev = NULL;
}

static heap_block_t *find_free_block(size_t size) {
    heap_block_t *current = (heap_block_t *)g_heap_start;

    while (current != NULL) {
        if (current->free && current->size >= size) {
            return current;
        }
        current = current->next;
    }

    return NULL;
}

static void split_block(heap_block_t *block, size_t size) {
    // Only split if remainder is large enough for header + some data
    if (block->size >= size + BLOCK_SIZE + ALIGN_SIZE) {
        heap_block_t *new_block = (heap_block_t *)((uint8_t *)block + size);
        new_block->size = block->size - size;
        new_block->free = true;
        new_block->next = block->next;
        new_block->prev = block;

        if (block->next != NULL) {
            block->next->prev = new_block;
        }

        block->next = new_block;
        block->size = size;
    }
}

static void coalesce(heap_block_t *block) {
    // Coalesce with next block
    if (block->next != NULL && block->next->free) {
        block->size += block->next->size;
        block->next = block->next->next;

        if (block->next != NULL) {
            block->next->prev = block;
        }
    }

    // Coalesce with previous block
    if (block->prev != NULL && block->prev->free) {
        block->prev->size += block->size;
        block->prev->next = block->next;

        if (block->next != NULL) {
            block->next->prev = block->prev;
        }
    }
}

void *slab_malloc(size_t size) {
    int idx = slab_index(size);
    if (idx < 0)
        return NULL;

    slab_t *slab = &slabs[idx];

    // No free objects → allocate a new slab page
    if (!slab->free_list) {
        size_t page_size = 4096;
        uint8_t *page = malloc(page_size);  // Use malloc instead of heap_malloc
        if (!page)
            return NULL;

        slab->obj_size = slab_sizes[idx];

        size_t count = page_size / slab->obj_size;
        for (size_t i = 0; i < count; i++) {
            void *obj = page + i * slab->obj_size;
            *(void **)obj = slab->free_list;
            slab->free_list = obj;
        }
    }

    void *obj = slab->free_list;
    slab->free_list = *(void **)obj;
    return obj;
}

void *malloc(size_t size) {
    if (size == 0) return NULL;

    // Check if heap is initialized
    if (g_heap_start == NULL) {
        return NULL;
    }

    // Align and add header size
    size_t total_size = ALIGN16(size + BLOCK_SIZE);

    // Find suitable free block
    heap_block_t *block = find_free_block(total_size);

    if (block == NULL) {
        return NULL; // Out of memory
    }

    // Split block if possible
    split_block(block, total_size);

    // Mark as used
    block->free = false;

    // Return pointer after header
    return (void *)((uint8_t *)block + BLOCK_SIZE);
}

void *calloc(size_t nmemb, size_t size) {
    size_t total = nmemb * size;
    void *ptr = malloc(total);
    if (!ptr)
        return NULL;

    memset(ptr, 0, total);
    return ptr;
}

void *realloc(void *ptr, size_t size) {
    if (!ptr)
        return malloc(size);

    if (size == 0) {
        free(ptr);
        return NULL;
    }

    heap_block_t *block = (heap_block_t *)((uint8_t *)ptr - BLOCK_SIZE);

    if (block->size - BLOCK_SIZE >= size)
        return ptr;

    void *new_ptr = malloc(size);
    if (!new_ptr)
        return NULL;

    memcpy(new_ptr, ptr, block->size - BLOCK_SIZE);
    free(ptr);

    return new_ptr;
}

void free(void *ptr) {
    if (ptr == NULL) return;

    // Get block header
    heap_block_t *block = (heap_block_t *)((uint8_t *)ptr - BLOCK_SIZE);

    // Mark as free
    block->free = true;

    // Coalesce with adjacent free blocks
    coalesce(block);
}

void heap_stats(heap_stats_t *stats) {
    stats->total_blocks = 0;
    stats->free_blocks = 0;
    stats->used_blocks = 0;
    stats->total_free_memory = 0;
    stats->total_used_memory = 0;

    heap_block_t *current = (heap_block_t *)g_heap_start;

    while (current != NULL) {
        stats->total_blocks++;

        if (current->free) {
            stats->free_blocks++;
            stats->total_free_memory += current->size - BLOCK_SIZE;
        } else {
            stats->used_blocks++;
            stats->total_used_memory += current->size - BLOCK_SIZE;
        }

        current = current->next;
    }
}