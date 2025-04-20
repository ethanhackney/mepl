#ifndef SLAB_H
#define SLAB_H

#include "lib.h"

#define SLAB_TYPE_DEF(T, NAME, SIZE)                    \
                                                        \
struct NAME {                                           \
        T pool[SIZE];                                   \
        T *free[SIZE];                                  \
        T **fp;                                         \
};                                                      \
                                                        \
static void                                             \
do_ ## NAME ## _init(struct NAME *sp)                   \
{                                                       \
        int i = 0;                                      \
                                                        \
        for (i = 0; i < (SIZE); i++)                    \
                sp->free[i] = &sp->pool[i];             \
                                                        \
        sp->fp = sp->free + (SIZE);                     \
}                                                       \
                                                        \
static T *                                              \
do_ ## NAME ## _get(struct NAME *sp)                    \
{                                                       \
        return *--sp->fp;                               \
}                                                       \
                                                        \
static bool                                             \
do_ ## NAME ## _mine(const struct NAME *sp, const T *p) \
{                                                       \
        return sp->pool <= p && p < sp->pool + (SIZE);  \
}                                                       \
                                                        \
static void                                             \
do_ ## NAME ## _put(struct NAME *sp, T **pp)            \
{                                                       \
        *sp->fp++ = *pp;                                \
        *pp = NULL;                                     \
}                                                       \
                                                        \
static bool                                             \
do_ ## NAME ## _full(const struct NAME *sp)             \
{                                                       \
        return sp->fp == sp->free + (SIZE);             \
}                                                       \
                                                        \
static bool                                             \
do_ ## NAME ## _empty(const struct NAME *sp)            \
{                                                       \
        return sp->fp == sp->free;                      \
}

struct slab {
        bool (*s_full)(const struct slab *sp);
        bool (*s_mine)(const struct slab *sp, const void *p);
        bool (*s_empty)(const struct slab *sp);
        void *(*s_get)(struct slab *sp);
        void (*s_put)(struct slab *sp, void **pp);
        void (*s_init)(struct slab *sp);
};

#define SLAB_DEF(T, NAME, FIELD, SLAB_FIELD)                            \
                                                                        \
static void                                                             \
NAME ## _init(struct slab *sp)                                          \
{                                                                       \
        struct NAME *dp = STRUCT_OF(sp, struct NAME, FIELD);            \
                                                                        \
        do__ ## NAME ## _init(&dp->SLAB_FIELD);                         \
}                                                                       \
                                                                        \
static void *                                                           \
NAME ## _get(struct slab *sp)                                           \
{                                                                       \
        struct NAME *dp = STRUCT_OF(sp, struct NAME, FIELD);            \
                                                                        \
        return (void *)do__ ## NAME ## _get(&dp->SLAB_FIELD);           \
}                                                                       \
                                                                        \
static bool                                                             \
NAME ## _mine(const struct slab *sp, const void *p)                     \
{                                                                       \
        const struct NAME *dp = STRUCT_OF(sp, struct NAME, FIELD);      \
                                                                        \
        return do__ ## NAME ## _mine(&dp->SLAB_FIELD, (T *)p);          \
}                                                                       \
                                                                        \
static void                                                             \
NAME ## _put(struct slab *sp, void **pp)                                \
{                                                                       \
        struct NAME *dp = STRUCT_OF(sp, struct NAME, FIELD);            \
                                                                        \
        do__ ## NAME ## _put(&dp->SLAB_FIELD, (T **)pp);                \
}                                                                       \
                                                                        \
static bool                                                             \
NAME ## _full(const struct slab *sp)                                    \
{                                                                       \
        const struct NAME *dp = STRUCT_OF(sp, struct NAME, FIELD);      \
                                                                        \
        return do__ ## NAME ## _full(&dp->SLAB_FIELD);                  \
}                                                                       \
                                                                        \
static bool                                                             \
NAME ## _empty(const struct slab *sp)                                   \
{                                                                       \
        const struct NAME *dp = STRUCT_OF(sp, struct NAME, FIELD);      \
                                                                        \
        return do__ ## NAME ## _empty(&dp->SLAB_FIELD);                 \
}

#define SLAB_INIT(NAME)                 \
{                                       \
        .s_full  = NAME ## _full,       \
        .s_init  = NAME ## _init,       \
        .s_mine  = NAME ## _mine,       \
        .s_get   = NAME ## _get,        \
        .s_put   = NAME ## _put,        \
        .s_empty = NAME ## _empty,      \
}

#endif /* #ifndef SLAB_H */
