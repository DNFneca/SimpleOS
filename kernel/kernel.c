// kernel/main.c
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "../include/limine.h"
#include "../include/display.h"
#include "../include/psf.h"
#include "../include/memory.h"
#include "../include/string.h"

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
static volatile struct limine_memmap_request memmap_request = {
		.id = LIMINE_MEMMAP_REQUEST_ID,
		.revision = 0
};

__attribute__((used, section(".limine_requests")))
static volatile struct limine_hhdm_request hhdm_request = {
		.id = LIMINE_HHDM_REQUEST_ID,
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

static void hcf(void) {
	for (;;) asm("hlt");
}

static psf_font_t font;
static int font_loaded = 0;

void print_hex_uintptr_fixed(psf_font_t font, struct limine_framebuffer *fb, struct limine_framebuffer *fb_info, uintptr_t value, int y) {
	static const char hex[] = "0123456789ABCDEF";

    char str[20] = {'0', 'x'};


	for (int i = (sizeof(uintptr_t) * 8) - 4, j = 2; i >= 0; i -= 4, j++) {
		str[j] = hex[(value >> i) & 0XF];
	}


	psf_draw_string(&font, fb,
					fb_info->width, fb_info->height, fb_info->pitch,
					str,
					10, y,
					0x00FF00,    // green text
					0x000000);
}

extern uint8_t _kernel_end;

// Entry point called by the Limine loader.
void kmain(void) {
	if (!LIMINE_BASE_REVISION_SUPPORTED(limine_base_revision))
		hcf();

	if (framebuffer_request.response == NULL ||
		framebuffer_request.response->framebuffer_count < 1)
		hcf();

//	TODO: Fix kernel end and the heap_init should work :pray:

//	uintptr_t heap_start = ((uintptr_t)_kernel_end + 0xFFF) & ~0xFFF;
//	heap_init((void *)heap_start, 1024 * 1024);

	struct limine_framebuffer *fb =
			framebuffer_request.response->framebuffers[0];

	struct limine_framebuffer *fb_info = framebuffer_request.response->framebuffers[0];

	uint32_t *fb_ptr = (uint32_t *) fb->address;
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

				}
				break;
			}
		}
	}


	if (font_loaded) {
		psf_draw_char(&font, fb, fb_info->width, fb_info->height, fb_info->pitch, 'c', 10, 10, 0xFFFFFF, 0x0000);

		psf_draw_string(&font, fb,
						fb_info->width, fb_info->height, fb_info->pitch,
						"Hello, World!\nThis is a PSF font!\nLine 3 here.",
						10, 10,      // x, y position
						0xFFFFFF,    // white text
						0x000000);   // black background

		// Draw more text at different positions
		psf_draw_string(&font, fb,
						fb_info->width, fb_info->height, fb_info->pitch,
						"PSF fonts are cool!",
						10, 100,
						0x00FF00,    // green text
						0x000000);
	}

	struct limine_memmap_response *memmap = memmap_request.response;

	if (memmap == NULL) hcf();

	void* heap_start = NULL;
	size_t heap_size = 1024 * 1024; // 1MB

	for (uint64_t i = 0; i < memmap->entry_count; i++) {
		struct limine_memmap_entry *entry = memmap->entries[i];

		// Find usable memory after kernel
		if (entry->type == LIMINE_MEMMAP_USABLE && entry->length >= heap_size) {
			// Use physical address + HHDM offset
			heap_start = (void *)(entry->base + hhdm_request.response->offset);
			break;
		}
	}

	if (heap_start == NULL) hcf();

	heap_init(heap_start, heap_size);

	char *buffer = malloc(256);

	// TODO: Finish console.c and .h not fully done should use the limine_framebuffer to write to screen probably but memory management is done i think!

	// Use buffer...
	print_hex_uintptr_fixed(font, fb, fb_info, buffer, 320);

	// Free memory
	free(buffer);

	// Check stats (optional)
	heap_stats_t stats;
	heap_stats(&stats);

	psf_draw_string(&font, fb,
					fb_info->width, fb_info->height, fb_info->pitch,
					(int) stats.total_used_memory,
					50, 100,
					0x00FF00,    // green text
					0x000000);


	hcf(); // Halt
}
