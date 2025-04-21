#include "list_set.h"
#include "lib.h"

#define KID_FOR_EACH(np, kp) \
        for (kp = (np)->kids; kp; kp = kp->next)

#define KID_FOR_EACH_PP(np, kpp) \
        for (kpp = &(np)->kids; *kpp; kpp = &(*kpp)->next)

#define KID_FOR_EACH_SAFE(np, kp, next) \
        for (kp = (np)->kids; kp && ((next = kp->next), 1); kp = next)

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
static struct kid *kid_get_or_set(struct list_set *lp,
                struct node *np, ptr_t chunk);
static struct kid *kid_find(const struct node *np, ptr_t chunk);

struct ptrset *
list_set_new(void)
{
        struct list_set *lp = zalloc(sizeof(*lp));
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
        struct kid *next = NULL;
        struct kid *kp = NULL;

        if (!np)
                return;

        KID_FOR_EACH_SAFE(np, kp, next) {
                do_list_set_free(&kp->edge);
                free(kp);
        }

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

                kp = kid_get_or_set(lp, *npp, *p);
                npp = &kp->edge;
        }

        *npp = node_new(lp);
}

static struct node *
node_new(struct list_set *lp)
{
        struct node *np = zalloc(sizeof(*np));
        lp->set.ps_bytes += sizeof(*np);
        return np;
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

static struct kid *
kid_get_or_set(struct list_set *lp, struct node *np, ptr_t chunk)
{
        struct kid **kpp = NULL;
        struct kid *kp = NULL;

        KID_FOR_EACH_PP(np, kpp) {
                if ((*kpp)->v == chunk)
                        return *kpp;
                if ((*kpp)->v > chunk)
                        break;
        }

        kp = zalloc(sizeof(*kp));
        kp->v = chunk;
        kp->next = *kpp;
        *kpp = kp;
        lp->set.ps_bytes += sizeof(*kp);
        return kp;
}
