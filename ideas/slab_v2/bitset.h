#ifndef BITSET_H
#define BITSET_H

#include "lib.h"

typedef uint64_t bitset_t;

#define BITSET_SHIFT       ((bitset_t)6)
#define BITSET_WORD_IDX(i) ((bitset_t)(i) >> BITSET_SHIFT)
#define BITSET_BIT_IDX(i)  ((bitset_t)(i) & (bitset_t)0x3F)
#define BITSET_SIZE(SIZE)  ((bitset_t)(SIZE) >> BITSET_SHIFT)
#define BITS_PER_WORD      ((bitset_t)64)
#define BITSET_COUNT(size) ((bitset_t)(size) << BITSET_SHIFT)
#define ONE                ((bitset_t)1)

#define bitset_size(bp) \
        BITSET_SIZE(bp->b_size)

#define bitset_count(bp) \
        BITSET_COUNT(bp->b_size)

#define bitset_for_each_word(bp, i) \
        for (i = 0; i < bitset_size(bp); ++i)

#define bitset_base_check(bp) do {              \
        ASSERT((bp));                           \
        ASSERT((bp)->b_set);                    \
        ASSERT(is_power_of_2((bp)->b_size));    \
        ASSERT(size_big_enough((bp)->b_size));  \
        ASSERT((bp)->b_last < bitset_size(bp)); \
} while (0)

#define bitset_idx_check(bp, word, bit) do {    \
        ASSERT(word < bitset_size(bp));         \
        ASSERT(bit < BITS_PER_WORD);            \
} while (0)

#define bitset_op(bp, op, n) do {               \
        bitset_t word = BITSET_WORD_IDX(n);     \
        bitset_t bit = BITSET_BIT_IDX(n);       \
                                                \
        bitset_base_check(bp);                  \
        bitset_idx_check(bp, word, bit);        \
                                                \
        bp->b_set[word] op (ONE << bit);        \
        if (word < bp->last)                    \
                bp->last = word;                \
} while (0)

static inline int
word_first_zero(bitset_t w)
{
        bitset_t inv = ~w;

        if (!inv)
                return -1;

        return __builtin_ctzll(inv);
}

static inline bool
size_big_enough(bitset_t size)
{
        return BITSET_SIZE(size) > (bitset_t)0;
}

#define BITSET_DEF(LINKAGE, NAME)                                       \
                                                                        \
struct NAME {                                                           \
        bitset_t *b_set;                                                \
        bitset_t  b_last;                                               \
        bitset_t  b_size;                                               \
};                                                                      \
                                                                        \
LINKAGE inline void                                                     \
NAME ## _init(struct NAME *bp, bitset_t size)                           \
{                                                                       \
        ASSERT(bp);                                                     \
        ASSERT(is_power_of_2(size));                                    \
        ASSERT(size_big_enough(size));                                  \
        bp->b_set = ALLOC(sizeof(bitset_t) * BITSET_SIZE(size));        \
        bp->b_last = 0;                                                 \
        bp->b_size = size;                                              \
}                                                                       \
                                                                        \
LINKAGE inline void                                                     \
NAME ## _free(struct NAME *bp)                                          \
{                                                                       \
        bitset_base_check(bp);                                          \
        pp_free(&bp->b_set);                                            \
}                                                                       \
                                                                        \
LINKAGE inline bitset_t                                                 \
NAME ## _first_zero(struct NAME *bp)                                    \
{                                                                       \
        bitset_t idx = 0;                                               \
        bitset_t i = 0;                                                 \
        int bit = 0;                                                    \
                                                                        \
        bitset_base_check(bp);                                          \
                                                                        \
        bitset_for_each_word(bp, i) {                                   \
                bit = word_first_zero(bp->b_set[i]);                    \
                if (bit < 0)                                            \
                        continue;                                       \
                                                                        \
                idx = (i << BITSET_SHIFT) + (bitset_t)bit;              \
                if (idx < bitset_count(bp)) {                           \
                        bp->b_last = i;                                 \
                        return idx;                                     \
                }                                                       \
        }                                                               \
                                                                        \
        ASSERT(0);                                                      \
        return (bitset_t)-1;                                            \
}                                                                       \
                                                                        \
LINKAGE inline void                                                     \
NAME ## _set(struct NAME *bp, bitset_t n)                               \
{                                                                       \
        bitset_op(bp, |=, n);                                           \
}                                                                       \
                                                                        \
LINKAGE inline void                                                     \
NAME ## _clear(struct NAME *bp, bitset_t n)                             \
{                                                                       \
        bitset_op(bp, &= ~, n);                                         \
}

#endif
