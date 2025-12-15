#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>
#include <stdbool.h>
#include "../include/console.h"

// VGA text mode constants
#define VGA_MEMORY 0xB8000
#define VGA_WIDTH 80
#define VGA_HEIGHT 25

// Terminal state
static size_t terminal_row;
static size_t terminal_column;
static uint8_t terminal_color;
static uint16_t* terminal_buffer;

// Helper functions
static inline uint8_t vga_entry_color(uint8_t fg, uint8_t bg) {
return fg | bg << 4;
}

static inline uint16_t vga_entry(unsigned char c, uint8_t color) {
    return (uint16_t) c | (uint16_t) color << 8;
}

uint8_t console_makecolor(enum vga_color fg, enum vga_color bg) {
    return vga_entry_color(fg, bg);
}

void console_init(void) {
    terminal_row = 0;
    terminal_column = 0;
    terminal_color = vga_entry_color(7, 0); // Light grey on black
    terminal_buffer = (uint16_t*) VGA_MEMORY;
    console_clear();
}

void console_clear(void) {
    for (size_t y = 0; y < VGA_HEIGHT; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            const size_t index = y * VGA_WIDTH + x;
            terminal_buffer[index] = vga_entry(' ', terminal_color);
        }
    }
    terminal_row = 0;
    terminal_column = 0;
}

void console_setcolor(uint8_t color) {
    terminal_color = color;
}

static void terminal_putentryat(char c, uint8_t color, size_t x, size_t y) {
    const size_t index = y * VGA_WIDTH + x;
    terminal_buffer[index] = vga_entry(c, color);
}

static void terminal_scroll(void) {
    for (size_t y = 0; y < VGA_HEIGHT - 1; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            terminal_buffer[y * VGA_WIDTH + x] = terminal_buffer[(y + 1) * VGA_WIDTH + x];
        }
    }

    for (size_t x = 0; x < VGA_WIDTH; x++) {
        terminal_buffer[(VGA_HEIGHT - 1) * VGA_WIDTH + x] = vga_entry(' ', terminal_color);
    }

    terminal_row = VGA_HEIGHT - 1;
}

void console_putchar(char c) {
    if (c == '\n') {
        terminal_column = 0;
        if (++terminal_row == VGA_HEIGHT) {
            terminal_scroll();
        }
        return;
    }

    if (c == '\r') {
        terminal_column = 0;
        return;
    }

    if (c == '\t') {
        terminal_column = (terminal_column + 4) & ~3;
        if (terminal_column >= VGA_WIDTH) {
            terminal_column = 0;
            if (++terminal_row == VGA_HEIGHT) {
                terminal_scroll();
            }
        }
        return;
    }

    terminal_putentryat(c, terminal_color, terminal_column, terminal_row);

    if (++terminal_column == VGA_WIDTH) {
        terminal_column = 0;
        if (++terminal_row == VGA_HEIGHT) {
            terminal_scroll();
        }
    }
}

void console_write(const char* data, size_t size) {
    for (size_t i = 0; i < size; i++) {
        console_putchar(data[i]);
    }
}

void console_writestring(const char* data) {
    size_t len = 0;
    while (data[len]) len++;
    console_write(data, len);
}

// String length
static size_t strlen(const char* str) {
    size_t len = 0;
    while (str[len]) len++;
    return len;
}

// Reverse a string
static void reverse(char* str, int length) {
    int start = 0;
    int end = length - 1;
    while (start < end) {
        char temp = str[start];
        str[start] = str[end];
        str[end] = temp;
        start++;
        end--;
    }
}

// Convert integer to string
static int itoa(int num, char* str, int base) {
    int i = 0;
    bool is_negative = false;

    if (num == 0) {
        str[i++] = '0';
        str[i] = '\0';
        return i;
    }

    if (num < 0 && base == 10) {
        is_negative = true;
        num = -num;
    }

    while (num != 0) {
        int rem = num % base;
        str[i++] = (rem > 9) ? (rem - 10) + 'a' : rem + '0';
        num = num / base;
    }

    if (is_negative) {
        str[i++] = '-';
    }

    str[i] = '\0';
    reverse(str, i);

    return i;
}

// Convert unsigned integer to string
static int uitoa(unsigned int num, char* str, int base) {
    int i = 0;

    if (num == 0) {
        str[i++] = '0';
        str[i] = '\0';
        return i;
    }

    while (num != 0) {
        int rem = num % base;
        str[i++] = (rem > 9) ? (rem - 10) + 'a' : rem + '0';
        num = num / base;
    }

    str[i] = '\0';
    reverse(str, i);

    return i;
}

// Printf implementation
void printf(const char* format, ...) {
    va_list args;
    va_start(args, format);

    char buffer[32];

    for (size_t i = 0; format[i] != '\0'; i++) {
        if (format[i] == '%') {
            i++;

            // Handle format specifiers
            switch (format[i]) {
                case 'd': // Signed decimal integer
                case 'i': {
                    int val = va_arg(args, int);
                    itoa(val, buffer, 10);
                    console_writestring(buffer);
                    break;
                }

                case 'u': { // Unsigned decimal integer
                    unsigned int val = va_arg(args, unsigned int);
                    uitoa(val, buffer, 10);
                    console_writestring(buffer);
                    break;
                }

                case 'x': { // Hexadecimal (lowercase)
                    unsigned int val = va_arg(args, unsigned int);
                    uitoa(val, buffer, 16);
                    console_writestring(buffer);
                    break;
                }

                case 'X': { // Hexadecimal (uppercase)
                    unsigned int val = va_arg(args, unsigned int);
                    uitoa(val, buffer, 16);
                    for (int j = 0; buffer[j]; j++) {
                        if (buffer[j] >= 'a' && buffer[j] <= 'f') {
                            buffer[j] = buffer[j] - 'a' + 'A';
                        }
                    }
                    console_writestring(buffer);
                    break;
                }

                case 'p': { // Pointer address
                    console_writestring("0x");
                    unsigned int val = va_arg(args, unsigned int);
                    uitoa(val, buffer, 16);
                    console_writestring(buffer);
                    break;
                }

                case 'c': { // Character
                    char c = (char) va_arg(args, int);
                    console_putchar(c);
                    break;
                }

                case 's': { // String
                    const char* str = va_arg(args, const char*);
                    if (str == NULL) {
                        console_writestring("(null)");
                    } else {
                        console_writestring(str);
                    }
                    break;
                }

                case '%': { // Literal %
                    console_putchar('%');
                    break;
                }

                default:
                    console_putchar('%');
                    console_putchar(format[i]);
                    break;
            }
        } else {
            console_putchar(format[i]);
        }
    }

    va_end(args);
}