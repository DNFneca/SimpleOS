#include "../include/console.h"
#include "../include/io.h"

static uint16_t* vga = (uint16_t*)0xB8000;
int row = 0, col = 0;
const int WIDTH = CONSOLE_WIDTH;
const int HEIGHT = CONSOLE_HEIGHT;
static uint8_t color = 0x0F; // white text

void console_update_cursor() {
    unsigned short pos = row * WIDTH + col;
    outb(0x3D4, 14);
    outb(0x3D5, (pos >> 8) & 0xFF);
    outb(0x3D4, 15);
    outb(0x3D5, pos & 0xFF);
}

// Move hardware cursor
static void move_cursor() {
    uint16_t pos = row * WIDTH + col;
    outb(0x3D4, 14);
    outb(0x3D5, (pos >> 8) & 0xFF);
    outb(0x3D4, 15);
    outb(0x3D5, pos & 0xFF);
}

void console_putc_at(char c, int r, int c_pos) {
    if (r >= 0 && r < HEIGHT && c_pos >= 0 && c_pos < WIDTH) {
        vga[r * WIDTH + c_pos] = (uint16_t)(c | (color << 8));
    }
}

// Scroll screen up by one row
static void scroll() {
    for (int y = 1; y < HEIGHT; y++)
        for (int x = 0; x < WIDTH; x++)
            vga[(y - 1) * WIDTH + x] = vga[y * WIDTH + x];

    // Clear last row
    for (int x = 0; x < WIDTH; x++)
        vga[(HEIGHT - 1) * WIDTH + x] = (uint16_t)(' ' | (color << 8));

    if (row > 0) row--;
}

// Clear the screen
void console_clear() {
    for (int y = 0; y < HEIGHT; y++)
        for (int x = 0; x < WIDTH; x++)
            vga[y * WIDTH + x] = (uint16_t)(' ' | (color << 8));

    row = col = 0;
    move_cursor();
}

// Initialize console
void console_init() {
    console_clear();
}

// Put a single character
void console_putc(char c) {
    switch (c) {
        case '\n':
            col = 0;
            row++;
            break;

        case '\r':
            col = 0;
            break;

        case '\b': // Backspace
            if (col > 0) {
                col--;
                vga[row * WIDTH + col] = (uint16_t)(' ' | (color << 8));
            }
            break;

        default:
            vga[row * WIDTH + col] = (uint16_t)(c | (color << 8));
            col++;
            break;
    }

    if (col >= WIDTH) {
        col = 0;
        row++;
    }

    if (row >= HEIGHT) {
        scroll();
        row = HEIGHT - 1;
    }

    move_cursor();
}

// Write a string
void console_write(const char* s) {
    while (*s)
        console_putc(*s++);
}
