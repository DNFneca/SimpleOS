#pragma once

#include <stddef.h>
#include <stdint.h>

// cpu_heap_t *heap = &cpu_heaps[get_cpu_id()]; get current cpu

#define MAX_CPUS 64
#define ALIGN8(x) (((x) + 7) & ~7)
#define BLOCK_SIZE sizeof(heap_block_t)
#define MAX_CPUS 64
#define SLAB_MAX_SIZE 256
#define SLAB_COUNT 6

static const size_t slab_sizes[SLAB_COUNT] = {
        8, 16, 32, 64, 128, 256
};

typedef struct slab {
    void *free_list;
    size_t obj_size;
} slab_t;

typedef struct heap_block {
    size_t size;
    int free;
    struct heap_block *next;
    struct heap_block *prev;
} heap_block_t;


typedef struct cpu_heap {
    heap_block_t *heap_head;
    slab_t slabs[SLAB_COUNT];
} cpu_heap_t;

static cpu_heap_t cpu_heaps[MAX_CPUS];

static slab_t slabs[SLAB_COUNT];
static heap_block_t *heap_head = NULL;




void *memcpy(void *dest, const void *src, size_t n);
void *memset(void *dest, int value, size_t n);
void *memmove(void *dest, const void *src, size_t n);
int memcmp(const void *a, const void *b, size_t n);

void heap_init(void *heap_start, size_t heap_size);

void *malloc(size_t size);
void *calloc(size_t nmemb, size_t size);
void *realloc(void *ptr, size_t size);
void free(void *ptr);
