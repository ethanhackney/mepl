#ifndef FIXSET_H
#define FIXSET_H

#include "ptrset.h"

#ifndef FIXSET_SIZE
#define FIXSET_SIZE (1ULL << (sizeof(ptr_t) * 8ULL))
#endif /* #ifndef FIXSET_SIZE */

struct fixset;

struct ptrset *fixset_new(void);

#endif /* #ifndef FIXSET_H */
