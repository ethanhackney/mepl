#include "bst_set.h"
#include "dynarr_set.h"
#include "fixset.h"
#include "list_set.h"
#include "ptrset.h"
#include <err.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sysexits.h>

static void ptrset_test(struct ptrset *psp);

int
main(int argc, char **argv)
{
        struct kvp {
                char *name;
                struct ptrset *(*newfn)(void);
        } kvps[] = {
                { "fixset",     fixset_new     },
                { "dynarr_set", dynarr_set_new },
                { "list_set",   list_set_new   },
                { "bst_set",    bst_set_new    },
                { NULL,         NULL           },
        };
        struct kvp *kp = NULL;

        struct ptrset *psp = NULL;

        if (!argv[1]) {
                printf("no arg given\n");
                exit(1);
        }

        for (kp = kvps; kp->name; kp++) {
                if (!strcmp(argv[1], kp->name))
                        break;
        }

        if (!kp->name) {
                printf("no set named %s\n", argv[1]);
                exit(1);
        }

        psp = kp->newfn();
        ptrset_test(psp);
        psp->ps_free(&psp);
}

static void
ptrset_test(struct ptrset *psp)
{
        union {
                void *p;
                ptr_t addr[ADDR_LEN];
        } u[1 << 16];
        int size = sizeof(u) / sizeof(*u);
        int i = 0;

        for (i = 0; i < size; i++) {
                u[i].p = strdup("hello world");
                psp->ps_add(psp, u[i].addr);
                free(u[i].p);
        }
        printf("usage: %zu\n", psp->ps_bytes);
}
