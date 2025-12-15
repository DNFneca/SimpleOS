#include <stdint.h>
#include <stdbool.h>
#include "../include/readline.h"
#include "../include/console.h"


#define MAX_ARGS 16

struct multiboot_tag {
    uint32_t type;
    uint32_t size;
};

struct multiboot_tag_framebuffer {
    uint32_t type;
    uint32_t size;
    uint64_t addr;
    uint32_t pitch;
    uint32_t width;
    uint32_t height;
    uint8_t bpp;
    uint8_t fb_type;
    uint16_t reserved;
};


extern void init_commands();
extern void execute_command(int argc, char* argv[]);

static inline uint8_t inb(uint16_t port) {
uint8_t r;
__asm__ volatile ("inb %1, %0" : "=a"(r) : "dN"(port));
return r;
}

static inline void outb(uint16_t port, uint8_t val) {
    __asm__ volatile ("outb %0, %1" : : "a"(val), "dN"(port));
}

void wait_key(char* out) {
    while (1) {
        uint8_t status = inb(0x64);
        if (status & 1) {
            uint8_t scancode = inb(0x60);

            if (scancode == 0x1C) { // Enter
                *out = '\n';
                return;
            }

            if (scancode >= 0x02 && scancode <= 0x0B) {
                char c = "1234567890"[scancode - 0x02];
                *out = c;
                return;
            }
        }
    }
}

int split_args(char* line, char** argv, int max_args) {
    int argc = 0;
    char* p = line;

    while (*p != '\0') {
        while (*p == ' ') p++;
        if (*p == '\0') break;

        argv[argc++] = p;
        if (argc >= max_args) break;

        while (*p != '\0' && *p != ' ') p++;
        if (*p == ' ') *p++ = '\0';
    }

    return argc;
}

//void kernel_main(uint64_t magic, uint64_t mbi) {
//    (void)magic;
//
//    struct multiboot_tag* tag = (struct multiboot_tag*)(mbi + 8);
//    struct multiboot_tag_framebuffer* fb = 0;
//
//    while (tag->type) {
//        if (tag->type == 8) {
//            fb = (struct multiboot_tag_framebuffer*)tag;
//            break;
//        }
//        tag = (struct multiboot_tag*)((uint8_t*)tag + ((tag->size + 7) & ~7));
//    }
//
//    if (!fb)
//        for (;;) __asm__("hlt");
//
//    uint32_t* buffer = (uint32_t*)(uint64_t)fb->addr;
//    uint32_t pitch = fb->pitch / 4;
//
//    // Clear screen
//    for (uint32_t y = 0; y < fb->height; y++)
//        for (uint32_t x = 0; x < fb->width; x++)
//            buffer[y * pitch + x] = 0x000000;
//
//    // White box
//    for (uint32_t y = 100; y < 200; y++)
//        for (uint32_t x = 100; x < 400; x++)
//            buffer[y * pitch + x] = 0xFFFFFF;
//}

void kernel_main() {
    console_init();

    printf("Hello, World!\n");
    printf("Number: %d\n", 42);
    printf("Hex: 0x%x\n", 255);
    printf("String: %s\n", "Hello");
    printf("Character: %c\n", 'A');
    printf("Pointer: %p\n", 0xDEADBEEF);
//    console_write("Simple OS - 64-bit Long Mode via GRUB\n");
//    console_write("Type 'help'\n\n");

//    // init_commands();
//
//    char input[READLINE_BUFFER];
//    char* argv[MAX_ARGS];
//    int argc;
//
//    while (true) {
//        readline(input, READLINE_BUFFER);
//        if (kstrlen(input) == 0)
//            continue;
//
//        argc = split_args(input, argv, MAX_ARGS);
//        // execute_command(argc, argv);
//
//        for (int i = 0; i < READLINE_BUFFER; i++) input[i] = 0;
//    }
}