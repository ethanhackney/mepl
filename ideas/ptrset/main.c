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
        struct ptrset *psp = NULL;

        if (!argv[1]) {
                printf("no arg given\n");
                exit(1);
        } else if (!strcmp(argv[1], "fixset")) {
                psp = fixset_new();
        } else if (!strcmp(argv[1], "dynarr_set")) {
                psp = dynarr_set_new();
        } else if (!strcmp(argv[1], "list_set")) {
                psp = list_set_new();
        } else {
                printf("no set named %s\n", argv[1]);
                exit(1);
        }

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
