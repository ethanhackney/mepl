#include "free_node.h"
#include "lib.h"
#include "list.h"
#include <stdio.h>
#include <stdlib.h>

#ifndef DB_QUERY_SIZE
#define DB_QUERY_SIZE ((1 << 10) - 1)
#endif

#ifndef POOL_SIZE
#define POOL_SIZE (1 << 16)
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

        LIST_FOR_EACH(&freelist, p) {
                fnp = STRUCT_OF(p, struct db_query_node, fn_list);
                if (fnp->fn_count)
                        break;
        }

        if (p == &freelist) {
                fnp = db_query_node_new();
                p = &fnp->fn_list;
        }

        list_rm(p);
        list_add_after(&freelist, p);
        return db_query_node_get(fnp);
}

int
main(void)
{
}
