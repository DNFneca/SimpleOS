#include <stdint.h>
#include "../include/gdt.h"

// GDT Entry structure
struct gdt_entry {
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t base_middle;
    uint8_t access;
    uint8_t granularity;
    uint8_t base_high;
} __attribute__((packed));

// GDT Pointer structure
struct gdt_ptr {
    uint16_t limit;
    uint64_t base;
} __attribute__((packed));

// GDT with 5 entries: null, kernel code, kernel data, user code, user data
struct gdt_entry gdt[5];
struct gdt_ptr gp;

// External assembly function to load GDT
extern void gdt_flush(uint64_t);

// Set a GDT entry
static void gdt_set_gate(int num, uint64_t base, uint64_t limit, uint8_t access, uint8_t gran) {
    gdt[num].base_low = (base & 0xFFFF);
    gdt[num].base_middle = (base >> 16) & 0xFF;
    gdt[num].base_high = (base >> 24) & 0xFF;

    gdt[num].limit_low = (limit & 0xFFFF);
    gdt[num].granularity = (limit >> 16) & 0x0F;
    gdt[num].granularity |= gran & 0xF0;
    gdt[num].access = access;
}

void gdt_init(void) {
    gp.limit = (sizeof(struct gdt_entry) * 5) - 1;
    gp.base = (uint64_t)&gdt;

    // NULL descriptor
    gdt_set_gate(0, 0, 0, 0, 0);

    // Kernel code segment (0x08)
    // Base = 0, Limit = 0xFFFFFFFF
    // Access: Present, Ring 0, Code segment, Executable, Readable
    gdt_set_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF);

    // Kernel data segment (0x10)
    // Base = 0, Limit = 0xFFFFFFFF
    // Access: Present, Ring 0, Data segment, Writable
    gdt_set_gate(2, 0, 0xFFFFFFFF, 0x92, 0xCF);

    // User code segment (0x18)
    // Access: Present, Ring 3, Code segment, Executable, Readable
    gdt_set_gate(3, 0, 0xFFFFFFFF, 0xFA, 0xCF);

    // User data segment (0x20)
    // Access: Present, Ring 3, Data segment, Writable
    gdt_set_gate(4, 0, 0xFFFFFFFF, 0xF2, 0xCF);

    gdt_flush((uint64_t)&gp);
}