#ifndef RING_H
#define RING_H

#include <stdlib.h>
#include <stdbool.h>

#define RING_INC(p, size) do {                  \
        *(p) = (*(p) + 1) & ((size) - 1);       \
} while (0)

#define RING_DEF(LINKAGE, T, NAME, SIZE)                \
                                                        \
struct NAME {                                           \
        size_t r_wr;                                    \
        size_t r_rd;                                    \
        size_t r_len;                                   \
        T      r_buf[SIZE];                             \
};                                                      \
                                                        \
LINKAGE inline void                                     \
NAME ## _init(struct NAME *rp)                          \
{                                                       \
        rp->r_wr = 0;                                   \
        rp->r_rd = 0;                                   \
        rp->r_len = 0;                                  \
}                                                       \
                                                        \
LINKAGE inline bool                                     \
NAME ## _full(const struct NAME *rp)                    \
{                                                       \
        return rp->r_len == (SIZE);                     \
}                                                       \
                                                        \
LINKAGE inline bool                                     \
NAME ## _empty(const struct NAME *rp)                   \
{                                                       \
        return rp->r_len == 0;                          \
}                                                       \
                                                        \
LINKAGE inline void                                     \
NAME ## _put(struct NAME *rp, T v)                      \
{                                                       \
        rp->r_buf[rp->r_wr] = v;                        \
        RING_INC(&rp->r_wr, SIZE);                      \
}                                                       \
                                                        \
LINKAGE inline T                                        \
NAME ## _get(struct NAME *rp)                           \
{                                                       \
        T tmp = rp->r_buf[rp->r_rd];                    \
        RING_INC(&rp->r_rd, SIZE);                      \
        --rp->r_len;                                    \
        return tmp;                                     \
}

#endif
