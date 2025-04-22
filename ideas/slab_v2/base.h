#ifndef SLAB_BASE_H
#define SLAB_BASE_H

#include <stddef.h>

struct ptr {
        void *p_owner;
        void *p_ptr;
};

struct slab_base {
        struct ptr *(*sb_get)(struct slab_base *bp);
        void        (*sb_init)(struct slab_base *bp, size_t arr_size, size_t ring_size);
        void        (*sb_put)(struct slab_base *bp, struct ptr **p);
        void        (*sb_free)(struct slab_base *bp);
};

#endif
