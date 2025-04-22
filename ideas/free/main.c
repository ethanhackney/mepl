#include "free_node.h"
#include "lib.h"
#include "list.h"
#include <stdio.h>
#include <stdlib.h>

#ifndef DB_QUERY_SIZE
#define DB_QUERY_SIZE ((1 << 10) - 1)
#endif

#ifndef POOL_SIZE
#define POOL_SIZE (1 << 8)
#endif

struct db_query {
        char dbq_str[DB_QUERY_SIZE + 1];
        FILE *dbq_fp;
};

FREE_NODE_DEF(static, struct db_query, db_query_node, POOL_SIZE)

static LIST_DEF(freelist);

static inline struct db_query_node_ptr *
freelist_get(void)
{
        struct db_query_node *fnp = NULL;
        struct list *p = NULL;
        int iters = 0;

        LIST_FOR_EACH(&freelist, p) {
                ++iters;
                fnp = STRUCT_OF(p, struct db_query_node, fn_list);
                if (fnp->fn_count)
                        break;
        }

        if (p == &freelist) {
                fnp = db_query_node_new();
                p = &fnp->fn_list;
        }

        printf("%d iters to find a node\n", iters);
        list_rm(p);
        list_add_after(&freelist, p);
        return db_query_node_get(fnp);
}

static inline void
freelist_put(struct db_query_node_ptr **pp)
{
        struct db_query_node *owner = (*pp)->p_owner;

        db_query_node_ptr_free(pp);
        if (owner->fn_count == POOL_SIZE) {
                list_rm(&owner->fn_list);
                free(owner);
        }
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

        for (n = strtol(argv[1], NULL, 10); n > 0; --n) {
                ptr = freelist_get();
                if (!ptr)
                        die("main(): freelist_get()");

                lp = malloc(sizeof(*lp));
                if (UNLIKELY(!lp))
                        die("main(): malloc()");

                lp->dqpl_ptr = ptr;
                list_init(&lp->dqpl_list);
                list_add_after(&list, &lp->dqpl_list);
        }

        LIST_FOR_EACH_SAFE(&list, p, next) {
                lp = STRUCT_OF(p, struct db_query_ptr_list, dqpl_list);
                freelist_put(&lp->dqpl_ptr);
                free(lp);
        }
}
