#include "../include/memory.h"

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

static void split_block(heap_block_t *block, size_t size) {
    if (block->size <= size + sizeof(heap_block_t))
        return;

    heap_block_t *new_block =
            (heap_block_t *)((uint8_t *)block + sizeof(heap_block_t) + size);

    new_block->size = block->size - size - sizeof(heap_block_t);
    new_block->free = 1;
    new_block->next = block->next;
    new_block->prev = block;

    if (new_block->next)
        new_block->next->prev = new_block;

    block->size = size;
    block->next = new_block;
}

static void coalesce(heap_block_t *block) {
    // Merge with next
    if (block->next && block->next->free) {
        block->size += sizeof(heap_block_t) + block->next->size;
        block->next = block->next->next;
        if (block->next)
            block->next->prev = block;
    }

    // Merge with previous
    if (block->prev && block->prev->free) {
        block->prev->size += sizeof(heap_block_t) + block->size;
        block->prev->next = block->next;
        if (block->next)
            block->next->prev = block->prev;
    }
}

static void *heap_end = NULL;

void heap_init(void *heap_start, size_t heap_size) {
    heap_head = (heap_block_t *)heap_start;
    heap_head->size = heap_size - BLOCK_SIZE;
    heap_head->free = 1;
    heap_head->next = NULL;

    heap_end = (uint8_t *)heap_start + heap_size;
}

static heap_block_t *find_free_block(size_t size) {
    heap_block_t *current = heap_head;
    while (current) {
        if (current->free && current->size >= size)
            return current;
        current = current->next;
    }
    return NULL;
}

static int slab_index(size_t size) {
    for (int i = 0; i < SLAB_COUNT; i++) {
        if (size <= slab_sizes[i])
            return i;
    }
    return -1;
}

void *heap_malloc(size_t size) {
    heap_block_t *block = find_free_block(size);
    if (!block)
        return NULL; // later: grow heap via paging

    block->free = 0;
    split_block(block, size);

    return (uint8_t *)block + sizeof(heap_block_t);
}

void *slab_malloc(size_t size) {
    int idx = slab_index(size);
    if (idx < 0)
        return NULL;

    slab_t *slab = &slabs[idx];

    // No free objects → allocate a new slab page
    if (!slab->free_list) {
        size_t page_size = 4096;
        uint8_t *page = heap_malloc(page_size);
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
    if (size == 0)
        return NULL;

    size = ALIGN8(size);

    if (size <= SLAB_MAX_SIZE)
        return slab_malloc(size);

    return heap_malloc(size);
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

    heap_block_t *block =
            (heap_block_t *)((uint8_t *)ptr - BLOCK_SIZE);

    if (block->size >= size)
        return ptr;

    void *new_ptr = malloc(size);
    if (!new_ptr)
        return NULL;

    memcpy(new_ptr, ptr, block->size);
    free(ptr);

    return new_ptr;
}

void free(void *ptr) {
    if (!ptr)
        return;

    heap_block_t *block =
            (heap_block_t *)((uint8_t *)ptr - BLOCK_SIZE);

    block->free = 1;
    coalesce(block);
}
