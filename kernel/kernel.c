// kernel/main.c
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "../include/limine.h"
#include "../include/display.h"
#include "../include/psf.h"

// Set the base revision to 4 (recommended latest for the Limine protocol).
__attribute__((used, section(".limine_requests")))
static volatile uint64_t limine_base_revision[] = LIMINE_BASE_REVISION(4);

// Request a framebuffer from the bootloader.
__attribute__((used, section(".limine_requests")))
static volatile struct limine_module_request module_request = {
        .id = LIMINE_MODULE_REQUEST_ID,
        .revision = 0
};

__attribute__((used, section(".limine_requests")))
static volatile struct limine_framebuffer_request framebuffer_request = {
        .id = LIMINE_FRAMEBUFFER_REQUEST_ID,
        .revision = 0
};

__attribute__((used, section(".limine_requests_start")))
static volatile uint64_t limine_requests_start_marker[] = LIMINE_REQUESTS_START_MARKER;

__attribute__((used, section(".limine_requests_end")))
static volatile uint64_t limine_requests_end_marker[] = LIMINE_REQUESTS_END_MARKER;

// Minimal implementations the compiler may emit.
void *memcpy(void *restrict dest, const void *restrict src, size_t n) {
	uint8_t *d = (uint8_t *)dest;
	const uint8_t *s = (const uint8_t *)src;
	for (size_t i = 0; i < n; i++)
		d[i] = s[i];
	return dest;
}

void *memset(void *s, int c, size_t n) {
	uint8_t *p = (uint8_t *)s;
	for (size_t i = 0; i < n; i++)
		p[i] = (uint8_t)c;
	return s;
}

static void hcf(void) {
	for (;;) asm("hlt");
}

static psf_font_t font;
static int font_loaded = 0;

void draw_test_rect(uint32_t *fb, uint32_t fb_width, uint32_t fb_pitch,
					uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t color) {
	for (uint32_t row = 0; row < h; row++) {
		for (uint32_t col = 0; col < w; col++) {
			uint32_t fb_index = (y + row) * (fb_pitch / 4) + (x + col);
			fb[fb_index] = color;
		}
	}
}

// Entry point called by the Limine loader.
void kmain(void) {
	if (!LIMINE_BASE_REVISION_SUPPORTED(limine_base_revision))
		hcf();

	if (framebuffer_request.response == NULL ||
		framebuffer_request.response->framebuffer_count < 1)
		hcf();

	struct limine_framebuffer *fb =
		framebuffer_request.response->framebuffers[0];

    struct limine_framebuffer *fb_info = framebuffer_request.response->framebuffers[0];

    uint32_t *fb_ptr = (uint32_t *)fb->address;
	for (uint64_t i = 0; i < (fb->height * fb->pitch) / 4; i++) {
		fb_ptr[i] = 0x00000000;
	}

	uint64_t width = fb->width;
	uint64_t height = fb->height;
	uint16_t bpp = fb->bpp;  // bits per pixel
	uint64_t pitch = fb->pitch;  // bytes per scanline
	void *fb_addr = fb->address;

	int center_x = fb->width / 2;
	int center_y = fb->height / 2;

	draw_line(fb, 0, 0,
				   100, 100, 0xFF0000); // Red square


	if (module_request.response != NULL && module_request.response->module_count > 0) {
		// Find font.psf in the loaded modules
		for (uint64_t i = 0; i < module_request.response->module_count; i++) {
			struct limine_file *file = module_request.response->modules[i];

			// Check if this is our font file
			// Limine provides the full path, so check the filename
			const char *filename = file->path;

			// Find the last '/' to get just the filename
			const char *last_slash = filename;
			for (const char *p = filename; *p; p++) {
				if (*p == '/') last_slash = p + 1;
			}

			// Compare with "font.psf"
			const char *expected = "font.psf";
			int match = 1;
			for (int j = 0; expected[j] || last_slash[j]; j++) {
				if (expected[j] != last_slash[j]) {
					match = 0;
					break;
				}
			}

			if (match) {
				// Load the PSF font
				if (psf_load(file->address, file->size, &font) == 0) {
					font_loaded = 1;

					draw_test_rect(fb, fb_info->width, fb_info->pitch,
								   200, 100, 50, 50, 0x00FF00); // Green square

				}
				break;
			}
		}
	}

//	draw_star(fb, center_x, center_y, 100, 40, 0x00FFFF00); (USE FOR DEBUG LOL)


	// Now you can draw text!
	if (font_loaded) {
		psf_draw_char(&font, fb, fb_info->width, fb_info->height, fb_info->pitch, 'c', 10, 10, 0xFFFFFF, 0x0000);

//		psf_draw_string(&font, fb,
//						fb_info->width, fb_info->height, fb_info->pitch,
//						"Hello, World!\nThis is a PSF font!\nLine 3 here.",
//						10, 10,      // x, y position
//						0xFFFFFF,    // white text
//						0x000000);   // black background
//
//		// Draw more text at different positions
//		psf_draw_string(&font, fb,
//						fb_info->width, fb_info->height, fb_info->pitch,
//						"PSF fonts are cool!",
//						10, 100,
//						0x00FF00,    // green text
//						0x000000);
	}

	hcf(); // Halt
}
