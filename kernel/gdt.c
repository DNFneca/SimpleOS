#include "../include/gdt.h"

// 5 Entries: Null, K-Code, K-Data, U-Data, U-Code
gdt_entry_t gdt_entries[5];
gdt_ptr_t gdt_ptr;

extern void gdt_flush(uint64_t); // Assembly function

static void gdt_set_gate(int32_t num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran) {
    gdt_entries[num].base_low    = (base & 0xFFFF);
    gdt_entries[num].base_middle = (base >> 16) & 0xFF;
    gdt_entries[num].base_high   = (base >> 24) & 0xFF;

    gdt_entries[num].limit_low   = (limit & 0xFFFF);
    gdt_entries[num].granularity = (limit >> 16) & 0x0F;

    gdt_entries[num].granularity |= gran & 0xF0;
    gdt_entries[num].access      = access;
}

void init_gdt() {
    gdt_ptr.limit = (sizeof(gdt_entry_t) * 5) - 1;
    gdt_ptr.base  = (uint64_t)& gdt_entries;

    // 0: Null Descriptor
    gdt_set_gate(0, 0, 0, 0, 0);

    // 1: Kernel Code (Ring 0)
    // Access: 0x9A (Exec/Read, Ring 0)
    // Granularity: 0xA0 (L=1 for 64-bit) | 0x0F (Limit high) = 0xAF
    gdt_set_gate(1, 0, 0, 0x9A, 0xA0);

    // 2: Kernel Data (Ring 0)
    // Access: 0x92 (Read/Write, Ring 0)
    // Granularity: 0xC0 (Standard 32-bit limit) | 0x0F = 0xCF
    // Note: Data segments don't need L-bit set.
    gdt_set_gate(2, 0, 0, 0x92, 0xC0);

    // 3: User Data (Ring 3) - ORDER MATTERS FOR SYSCALL!
    // Access: 0xF2 (Read/Write, Ring 3)
    gdt_set_gate(3, 0, 0, 0xF2, 0xC0);

    // 4: User Code (Ring 3)
    // Access: 0xFA (Exec/Read, Ring 3)
    // Granularity: 0xA0 (L=1 for 64-bit) | 0x0F = 0xAF
    gdt_set_gate(4, 0, 0, 0xFA, 0xA0);

    gdt_flush((uint64_t)&gdt_ptr);
}