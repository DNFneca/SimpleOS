#include "../include/console.h"

#include <stdarg.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

// You need to implement this - outputs a single character to your display
extern void putchar(char c);

// Helper: reverse a string in place
static void reverse(char *str, int length) {
    int start = 0;
    int end = length - 1;
    while (start < end) {
        char temp = str[start];
        str[start] = str[end];
        str[end] = temp;
        start++;
        end--;
    }
}

// Convert integer to string
static int itoa(int64_t num, char *str, int base) {
    int i = 0;
    bool is_negative = false;

    if (num == 0) {
        str[i++] = '0';
        str[i] = '\0';
        return i;
    }

    if (num < 0 && base == 10) {
        is_negative = true;
        num = -num;
    }

    while (num != 0) {
        int64_t rem = num % base;
        str[i++] = (rem > 9) ? (rem - 10) + 'a' : rem + '0';
        num = num / base;
    }

    if (is_negative) {
        str[i++] = '-';
    }

    str[i] = '\0';
    reverse(str, i);

    return i;
}

// Convert unsigned integer to string
static int uitoa(uint64_t num, char *str, int base) {
    int i = 0;

    if (num == 0) {
        str[i++] = '0';
        str[i] = '\0';
        return i;
    }

    while (num != 0) {
        uint64_t rem = num % base;
        str[i++] = (rem > 9) ? (rem - 10) + 'a' : rem + '0';
        num = num / base;
    }

    str[i] = '\0';
    reverse(str, i);

    return i;
}

// Convert float to string (basic implementation)
static int ftoa(double num, char *str, int precision) {
    int i = 0;

    // Handle negative numbers
    if (num < 0) {
        str[i++] = '-';
        num = -num;
    }

    // Extract integer part
    int64_t int_part = (int64_t)num;
    double float_part = num - (double)int_part;

    // Convert integer part to string
    char int_str[32];
    int int_len = itoa(int_part, int_str, 10);

    for (int j = 0; j < int_len; j++) {
        str[i++] = int_str[j];
    }

    // Add decimal point
    str[i++] = '.';

    // Convert fractional part
    for (int j = 0; j < precision; j++) {
        float_part *= 10;
        int digit = (int)float_part;
        str[i++] = digit + '0';
        float_part -= digit;
    }

    str[i] = '\0';
    return i;
}

// Print string
static void print_string(const char *str) {
    if (!str) {
        str = "(null)";
    }
    while (*str) {
        putchar(*str++);
    }
}

// Print integer with padding
static void print_int(int64_t num, int width, char pad_char) {
    char buffer[32];
    int len = itoa(num, buffer, 10);

    // Print padding
    for (int i = len; i < width; i++) {
        putchar(pad_char);
    }

    print_string(buffer);
}

// Print unsigned integer
static void print_uint(uint64_t num, int base, int width, char pad_char, bool uppercase) {
    char buffer[32];
    int len = uitoa(num, buffer, base);

    // Convert to uppercase if needed
    if (uppercase) {
        for (int i = 0; i < len; i++) {
            if (buffer[i] >= 'a' && buffer[i] <= 'z') {
                buffer[i] = buffer[i] - 'a' + 'A';
            }
        }
    }

    // Print padding
    for (int i = len; i < width; i++) {
        putchar(pad_char);
    }

    print_string(buffer);
}

// Main printf implementation
int printf(const char *format, ...) {
    va_list args;
    va_start(args, format);

    int chars_written = 0;

    while (*format) {
        if (*format == '%') {
            format++;

            // Parse width
            int width = 0;
            char pad_char = ' ';

            if (*format == '0') {
                pad_char = '0';
                format++;
            }

            while (*format >= '0' && *format <= '9') {
                width = width * 10 + (*format - '0');
                format++;
            }

            // Parse precision for floats
            int precision = 6; // Default precision
            if (*format == '.') {
                format++;
                precision = 0;
                while (*format >= '0' && *format <= '9') {
                    precision = precision * 10 + (*format - '0');
                    format++;
                }
            }

            // Parse length modifiers
            bool is_long = false;
            bool is_long_long = false;

            if (*format == 'l') {
                format++;
                is_long = true;
                if (*format == 'l') {
                    format++;
                    is_long_long = true;
                }
            } else if (*format == 'z') {
                format++;
                is_long = sizeof(size_t) == 8;
            }

            // Handle format specifier
            switch (*format) {
                case 'd':
                case 'i': {
                    int64_t num;
                    if (is_long_long) {
                        num = va_arg(args, long long);
                    } else if (is_long) {
                        num = va_arg(args, long);
                    } else {
                        num = va_arg(args, int);
                    }
                    print_int(num, width, pad_char);
                    break;
                }

                case 'u': {
                    uint64_t num;
                    if (is_long_long) {
                        num = va_arg(args, unsigned long long);
                    } else if (is_long) {
                        num = va_arg(args, unsigned long);
                    } else {
                        num = va_arg(args, unsigned int);
                    }
                    print_uint(num, 10, width, pad_char, false);
                    break;
                }

                case 'x': {
                    uint64_t num;
                    if (is_long_long) {
                        num = va_arg(args, unsigned long long);
                    } else if (is_long) {
                        num = va_arg(args, unsigned long);
                    } else {
                        num = va_arg(args, unsigned int);
                    }
                    print_uint(num, 16, width, pad_char, false);
                    break;
                }

                case 'X': {
                    uint64_t num;
                    if (is_long_long) {
                        num = va_arg(args, unsigned long long);
                    } else if (is_long) {
                        num = va_arg(args, unsigned long);
                    } else {
                        num = va_arg(args, unsigned int);
                    }
                    print_uint(num, 16, width, pad_char, true);
                    break;
                }

                case 'o': {
                    uint64_t num = va_arg(args, unsigned int);
                    print_uint(num, 8, width, pad_char, false);
                    break;
                }

                case 'p': {
                    void *ptr = va_arg(args, void*);
                    print_string("0x");
                    print_uint((uint64_t)ptr, 16, sizeof(void*) * 2, '0', false);
                    break;
                }

                case 's': {
                    const char *str = va_arg(args, const char*);
                    print_string(str);
                    break;
                }

                case 'c': {
                    char c = (char)va_arg(args, int);
                    putchar(c);
                    break;
                }

                case 'f': {
                    double num = va_arg(args, double);
                    char buffer[64];
                    ftoa(num, buffer, precision);
                    print_string(buffer);
                    break;
                }

                case '%': {
                    putchar('%');
                    break;
                }

                default: {
                    putchar('%');
                    putchar(*format);
                    break;
                }
            }

            format++;
        } else {
            putchar(*format++);
            chars_written++;
        }
    }

    va_end(args);
    return chars_written;
}

// Bonus: sprintf implementation
int sprintf(char *str, const char *format, ...) {
    va_list args;
    va_start(args, format);

    int pos = 0;

    while (*format) {
        if (*format == '%') {
            format++;

            switch (*format) {
                case 'd':
                case 'i': {
                    int num = va_arg(args, int);
                    char buffer[32];
                    int len = itoa(num, buffer, 10);
                    for (int i = 0; i < len; i++) {
                        str[pos++] = buffer[i];
                    }
                    break;
                }

                case 's': {
                    const char *s = va_arg(args, const char*);
                    while (*s) {
                        str[pos++] = *s++;
                    }
                    break;
                }

                case 'c': {
                    char c = (char)va_arg(args, int);
                    str[pos++] = c;
                    break;
                }

                case 'x': {
                    unsigned int num = va_arg(args, unsigned int);
                    char buffer[32];
                    int len = uitoa(num, buffer, 16);
                    for (int i = 0; i < len; i++) {
                        str[pos++] = buffer[i];
                    }
                    break;
                }

                default:
                    str[pos++] = *format;
                    break;
            }
            format++;
        } else {
            str[pos++] = *format++;
        }
    }

    str[pos] = '\0';
    va_end(args);
    return pos;
}