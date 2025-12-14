#pragma once
#include <stdint.h>

void console_clear();
void console_init();
void console_putc(char c);
void console_write(const char* s);
void console_putc_at(char c, int r, int c_pos);
void console_update_cursor();

// Expose cursor position
extern int row;
extern int col;

// Console size
#define CONSOLE_WIDTH 80
#define CONSOLE_HEIGHT 25