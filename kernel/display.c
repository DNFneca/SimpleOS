#include "../include/display.h"

void put_pixel(struct limine_framebuffer *fb, uint32_t x, uint32_t y, uint32_t color) {
    if (x >= fb->width || y >= fb->height) return;

    uint32_t *fb_ptr = (uint32_t *)fb->address;
    fb_ptr[y * (fb->pitch / 4) + x] = color;
}

// Draw a line using Bresenham's algorithm
void draw_line(struct limine_framebuffer *fb, int x0, int y0, int x1, int y1, uint32_t color) {
    int dx = x1 - x0;
    int dy = y1 - y0;

    if (dx < 0) dx = -dx;
    if (dy < 0) dy = -dy;

    int sx = x0 < x1 ? 1 : -1;
    int sy = y0 < y1 ? 1 : -1;
    int err = dx - dy;

    while (1) {
        put_pixel(fb, x0, y0, color);

        if (x0 == x1 && y0 == y1) break;

        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x0 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y0 += sy;
        }
    }
}

// Draw a 5-pointed star
void draw_star(struct limine_framebuffer *fb, int cx, int cy, int outer_radius, int inner_radius, uint32_t color) {
    // Calculate 10 points (5 outer, 5 inner) of the star
    int points_x[10];
    int points_y[10];

    // Simple approximation using fixed angles
    // Outer points at 0°, 72°, 144°, 216°, 288°
    // Inner points at 36°, 108°, 180°, 252°, 324°

    // Point 0 (top, 0° = -90° from right)
    points_x[0] = cx;
    points_y[0] = cy - outer_radius;

    // Point 1 (inner right-top)
    points_x[1] = cx + (inner_radius * 59) / 100;
    points_y[1] = cy - (inner_radius * 81) / 100;

    // Point 2 (outer right)
    points_x[2] = cx + (outer_radius * 95) / 100;
    points_y[2] = cy - (outer_radius * 31) / 100;

    // Point 3 (inner right-bottom)
    points_x[3] = cx + (inner_radius * 36) / 100;
    points_y[3] = cy + (inner_radius * 26) / 100;

    // Point 4 (outer bottom-right)
    points_x[4] = cx + (outer_radius * 59) / 100;
    points_y[4] = cy + (outer_radius * 81) / 100;

    // Point 5 (inner bottom)
    points_x[5] = cx;
    points_y[5] = cy + inner_radius / 2;

    // Point 6 (outer bottom-left)
    points_x[6] = cx - (outer_radius * 59) / 100;
    points_y[6] = cy + (outer_radius * 81) / 100;

    // Point 7 (inner left-bottom)
    points_x[7] = cx - (inner_radius * 36) / 100;
    points_y[7] = cy + (inner_radius * 26) / 100;

    // Point 8 (outer left)
    points_x[8] = cx - (outer_radius * 95) / 100;
    points_y[8] = cy - (outer_radius * 31) / 100;

    // Point 9 (inner left-top)
    points_x[9] = cx - (inner_radius * 59) / 100;
    points_y[9] = cy - (inner_radius * 81) / 100;

    // Draw lines connecting the points
    for (int i = 0; i < 10; i++) {
        int next = (i + 1) % 10;
        draw_line(fb, points_x[i], points_y[i], points_x[next], points_y[next], color);
    }
}