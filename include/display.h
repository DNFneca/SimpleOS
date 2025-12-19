#pragma once

#include <stdint.h>
#include "limine.h"

void put_pixel(struct limine_framebuffer *fb, uint32_t x, uint32_t y, uint32_t color);
void draw_line(struct limine_framebuffer *fb, int x0, int y0, int x1, int y1, uint32_t color);
void draw_star(struct limine_framebuffer *fb, int cx, int cy, int outer_radius, int inner_radius, uint32_t color);