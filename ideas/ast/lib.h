#ifndef LIB_H
#define LIB_H

#include <errno.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ARRAY_SIZE(arr) \
        (sizeof(arr) / sizeof(*arr))

#define STRUCT_OF(p, type, field) \
        ((type *)((char *)(p) - offsetof(type, field)))

static inline void
die(const char *fmt, ...)
{
        va_list va;

        va_start(va, fmt);
        vfprintf(stderr, fmt, va);
        va_end(va);
        fprintf(stderr, ": %s\n", strerror(errno));
        exit(EXIT_FAILURE);
}

static inline void
die_c(int c, const char *fmt, ...)
{
        va_list va;

        va_start(va, fmt);
        vfprintf(stderr, fmt, va);
        va_end(va);
        fprintf(stderr, ": %s\n", strerror(c));
        exit(EXIT_FAILURE);
}

#endif
