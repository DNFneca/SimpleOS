#include "../include/psf.h"
#include "../include/display.h"

int psf_load(const void* font_data, size_t font_size, psf_font_t* font) {
    if (!font_data || !font || font_size < sizeof(psf1_header_t)) {
        return -1;
    }

    uint16_t magic = *(uint16_t*)font_data;

    if (magic == PSF1_MAGIC) {
        psf1_header_t* header = (psf1_header_t*)font_data;

        font->psf_version = 1;
        font->glyph_height = header->charsize;
        font->glyph_width = 8;
        font->num_glyphs = (header->mode & 0x01) ? 512 : 256;
        font->bytes_per_glyph = header->charsize;
        font->glyph_buffer = (uint8_t*)font_data + sizeof(psf1_header_t);

        size_t expected_size = sizeof(psf1_header_t) + (font->num_glyphs * font->bytes_per_glyph);
        if (font_size < expected_size) {
            return -1;
        }

        return 0;
    }
    else if (*(uint32_t*)font_data == PSF2_MAGIC) {
        psf2_header_t* header = (psf2_header_t*)font_data;

        if (font_size < sizeof(psf2_header_t)) {
            return -1;
        }

        font->psf_version = 2;
        font->glyph_width = header->width;
        font->glyph_height = header->height;
        font->num_glyphs = header->numglyph;
        font->bytes_per_glyph = header->bytesperglyph;
        font->glyph_buffer = (uint8_t*)font_data + header->headersize;

        size_t expected_size = header->headersize + (font->num_glyphs * font->bytes_per_glyph);
        if (font_size < expected_size) {
            return -1;
        }

        return 0;
    }

    return -1;
}

void psf_draw_char(psf_font_t* font, struct limine_framebuffer* framebuffer, uint32_t fb_width, uint32_t fb_height, uint32_t fb_pitch, char c, uint32_t x, uint32_t y, uint32_t fg_color, uint32_t bg_color) {
    if (!font || !framebuffer) {
        return;
    }

    if (!font->glyph_buffer || font->bytes_per_glyph == 0) {
        return;
    }

    uint8_t uc = (uint8_t)c;
    if (uc >= font->num_glyphs) {
        uc = 0;
    }

    uint8_t* glyph = font->glyph_buffer + (uc * font->bytes_per_glyph);

    if (font->psf_version == 1) {
        for (uint32_t row = 0; row < font->glyph_height; row++) {

            uint8_t glyph_byte = glyph[row];

            for (uint32_t col = 0; col < 8; col++) {

                uint8_t bit = 7 - col;

                if ((glyph_byte >> bit) & 1) {
                    put_pixel(framebuffer, x + col, y + row, fg_color);
                } else {
                    put_pixel(framebuffer, x + col, y + row, bg_color);
                }
            }
        }
    } else {
        uint32_t bytes_per_row = (font->glyph_width + 7) / 8;

        for (uint32_t row = 0; row < font->glyph_height; row++) {
            for (uint32_t col = 0; col < font->glyph_width; col++) {

                uint32_t byte_index = row * bytes_per_row + (col >> 3);
                uint8_t bit = 7 - (col & 7);

                if ((glyph[byte_index] >> bit) & 1) {
                    put_pixel(framebuffer, x + col, y + row, fg_color);
                } else {
                    put_pixel(framebuffer, x + col, y + row, bg_color);
                }
            }
        }
    }
}

void psf_draw_string(psf_font_t* font, struct limine_framebuffer* framebuffer, uint32_t fb_width, uint32_t fb_height, uint32_t fb_pitch, const char* str, uint32_t x, uint32_t y, uint32_t fg_color, uint32_t bg_color) {
    if (!font || !framebuffer || !str) {
        return;
    }

    uint32_t current_x = x;
    uint32_t current_y = y;

    while (*str) {
        if (*str == '\n') {
            current_x = x;
            current_y += font->glyph_height;
            str++;
            continue;
        }

        if (*str == '\r') {
            str++;
            continue;
        }

        if (*str == '\t') {
            current_x += font->glyph_width * 4;
            str++;
            continue;
        }

        if (current_y + font->glyph_height > fb_height) {
            break;
        }

        if (current_x + font->glyph_width > fb_width) {
            current_x = x;
            current_y += font->glyph_height;
            if (current_y + font->glyph_height > fb_height) {
                break;
            }
        }

        psf_draw_char(font, framebuffer, fb_width, fb_height, fb_pitch, *str, current_x, current_y, fg_color, bg_color);
        current_x += font->glyph_width;
        str++;
    }
}