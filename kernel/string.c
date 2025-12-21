#include "../include/string.h"
#include "../include/memory.h"

size_t strlen_simple(const char *s) {
    size_t len = 0;
    while (s[len])
        len++;
    return len;
}


char *concat(const char *a, const char *b) {
    size_t len_a = strlen_simple(a);
    size_t len_b = strlen_simple(b);

    char *out = malloc(len_a + len_b + 1);
    if (!out)
        return NULL;

    char *p = out;

    while (*a)
        *p++ = *a++;

    while (*b)
        *p++ = *b++;

    *p = '\0';
    return out;
}
