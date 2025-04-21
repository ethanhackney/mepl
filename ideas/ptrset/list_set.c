#include "list_set.h"
#include "lib.h"

#define KID_FOR_EACH(np, kp) \
        for (kp = (np)->kids; kp; kp = kp->next)

struct node;

struct kid {
        struct node *edge;
        struct kid  *next;
        ptr_t       v;
};

struct node {
        struct kid *kids;
};

struct list_set {
        struct ptrset set;
        struct node   *root;
};

static bool list_set_has(const struct ptrset *psp, ptr_t *ptr);
static void list_set_free(struct ptrset **pspp);
static void list_set_add(struct ptrset *psp, ptr_t *ptr);

static bool do_list_set_has(const struct node *np, ptr_t *ptr);
static void do_list_set_free(struct node **npp);
static void do_list_set_add(struct list_set *lp, ptr_t *ptr);
static void node_push(struct list_set *lp, struct node *np, ptr_t chunk);
static struct kid *kid_find(const struct node *np, ptr_t chunk);

struct ptrset *
list_set_new(void)
{
        struct list_set *lp = calloc(1, sizeof(*lp));

        if (!lp)
                die("list_set_new(): calloc()");

        lp->set.ps_bytes = sizeof(*lp);
        lp->set.ps_has = list_set_has;
        lp->set.ps_free = list_set_free;
        lp->set.ps_add = list_set_add;
        return &lp->set;
}

static bool
list_set_has(const struct ptrset *psp, ptr_t *ptr)
{
        struct list_set *lp = struct_of(psp, struct list_set, set);

        return do_list_set_has(lp->root, ptr);
}

static void
list_set_free(struct ptrset **pspp)
{
        struct list_set *lp = struct_of(*pspp, struct list_set, set);

        do_list_set_free(&lp->root);
        free(lp);
        *pspp = NULL;
}

static void
list_set_add(struct ptrset *psp, ptr_t *ptr)
{
        struct list_set *lp = struct_of(psp, struct list_set, set);

        do_list_set_add(lp, ptr);
}

static bool
do_list_set_has(const struct node *np, ptr_t *ptr)
{
        struct kid *kp = NULL;
        ptr_t *p = NULL;

        PTR_FOR_EACH(ptr, p) {
                if (!np)
                        return false;

                kp = kid_find(np, *p);
                if (!kp)
                        return false;

                np = kp->edge;
        }

        return true;
}

static void
do_list_set_free(struct node **npp)
{
        struct node *np = *npp;
        struct kid *kp = NULL;

        if (!np)
                return;

        KID_FOR_EACH(np, kp)
                do_list_set_free(&kp->edge);

        free(np);
        *npp = NULL;
}

static struct node *node_new(struct list_set *lp);

static void
do_list_set_add(struct list_set *lp, ptr_t *ptr)
{
        struct node **npp = &lp->root;
        struct kid *kp = NULL;
        ptr_t *p = NULL;

        PTR_FOR_EACH(ptr, p) {
                if (!*npp)
                        *npp = node_new(lp);

                kp = kid_find(*npp, *p);
                if (kp) {
                        npp = &kp->edge;
                        continue;
                }

                node_push(lp, *npp, *p);
                npp = &(*npp)->kids->edge;
        }

        *npp = node_new(lp);
}

static struct node *
node_new(struct list_set *lp)
{
        struct node *np = calloc(1, sizeof(*np));

        if (!np)
                die("node_new(): calloc()");

        lp->set.ps_bytes += sizeof(*np);
        return np;
}

static void
node_push(struct list_set *lp, struct node *np, ptr_t chunk)
{
        struct kid *kp = calloc(1, sizeof(*kp));

        if (!kp)
                die("node_push(): calloc()");

        kp->v = chunk;
        kp->next = np->kids;
        np->kids = kp;
        lp->set.ps_bytes += sizeof(*kp);
}

static struct kid *
kid_find(const struct node *np, ptr_t chunk)
{
        struct kid *kp = NULL;

        KID_FOR_EACH(np, kp) {
                if (kp->v == chunk)
                        break;
        }

        return kp;
}
