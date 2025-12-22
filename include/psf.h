#pragma once

#include <stdint.h>
#include <stddef.h>
#include "limine.h"

#define PSF1_MAGIC 0x0436
#define PSF2_MAGIC 0x864ab572

typedef struct {
    uint16_t magic;
    uint8_t mode;
    uint8_t charsize;
} __attribute__((packed)) psf1_header_t;

typedef struct {
    uint32_t magic;
    uint32_t version;
    uint32_t headersize;
    uint32_t flags;
    uint32_t numglyph;
    uint32_t bytesperglyph;
    uint32_t height;
    uint32_t width;
} __attribute__((packed)) psf2_header_t;

typedef struct {
    uint8_t* glyph_buffer;
    uint32_t glyph_width;
    uint32_t glyph_height;
    uint32_t bytes_per_glyph;
    uint32_t num_glyphs;
    uint8_t psf_version;
} psf_font_t;

int psf_load(const void* font_data, size_t font_size, psf_font_t* font);
void psf_draw_char(psf_font_t* font, struct limine_framebuffer *framebuffer, uint32_t fb_width, uint32_t fb_height, uint32_t fb_pitch, char c, uint32_t x, uint32_t y, uint32_t fg_color, uint32_t bg_color);
void psf_draw_string(psf_font_t* font, struct limine_framebuffer *framebuffer, uint32_t fb_width, uint32_t fb_height, uint32_t fb_pitch, const char* str, uint32_t x, uint32_t y, uint32_t fg_color, uint32_t bg_color);