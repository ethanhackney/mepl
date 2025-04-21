#ifndef PTRSET_H
#define PTRSET_H

#include "types.h"
#include <stdbool.h>

struct ptrset {
        uint64_t ps_bytes;
        void     (*ps_free)(struct ptrset **pspp);
        void     (*ps_add)(struct ptrset *psp, ptr_t *ptr);
        bool     (*ps_has)(const struct ptrset *psp, ptr_t *ptr);
};

#endif /* #ifndef PTRSET_H */
