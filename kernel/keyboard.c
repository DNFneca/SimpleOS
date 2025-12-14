#include <stdint.h>
#include <stdbool.h>
#include "../include/keyboard.h"
#include "../include/io.h"
#include "../include/console.h"

#define LEFT_SHIFT 0X2A
#define RIGHT_SHIFT 0x36
#define CAPSLOCK 0x3A
#define EXTENDED 0xE0
#define KEYBOARD_INPUT_BUFFER 0x60
#define HAS_KEYBOARD_INPUT_BUFFER 0x64
#define KEY_RELEASED 0x80

// Normal keys
char scancode_to_char[128] = {
    0,  27, '1','2','3','4','5','6','7','8','9','0','-','=', '\b',
    '\t','q','w','e','r','t','y','u','i','o','p','[',']','\n',0,
    'a','s','d','f','g','h','j','k','l',';','\'','`',0,'\\','z',
    'x','c','v','b','n','m',',','.','/',0, '*',0, ' ',0,0,0
};

// Shifted keys
char scancode_to_char_shift[128] = {
    0, 27, '!','@','#','$','%','^','&','*','(',')','_','+', '\b',
    '\t','Q','W','E','R','T','Y','U','I','O','P','{','}','\n',0,
    'A','S','D','F','G','H','J','K','L',':','"','~',0,'|','Z',
    'X','C','V','B','N','M','<','>','?',0,'*',0,' ',0,0,0
};

int keyboard_has_char() {
    return inb(HAS_KEYBOARD_INPUT_BUFFER) & 1;
}

static bool capslock = false;
static bool shift = false;
static bool extended_code = false;

static bool is_shift_pressed(uint8_t keyboard_char) {
	return keyboard_char == LEFT_SHIFT || keyboard_char == RIGHT_SHIFT;
}

static bool is_shift_released(uint8_t keyboard_char) {
    return (keyboard_char & KEY_RELEASED && is_shift_pressed(keyboard_char));
}

static bool is_caps_pressed(uint8_t keyboard_char) {
    return keyboard_char == CAPSLOCK;
}

static bool is_extended() {
    uint8_t keyboard_char = inb(KEYBOARD_INPUT_BUFFER);
    if (keyboard_char == EXTENDED) {
        return true;
    }
    return false;
}

char keyboard_get_char() {
    // Polling loop must only read when data is available
    while (!keyboard_has_char()) {
        // Simple busy wait
    }

    uint8_t scancode = inb(KEYBOARD_INPUT_BUFFER);

    // --- State updates (Handle Shift/Caps lock) ---
    if (scancode == LEFT_SHIFT || scancode == RIGHT_SHIFT) {
        shift = true;
        return 0; // Return 0 to indicate state change, not a character
    }
    if (scancode == (LEFT_SHIFT | KEY_RELEASED) || scancode == (RIGHT_SHIFT | KEY_RELEASED)) {
        shift = false;
        return 0;
    }
    if (scancode == CAPSLOCK) {
        capslock = !capslock;
        return 0;
    }

    // --- Extended Code Handling ---
    if (scancode == EXTENDED) {
        extended_code = true;
        return 0; // Wait for the next scancode
    }

    if (extended_code) {
        extended_code = false; // Reset for the next sequence

        // Only interested in the key press scancodes for arrows (0x48, 0x50, etc.)
        if (scancode & KEY_RELEASED) return 0;

        switch (scancode) {
            case 0x48: return (char)0xF1; // Up
            case 0x50: return (char)0xF2; // Down
            case 0x4B: return (char)0xF3; // Left
            case 0x4D: return (char)0xF4; // Right
            default:
                return 0;
        }
    }

    // --- Released Key check (must be AFTER state updates) ---
    if (scancode & KEY_RELEASED) return 0;

    // --- Special hardcoded keys ---
    if (scancode == 0x0E) return 0x08; // Backspace
    if (scancode == 0x1C) return '\n'; // Enter

    // --- Normal Character mapping ---
    char c;
    if (shift)
        c = scancode_to_char_shift[scancode];
    else
        c = scancode_to_char[scancode];

    // Handle CapsLock for letters
    if (c >= 'a' && c <= 'z') {
        if (capslock && !shift) c -= 32;      // lowercase → uppercase
        else if (capslock && shift) c += 32;  // uppercase → lowercase (shift + caps)
    }

    return c;
}
