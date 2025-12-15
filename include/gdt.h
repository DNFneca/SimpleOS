#pragma once

#include <stdint.h>

// Standard GDT Entry (8 bytes)
struct gdt_entry_struct {
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t  base_middle;
    uint8_t  access;        // Access flags (Ring level, Code/Data type)
    uint8_t  granularity;   // Flags (Long Mode bit lives here!)
    uint8_t  base_high;
} __attribute__((packed));

typedef struct gdt_entry_struct gdt_entry_t;

// GDT Pointer for lgdt instruction
struct gdt_ptr_struct {
    uint16_t limit;
    uint64_t base;          // 64-bit Base Address (changed from uint32_t)
} __attribute__((packed));

typedef struct gdt_ptr_struct gdt_ptr_t;

void init_gdt();