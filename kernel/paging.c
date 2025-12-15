#include <stdint.h>
#include "../include/paging.h"

// Page directory and page table structures
#define PAGE_SIZE 4096
#define TABLE_SIZE 1024
#define DIRECTORY_SIZE 1024

// Align to 4KB boundary
uint64_t page_directory[DIRECTORY_SIZE] __attribute__((aligned(4096)));
uint64_t first_page_table[TABLE_SIZE] __attribute__((aligned(4096)));

// Helper function to enable paging
static inline void enable_paging(void) {
    uint64_t cr0, cr4, efer;

    /* Load PML4 physical address into CR3 */
    asm volatile (
            "mov %0, %%cr3"
            :
            : "r"(&page_directory)
            : "memory"
            );

    /* Enable PAE (CR4.PAE = bit 5) */
    asm volatile ("mov %%cr4, %0" : "=r"(cr4));
    cr4 |= (1ULL << 5);
    asm volatile ("mov %0, %%cr4" :: "r"(cr4));

    /* Enable Long Mode (EFER.LME = bit 8) */
    asm volatile (
            "rdmsr"
            : "=a"(efer)
            : "c"(0xC0000080)
            : "rdx"
            );
    efer |= (1ULL << 8);
    asm volatile (
            "wrmsr"
            :
            : "c"(0xC0000080), "a"(efer)
            : "rdx"
            );

    /* Enable paging (CR0.PG = bit 31) */
    asm volatile ("mov %%cr0, %0" : "=r"(cr0));
    cr0 |= (1ULL << 31);
    asm volatile ("mov %0, %%cr0" :: "r"(cr0));

    /* Paging is now active — next step is a far jump */
}

void paging_init(void) {
    // Clear page directory
    for (int i = 0; i < DIRECTORY_SIZE; i++) {
        page_directory[i] = 0x00000002; // Not present, read/write
    }

    // Identity map the first 4MB (0x00000000 - 0x003FFFFF)
    // This covers kernel code, VGA memory, etc.
    for (int i = 0; i < TABLE_SIZE; i++) {
        // Map each 4KB page
        // Physical address | Present | Read/Write
        first_page_table[i] = (i * PAGE_SIZE) | PAGE_PRESENT | PAGE_WRITE;
    }

    // Put the first page table into the page directory
    page_directory[0] = ((uint64_t)first_page_table) | PAGE_PRESENT | PAGE_WRITE;

    // Enable paging
    enable_paging();
}

void paging_map_page(uint64_t virt, uint64_t phys, uint64_t flags) {
    uint64_t pd_index = virt >> 22;                    // Top 10 bits
    uint64_t pt_index = (virt >> 12) & 0x3FF;         // Middle 10 bits

    // Get page table address from directory
    uint64_t* page_table = (uint64_t*)(page_directory[pd_index] & ~0xFFF);

    // Map the page
    page_table[pt_index] = (phys & ~0xFFF) | flags;

    // Flush TLB for this page
    asm volatile("invlpg (%0)" : : "r"(virt) : "memory");
}

void* paging_get_page_directory(void) {
    return (void*)page_directory;
}