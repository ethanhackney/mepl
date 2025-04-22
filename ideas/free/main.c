#include "free_node.h"
#include "ring.h"
#include "lib.h"
#include "list.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define list_empty(head) \
        ((head)->next == (head))

/* TODO: future ideas
struct ptr {
        void *p_owner;
        void *p_ptr;
};

struct pool {
        struct ptr *(*p_get)(struct pool *p);
        void       (*p_put)(struct pool *p, struct ptr *ptr);
};

static void my_pool_put(struct pool *p, struct ptr *ptr)
{
        struct my_pool *mp = STRUCT_OF(p, struct my_pool, mp_p);
        struct db_query_node *np = p->p_owner;
}

struct pool_args {
        size_t pa_pool_size;
        size_t pa_cache_size;
        size_t pa_max_usage;
        size_t pa_nallocs;
        size_t pa_nbytes;
};

static inline void
ptr_inc(size_t *p, size_t cap)
{
        *p = (*p + 1) & (cap - 1);
}
*/

#ifndef DB_QUERY_SIZE
#define DB_QUERY_SIZE ((1 << 10) - 1)
#endif

#ifndef POOL_SIZE
#define POOL_SIZE (1 << 16)
#endif

#ifndef HOLD_OFF_CACHE_SIZE
#define HOLD_OFF_CACHE_SIZE (1 << 4)
#endif

struct db_query {
        char dbq_str[DB_QUERY_SIZE + 1];
        FILE *dbq_fp;
};

FREE_NODE_DEF(static, struct db_query, db_query_node, POOL_SIZE)
RING_DEF(static, struct db_query_node *, db_query_node_ring, HOLD_OFF_CACHE_SIZE)

static struct db_query_node_ring ring;

/**
 * freelist
 * ========
 * ring[]   = { cache nodes for later use rather than free them immediately }
 * freelist = circular doubly linked list of fixed size arrays
 */
static LIST_DEF(freelist);
/*
static struct db_query_node *hold_off_cache[HOLD_OFF_CACHE_SIZE];
static size_t hold_off_rd = 0;
static size_t hold_off_wr = 0;
static size_t hold_off_len = 0;
*/
static uint64_t sum = 0;
static uint64_t n_alloc = 0;

static inline struct db_query_node_ptr *
freelist_get(void)
{
        struct db_query_node *fnp = NULL;
        struct list *p = NULL;

        ++n_alloc;

        LIST_FOR_EACH(&freelist, p) {
                ++sum;
                fnp = STRUCT_OF(p, struct db_query_node, fn_list);
                if (fnp->fn_count)
                        break;
        }

        if (p != &freelist)
                goto get;

        if (!db_query_node_ring_empty(&ring)) {
                fnp = db_query_node_ring_get(&ring);
                db_query_node_init(fnp);
        } else {
                fnp = db_query_node_new();
        }
        p = &fnp->fn_list;
get:
        list_rm(p);
        list_add_after(&freelist, p);
        return db_query_node_get(fnp);
}

static inline void
freelist_put(struct db_query_node_ptr **pp)
{
        struct db_query_node *owner = (*pp)->p_owner;

        db_query_node_ptr_free(pp);
        if (owner->fn_count != POOL_SIZE)
                return;

        db_query_node_ring_put(&ring, owner);
        if (db_query_node_ring_full(&ring))
                free(db_query_node_ring_get(&ring));
}

struct db_query_ptr_list {
        struct db_query_node_ptr *dqpl_ptr;
        struct list              dqpl_list;
};

int
main(int argc, char **argv)
{
        LIST_DEF(list);
        struct db_query_node_ptr *ptr = NULL;
        struct db_query_ptr_list *lp = NULL;
        struct list *p = NULL;
        struct list *next = NULL;
        int n;

        if (!argv[1])
                exit(0);

        srand(time(NULL));

        for (n = strtol(argv[1], NULL, 10); n > 0; --n) {
                if ((rand() % 2) == 0) {
                        ptr = freelist_get();
                        if (!ptr)
                                die("main(): freelist_get()");

                        lp = malloc(sizeof(*lp));
                        if (UNLIKELY(!lp))
                                die("main(): malloc()");

                        lp->dqpl_ptr = ptr;
                        list_init(&lp->dqpl_list);
                        list_add_after(&list, &lp->dqpl_list);
                } else {
                        if (list_empty(&list))
                                continue;

                        lp = STRUCT_OF(list.next,
                                        struct db_query_ptr_list,
                                        dqpl_list);
                        list_rm(&lp->dqpl_list);
                        freelist_put(&lp->dqpl_ptr);
                        free(lp);
                }
        }

        LIST_FOR_EACH_SAFE(&list, p, next) {
                lp = STRUCT_OF(p, struct db_query_ptr_list, dqpl_list);
                freelist_put(&lp->dqpl_ptr);
                free(lp);
        }

        printf("average allocation time = %lu\n", sum / n_alloc);
}
