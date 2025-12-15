#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "../include/console.h"
#include "../include/keyboard.h"
#include "../include/readline.h"
#include "../include/memory.h"
#include "../include/gdt.h"

#define MAX_ARGS 16

__attribute__((section(".multiboot2_header")))
const unsigned long multiboot2_header[] =
{
    // magic number
    0xe85250d6,
    // architecture: 0 = 32-bit i386
    0x00000000,
    // header length
    24,
    // checksum (magic + arch + length + checksum = 0)
    -(0xe85250d6 + 0x00000000 + 24),

    // end tag
    0x00000000,
    0x00000000
};

extern void init_commands();
extern void execute_command(int argc, char* argv[]);


static inline uint8_t inb(uint16_t port) {
    uint8_t r;
    __asm__ volatile ("inb %1, %0" : "=a"(r) : "dN"(port));
    return r;
}


void wait_key(char* out) {
    // Uses BIOS/keyboard buffer via port I/O
    // This polling method works under GRUB
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
        // Skip leading spaces
        while (*p == ' ') p++;

        if (*p == '\0') break;

        // Start of argument
        argv[argc++] = p;
        if (argc >= max_args) break;

        // Find end of argument
        while (*p != '\0' && *p != ' ') p++;

        if (*p == ' ') *p++ = '\0'; // null-terminate argument
    }

    return argc;
}

void _start() {

    heap_init();

    console_init();
    console_write("Simple OS\nType 'help'\n\n");


    char input[READLINE_BUFFER];
    char* argv[MAX_ARGS];
    int argc;

    int8_t first = malloc(sizeof(int8_t));
    int8_t second = malloc(sizeof(int8_t));
    int8_t third = malloc(sizeof(int8_t));
    int8_t fourth = malloc(sizeof(int8_t));

    console_write((char*) &first);
    console_write((char*) &second);
    console_write((char*) &third);
    console_write((char*) &fourth);

    free(second);
    free(third);
    int16_t tf = malloc(sizeof(tf));

    console_write((char*) &tf);

    while (true) {
        readline(input, READLINE_BUFFER);
        if (kstrlen(input) == 0)
            continue;

        // Split input into args
        argc = split_args(input, argv, MAX_ARGS);
    }
}
