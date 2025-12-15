#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "../include/console.h"
#include "../include/keyboard.h"
#include "../include/readline.h"
#include "../include/registry.h"

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
    console_init();
    console_write("Simple OS\nType 'help'\n\n");


//	init_commands();

    char input[READLINE_BUFFER];
    char* argv[MAX_ARGS];
    int argc;

    while (true) {
        readline(input, READLINE_BUFFER);   // Read full line with editing, history, arrows
        if (kstrlen(input) == 0)
            continue;

        // Split input into args
        argc = split_args(input, argv, MAX_ARGS);

        // Execute command
//        execute_command(argc, argv);

		for (int i = 0; i < READLINE_BUFFER; i++) input[i] = 0;

    }
}
