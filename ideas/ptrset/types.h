#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>

typedef uint16_t ptr_t;

#define BITS(type) (sizeof(type) * 8)
#define ADDR_LEN   (BITS(void *) / BITS(ptr_t))

#define PTR_FOR_EACH(ptr, p) \
        for (p = ptr; p < (ptr) + ADDR_LEN; p++)

#endif
