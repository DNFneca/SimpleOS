#pragma once

#include <stddef.h>

int strcmp(const char* a, const char* b);
void* memcpy(void* restrict dst, const void* restrict src, size_t n);
void* memset(void* s, int c, size_t n);
char* strdup(const char *s);