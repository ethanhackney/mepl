#ifndef LIB_H
#define LIB_H

#include <stddef.h>

#define ARRAY_SIZE(arr) \
        (sizeof(arr) / sizeof(*arr))

#define struct_of(p, type, field) \
        ((type *)((char *)(p) - offsetof(type, field)))


#endif
