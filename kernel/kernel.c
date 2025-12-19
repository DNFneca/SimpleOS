// kernel/main.c
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "../include/limine.h"
#include "../include/display.h"

// Set the base revision to 4 (recommended latest for the Limine protocol).
__attribute__((used, section(".limine_requests")))
static volatile uint64_t limine_base_revision[] = LIMINE_BASE_REVISION(4);

// Request a framebuffer from the bootloader.
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

// Entry point called by the Limine loader.
void kmain(void) {
	if (!LIMINE_BASE_REVISION_SUPPORTED(limine_base_revision))
		hcf();

	if (framebuffer_request.response == NULL ||
		framebuffer_request.response->framebuffer_count < 1)
		hcf();

	struct limine_framebuffer *fb =
		framebuffer_request.response->framebuffers[0];

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

	draw_star(fb, center_x, center_y, 100, 40, 0x00FFFF00);

	hcf(); // Halt
}
