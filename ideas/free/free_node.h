#ifndef FREE_NODE_H
#define FREE_NODE_H

#include "bitset.h"
#include "lib.h"
#include "list.h"
#include <stdlib.h>

#define FREE_NODE_DEF(LINKAGE, T, NAME, POOL_SIZE)              \
                                                                \
BITSET_DEF(LINKAGE, NAME ## _set, POOL_SIZE)                    \
                                                                \
struct NAME {                                                   \
        struct NAME ## _set fn_free;                            \
        struct list         fn_list;                            \
        size_t              fn_count;                           \
        T                   fn_pool[POOL_SIZE];                 \
};                                                              \
                                                                \
struct NAME ## _ptr {                                           \
        struct NAME *p_owner;                                   \
        T           *p_ptr;                                     \
};                                                              \
                                                                \
LINKAGE inline void                                             \
NAME ## _init(struct NAME *fnp)                                 \
{                                                               \
        fnp->fn_count = POOL_SIZE;                              \
        NAME ## _set_init(&fnp->fn_free);                       \
        list_init(&fnp->fn_list);                               \
}                                                               \
                                                                \
LINKAGE inline struct NAME *                                    \
NAME ## _new(void)                                              \
{                                                               \
        struct NAME *fnp = malloc(sizeof(*fnp));                \
                                                                \
        if (UNLIKELY(!fnp))                                     \
                return NULL;                                    \
                                                                \
        NAME ## _init(fnp);                                     \
        return fnp;                                             \
}                                                               \
                                                                \
LINKAGE inline void                                             \
NAME ## _free(struct NAME **fnpp)                               \
{                                                               \
        free(*fnpp);                                            \
        *fnpp = NULL;                                           \
}                                                               \
                                                                \
LINKAGE inline void                                             \
NAME ## _put(struct NAME *fnp, void *ptr)                       \
{                                                               \
        bitset_t idx = ((T *)ptr) - fnp->fn_pool;               \
                                                                \
        NAME ## _set_clear(&fnp->fn_free, idx);                 \
}                                                               \
                                                                \
LINKAGE inline struct NAME ## _ptr *                            \
NAME ## _get(struct NAME *fnp)                                  \
{                                                               \
        bitset_t idx = NAME ## _set_first_zero(&fnp->fn_free);  \
        T *ptr = &fnp->fn_pool[idx];                            \
        struct NAME ## _ptr *p = malloc(sizeof(*p));            \
                                                                \
        if (UNLIKELY(!p))                                       \
                return NULL;                                    \
                                                                \
        NAME ## _set_set(&fnp->fn_free, idx);                   \
        p->p_owner = fnp;                                       \
        p->p_ptr = ptr;                                         \
        --fnp->fn_count;                                        \
        return p;                                               \
}                                                               \
                                                                \
                                                                \
LINKAGE inline void                                             \
NAME ## _ptr_free(struct NAME ## _ptr **pp)                     \
{                                                               \
        struct NAME ## _ptr *p = *pp;                           \
        struct NAME *owner = p->p_owner;                        \
                                                                \
        NAME ## _put(owner, p->p_ptr);                          \
        free(p);                                                \
        *pp = NULL;                                             \
}                                                               \
                                                                \
LINKAGE inline T *                                              \
NAME ## _ptr_val(struct NAME ## _ptr *p)                        \
{                                                               \
        return p->p_ptr;                                        \
}

#endif
