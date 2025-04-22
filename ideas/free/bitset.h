#ifndef BITSET_H
#define BITSET_H

#include <assert.h>
#include <stdint.h>
#include <string.h>

typedef uint64_t bitset_t;

#define BITSET_SHIFT       ((bitset_t)6)
#define BITSET_WORD_IDX(i) ((bitset_t)(i) >> BITSET_SHIFT)
#define BITSET_BIT_IDX(i)  ((bitset_t)(i) & (bitset_t)0x3F)
#define BITSET_SIZE(SIZE)  ((bitset_t)(SIZE) >> BITSET_SHIFT)
#define BITS_PER_WORD      ((bitset_t)64)

static inline int
word_first_zero(bitset_t w)
{
        bitset_t inv = ~w;

        if (!inv)
                return -1;

        return __builtin_ctzll(inv);
}

#define BITSET_DEF(LINKAGE, NAME, SIZE)                         \
                                                                \
static_assert((SIZE > 0) && (((SIZE) & ((SIZE) - 1)) == 0));    \
static_assert(BITSET_SIZE(SIZE) > 0);                           \
                                                                \
struct NAME {                                                   \
        bitset_t set[BITSET_SIZE(SIZE)];                        \
};                                                              \
                                                                \
LINKAGE inline void                                             \
NAME ## _init(struct NAME *bp)                                  \
{                                                               \
        memset(bp, 0, sizeof(*bp));                             \
}                                                               \
                                                                \
LINKAGE inline void                                             \
NAME ## _free(struct NAME *bp)                                  \
{                                                               \
        NAME ## _init(bp);                                      \
}                                                               \
                                                                \
LINKAGE inline bitset_t                                         \
NAME ## _first_zero(const struct NAME *bp)                      \
{                                                               \
        bitset_t idx = 0;                                       \
        bitset_t i = 0;                                         \
        int bit = 0;                                            \
                                                                \
        for (i = 0; i < BITSET_SIZE(SIZE); i++) {               \
                bit = word_first_zero(bp->set[i]);              \
                if (bit < 0)                                    \
                        continue;                               \
                                                                \
                idx = (i << BITSET_SHIFT) + (bitset_t)bit;      \
                if (idx < (SIZE))                               \
                        return idx;                             \
        }                                                       \
                                                                \
        return (bitset_t)-1;                                    \
}                                                               \
                                                                \
LINKAGE inline void                                             \
NAME ## _set(struct NAME *bp, bitset_t n)                       \
{                                                               \
        bitset_t word = BITSET_WORD_IDX(n);                     \
        bitset_t bit = BITSET_BIT_IDX(n);                       \
                                                                \
        bp->set[word] |= ((bitset_t)1 << bit);                  \
}                                                               \
                                                                \
LINKAGE inline void                                             \
NAME ## _clear(struct NAME *bp, bitset_t n)                     \
{                                                               \
        bitset_t word = BITSET_WORD_IDX(n);                     \
        bitset_t bit = BITSET_BIT_IDX(n);                       \
                                                                \
        bp->set[word] &= ~((bitset_t)1 << bit);                 \
}

#endif
