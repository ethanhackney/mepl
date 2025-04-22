#ifndef LIB_H
#define LIB_H

#include <assert.h>
#include <errno.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DBUG
#ifdef DBUG
#define ASSERT(cond) assert(cond)
#else
#define ASSERT(cond)
#endif

#define ctor(p, type, field) \
        ((type *)((char *)(p) - offsetof(type, field)))

static inline void
die(const char *fmt, ...)
{
        va_list va;

        ASSERT(fmt);
        va_start(va, fmt);
        vfprintf(stderr, fmt, va);
        va_end(va);
        fprintf(stderr, ": %s\n", strerror(errno));
        exit(EXIT_FAILURE);
}

static inline void *
alloc(size_t size, const char *caller)
{
        void *p = malloc(size);

        ASSERT(size);
        ASSERT(caller);

        if (!p)
                die("%s: alloc()", caller);

        return p;
}
#define ALLOC(size) \
        alloc(size, __func__)

static inline bool
is_power_of_2(uint64_t n)
{
        return (n > (uint64_t)0) && !(n & (n - (uint64_t)1));
}

#define pp_free(pp) do {        \
        ASSERT(pp);             \
        ASSERT(*(pp));          \
        free(*(pp));            \
        *(pp) = NULL;           \
} while (0)

#endif
