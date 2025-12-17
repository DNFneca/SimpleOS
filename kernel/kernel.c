// kernel/main.c
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "limine.h"

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

	// Draw a simple pattern (white diagonal).
	for (size_t y = 0; y < 100; y++) {
		volatile uint32_t *pix = (uint32_t *)fb->address;
		pix[y * (fb->pitch / 4) + y] = 0xFFFFFF;
	}

	hcf(); // Halt
}
