#include "../include/memory.h"
#include <stdint.h>

int kstrlen(const char* s) {
    int len = 0;
    while (s[len] != '\0') len++;
    return len;
}

int strcmp(const char* a, const char* b) {
    while (*a && (*a == *b)) {
        a++;
        b++;
    }
    return *(const unsigned char*)a - *(const unsigned char*)b;
}

void* memcpy(void* restrict dst, const void* restrict src, size_t n) {
    char* d = (char*)dst;
    const char* s = (const char*)src;
    while (n--) {
        *d++ = *s++;
    }
    return dst;
}

void* memset(void* s, int c, size_t n) {
    unsigned char* p = s;
    while (n--) {
        *p++ = (unsigned char)c;
    }
    return s;
}

char* strdup(const char *s) {
    size_t len = kstrlen(s) + 1;
    char *new_s = malloc(len);
    if (new_s == NULL) return NULL;
    memcpy(new_s, s, len);
    return new_s;
}