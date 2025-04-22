#ifndef RING_H
#define RING_H

#include "lib.h"

#define ring_for_each(rp, i, count)                     \
        for (i = (rp)->r_rd, count = 0;                 \
             count < (rp)->r_len;                       \
             i = wrap_inc(i, (rp)->r_size), ++count)

#define ring_base_check(rp) do {                \
        ASSERT((rp));                           \
        ASSERT((rp)->r_buf);                    \
        ASSERT((rp)->r_rd < rp->r_size);        \
        ASSERT((rp)->r_wr < rp->r_size);        \
        ASSERT((rp)->r_len <= rp->r_size);      \
        ASSERT(is_power_of_2(rp->r_size));      \
} while (0)

#define ring_rd_inc(rp) do {                                    \
        (rp)->r_rd = wrap_inc((rp)->r_rd, (rp)->r_size);        \
} while (0)

#define ring_wr_inc(rp) do {                                    \
        (rp)->r_wr = wrap_inc((rp)->r_wr, (rp)->r_size);        \
} while (0)

static inline size_t
wrap_inc(size_t p, size_t size)
{
        return (p + 1) & (size - 1);
}

#define ring_rd_dec(rp) do {                                    \
        (rp)->r_rd = wrap_dec((rp)->r_rd, (rp)->r_size);        \
} while (0)

#define ring_wr_dec(rp) do {                                    \
        (rp)->r_wr = wrap_dec((rp)->r_wr, (rp)->r_size);        \
} while (0)

static inline size_t
wrap_dec(size_t p, size_t size)
{
        return (p - 1) & (size - 1);
}

#define ring_wr(rp) \
        ((rp)->r_buf[(rp)->r_wr])

#define ring_rd(rp) \
        ((rp)->r_buf[(rp)->r_rd])

#define RING_DEF(LINKAGE, T, NAME)              \
                                                \
struct NAME {                                   \
        size_t  r_wr;                           \
        size_t  r_rd;                           \
        size_t  r_len;                          \
        size_t  r_size;                         \
        T      *r_buf;                          \
};                                              \
                                                \
LINKAGE inline void                             \
NAME ## _init(struct NAME *rp, size_t size)     \
{                                               \
        ASSERT(rp);                             \
        ASSERT(is_power_of_2(size));            \
        rp->r_buf = ALLOC(sizeof(T) * size);    \
        rp->r_size = size;                      \
        rp->r_wr = 0;                           \
        rp->r_rd = 0;                           \
        rp->r_len = 0;                          \
}                                               \
                                                \
LINKAGE inline void                             \
NAME ## _free(struct NAME *rp)                  \
{                                               \
        ring_base_check(rp);                    \
        pp_free(&rp->r_buf);                    \
}                                               \
                                                \
LINKAGE inline bool                             \
NAME ## _full(const struct NAME *rp)            \
{                                               \
        ring_base_check(rp);                    \
        return rp->r_len == rp->r_size;         \
}                                               \
                                                \
LINKAGE inline bool                             \
NAME ## _empty(const struct NAME *rp)           \
{                                               \
        ring_base_check(rp);                    \
        return !rp->r_len;                      \
}                                               \
                                                \
LINKAGE inline void                             \
NAME ## _wr_back(struct NAME *rp, T v)          \
{                                               \
        ring_base_check(rp);                    \
        ring_wr(rp) = v;                        \
        ring_wr_inc(rp);                        \
        ++rp->r_len;                            \
}                                               \
                                                \
LINKAGE inline T                                \
NAME ## _rd_back(struct NAME *rp)               \
{                                               \
        T v;                                    \
                                                \
        ring_base_check(rp);                    \
        ring_wr_dec(rp);                        \
        v = ring_wr(rp);                        \
        --rp->r_len;                            \
        return v;                               \
}                                               \
                                                \
LINKAGE inline T                                \
NAME ## _rd_front(struct NAME *rp)              \
{                                               \
        T v;                                    \
                                                \
        ring_base_check(rp);                    \
        v = ring_rd(rp);                        \
        ring_rd_inc(rp);                        \
        --rp->r_len;                            \
        return v;                               \
}                                               \

#endif
