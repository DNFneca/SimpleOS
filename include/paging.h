#pragma once

#include <stdint.h>

static inline void enable_paging(void);
void paging_init(void);
void paging_map_page(uint64_t virt, uint64_t phys, uint64_t flags);
void* paging_get_page_directory(void);

// Page flags
#define PAGE_PRESENT    0x1
#define PAGE_WRITE      0x2
#define PAGE_USER       0x4