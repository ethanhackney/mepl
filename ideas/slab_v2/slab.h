#ifndef SLAB_H
#define SLAB_H

#include "base.h"
#include "bitset.h"
#include "lib.h"
#include "list.h"
#include "ring.h"

#define slab_base_check(sp) do {                        \
        ASSERT((sp)->s_nodes.next);                     \
        ASSERT((sp)->s_nodes.prev);                     \
        ASSERT((sp)->s_arr_size);                       \
        ASSERT(is_power_of_2((sp)->s_arr_size));        \
} while (0)

#define slab_node_base_check(np) do {           \
        ASSERT((np)->n_nodes.next);             \
        ASSERT((np)->n_nodes.prev);             \
        ASSERT((np)->n_arr);                    \
} while (0)

#define slab_for_each_entry(head, p, ep, name) \
        list_for_each_entry(head, p, ep, struct name ## _node, n_nodes)

#define node(p, name) \
        ctor(p, struct name ## _node, n_nodes)

#define SLAB_DEF(LINKAGE, T, NAME)                                      \
                                                                        \
BITSET_DEF(LINKAGE, NAME ## _set)                                       \
struct NAME ## _node {                                                  \
        struct NAME ## _set n_set;                                      \
        struct list         n_nodes;                                    \
        size_t              n_count;                                    \
        T                   *n_arr;                                     \
};                                                                      \
                                                                        \
RING_DEF(LINKAGE, struct NAME ## _node *, NAME ## _ring)                \
struct NAME {                                                           \
        struct NAME ## _ring s_ring;                                    \
        struct list          s_nodes;                                   \
        struct list          s_empty;                                   \
        size_t               s_arr_size;                                \
};                                                                      \
                                                                        \
LINKAGE inline void                                                     \
NAME ## _node_put(struct NAME ## _node *np, T *ptr)                     \
{                                                                       \
        bitset_t idx;                                                   \
                                                                        \
        slab_node_base_check(np);                                       \
        idx = ptr - np->n_arr;                                          \
        NAME ## _set_clear(&np->n_set, idx);                            \
}                                                                       \
                                                                        \
                                                                        \
LINKAGE inline T *                                                      \
NAME ## _node_get(struct NAME ## _node *np)                             \
{                                                                       \
        bitset_t idx = 0;                                               \
        slab_node_base_check(np);                                       \
        idx = NAME ## _set_first_zero(&np->n_set);                      \
        NAME ## _set_set(&np->n_set, idx);                              \
        --np->n_count;                                                  \
        return &np->n_arr[idx];                                         \
}                                                                       \
                                                                        \
LINKAGE inline struct NAME ## _node *                                   \
NAME ## _node_new(struct NAME *sp)                                      \
{                                                                       \
        struct NAME ## _node *np = NULL;                                \
                                                                        \
        slab_base_check(sp);                                            \
                                                                        \
        if (!NAME ## _ring_empty(&sp->s_ring))                          \
                np = NAME ## _ring_rd(&sp->s_ring);                     \
        else                                                            \
                np = ALLOC(sizeof(*np));                                \
                                                                        \
        np->n_count = sp->s_arr_size;                                   \
        list_init(&np->n_nodes);                                        \
        NAME ## _set_init(&np->n_set, np->n_count);                     \
        np->n_arr = ALLOC(sizeof(T) * np->n_count);                     \
        return np;                                                      \
}                                                                       \
                                                                        \
LINKAGE inline void                                                     \
do_ ## NAME ## _node_free(struct NAME ## _node **npp)                   \
{                                                                       \
        slab_node_base_check(*npp);                                     \
        list_rm(&(*npp)->n_nodes);                                      \
        NAME ## _set_free(&(*npp)->n_set);                              \
        pp_free(&(*npp)->n_arr);                                        \
        pp_free(npp);                                                   \
        *npp = NULL;                                                    \
}                                                                       \
                                                                        \
LINKAGE inline void                                                     \
NAME ## _node_free(struct NAME *sp, struct NAME ## _node **npp)         \
{                                                                       \
        struct NAME ## _node *np = NULL;                                \
                                                                        \
        slab_base_check(sp);                                            \
        slab_node_base_check(*npp);                                     \
        list_rm(&(*npp)->n_nodes);                                      \
                                                                        \
        if (NAME ## _ring_full(&sp->s_ring)) {                          \
                np = NAME ## _ring_rd(&sp->s_ring);                     \
                do_ ## NAME ## _node_free(&np);                         \
        }                                                               \
                                                                        \
        NAME ## _ring_wr(&sp->s_ring, *npp);                            \
        *npp = NULL;                                                    \
}                                                                       \
                                                                        \
LINKAGE inline void                                                     \
NAME ## _init(struct NAME *sp, size_t arr_size, size_t ring_size)       \
{                                                                       \
        ASSERT(sp);                                                     \
        ASSERT(is_power_of_2(arr_size));                                \
        ASSERT(is_power_of_2(ring_size));                               \
                                                                        \
        NAME ## _ring_init(&sp->s_ring, ring_size);                     \
        list_init(&sp->s_nodes);                                        \
        list_init(&sp->s_empty);                                        \
        sp->s_arr_size = arr_size;                                      \
}                                                                       \
                                                                        \
LINKAGE inline void                                                     \
NAME ## _free(struct NAME *sp)                                          \
{                                                                       \
        struct NAME ## _node *np = NULL;                                \
        size_t n = 0;                                                   \
        size_t i = 0;                                                   \
                                                                        \
        slab_base_check(sp);                                            \
                                                                        \
        ring_for_each(&sp->s_ring, i, n)                                \
                do_ ## NAME ## _node_free(&sp->s_ring.r_buf[i]);        \
                                                                        \
        while (!list_empty(&sp->s_nodes)) {                             \
                np = node(sp->s_nodes.next, NAME);                      \
                do_ ## NAME ## _node_free(&np);                         \
        }                                                               \
                                                                        \
        while (!list_empty(&sp->s_empty)) {                             \
                np = node(sp->s_empty.next, NAME);                      \
                do_ ## NAME ## _node_free(&np);                         \
        }                                                               \
                                                                        \
        NAME ## _ring_free(&sp->s_ring);                                \
}                                                                       \
                                                                        \
LINKAGE inline struct ptr *                                             \
NAME ## _get(struct NAME *sp)                                           \
{                                                                       \
        struct NAME ## _node *np = NULL;                                \
        struct list *p = NULL;                                          \
        struct ptr *ptr = ALLOC(sizeof(*ptr));                          \
        T *vp = NULL;                                                   \
                                                                        \
        slab_base_check(sp);                                            \
                                                                        \
        slab_for_each_entry(&sp->s_nodes, p, np, NAME) {                \
                if (np->n_count)                                        \
                        break;                                          \
        }                                                               \
                                                                        \
        if (p == &sp->s_nodes)                                          \
                np = NAME ## _node_new(sp);                             \
                                                                        \
        vp = NAME ## _node_get(np);                                     \
        if (!np->n_count) {                                             \
                list_rm(&np->n_nodes);                                  \
                list_add_after(&sp->s_empty, &np->n_nodes);             \
        }                                                               \
        ptr->p_owner = np;                                              \
        ptr->p_ptr = vp;                                                \
        return ptr;                                                     \
}                                                                       \
                                                                        \
LINKAGE inline void                                                     \
NAME ## _put(struct NAME *sp, struct ptr **pp)                          \
{                                                                       \
        struct NAME ## _node *np = NULL;                                \
                                                                        \
        ASSERT(pp);                                                     \
        ASSERT(*pp);                                                    \
        slab_base_check(sp);                                            \
                                                                        \
        np = (*pp)->p_owner;                                            \
        NAME ## _node_put(np, (*pp)->p_ptr);                            \
        pp_free(pp);                                                    \
                                                                        \
        if (np->n_count == sp->s_arr_size) {                            \
                NAME ## _node_free(sp, &np);                            \
        } else {                                                        \
                list_rm(&np->n_nodes);                                  \
                list_add_after(&sp->s_nodes, &np->n_nodes);             \
        }                                                               \
}

#endif
