#ifndef LIB_H
#define LIB_H

#include <stddef.h>

#define STRUCT_OF(p, type, field) \
        ((type *)((char *)(p) - offsetof(type, field)))

#define UNLIKELY(x) __builtin_expect(!!(x), 0)

#endif
