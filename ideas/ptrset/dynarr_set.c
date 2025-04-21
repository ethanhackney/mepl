#include "dynarr_set.h"
#include "lib.h"

#define KID_END(np) \
        ((np)->kids + (np)->len)

#define KID_FOR_EACH(np, kp) \
        for (kp = (np)->kids; kp < KID_END(np); kp++)

struct node;

struct kid {
        struct node *edge;
        ptr_t       v;
};

struct node {
        struct kid *kids;
        size_t     len;
        size_t     cap;
};

struct dynarr_set {
        struct ptrset set;
        struct node   *root;
};

static bool dynarr_set_has(const struct ptrset *psp, ptr_t *ptr);
static void dynarr_set_free(struct ptrset **pspp);
static void dynarr_set_add(struct ptrset *psp, ptr_t *ptr);

static bool do_dynarr_set_has(const struct node *np, ptr_t *ptr);
static void do_dynarr_set_free(struct node **npp);
static void do_dynarr_set_add(struct dynarr_set *dp, ptr_t *ptr);
static void node_push(struct dynarr_set *dp, struct node *np, ptr_t chunk);
static void node_grow(struct dynarr_set *dp, struct node *np);
static struct kid *kid_find(const struct node *np, ptr_t chunk);

struct ptrset *
dynarr_set_new(void)
{
        struct dynarr_set *dp = calloc(1, sizeof(*dp));

        if (!dp)
                die("dynarr_set_new(): calloc()");

        dp->set.ps_bytes = sizeof(*dp);
        dp->set.ps_has = dynarr_set_has;
        dp->set.ps_free = dynarr_set_free;
        dp->set.ps_add = dynarr_set_add;
        return &dp->set;
}

static bool
dynarr_set_has(const struct ptrset *psp, ptr_t *ptr)
{
        struct dynarr_set *dp = struct_of(psp, struct dynarr_set, set);

        return do_dynarr_set_has(dp->root, ptr);
}

static void
dynarr_set_free(struct ptrset **pspp)
{
        struct dynarr_set *dp = struct_of(*pspp, struct dynarr_set, set);

        do_dynarr_set_free(&dp->root);
        free(dp);
        *pspp = NULL;
}

static void
dynarr_set_add(struct ptrset *psp, ptr_t *ptr)
{
        struct dynarr_set *dp = struct_of(psp, struct dynarr_set, set);

        do_dynarr_set_add(dp, ptr);
}

static bool
do_dynarr_set_has(const struct node *np, ptr_t *ptr)
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
do_dynarr_set_free(struct node **npp)
{
        struct node *np = *npp;
        struct kid *kp = NULL;

        if (!np)
                return;

        KID_FOR_EACH(np, kp)
                do_dynarr_set_free(&kp->edge);

        free(np);
        *npp = NULL;
}

static struct node *dynarr_set_node_new(struct dynarr_set *dp);

static void
do_dynarr_set_add(struct dynarr_set *dp, ptr_t *ptr)
{
        struct node **npp = &dp->root;
        struct kid *kp = NULL;
        ptr_t *p = NULL;

        PTR_FOR_EACH(ptr, p) {
                if (!*npp)
                        *npp = dynarr_set_node_new(dp);

                kp = kid_find(*npp, *p);
                if (kp) {
                        npp = &kp->edge;
                        continue;
                }

                node_push(dp, *npp, *p);
                npp = &(*npp)->kids[(*npp)->len - 1].edge;
        }

        *npp = dynarr_set_node_new(dp);
}

static struct node *
dynarr_set_node_new(struct dynarr_set *dp)
{
        struct node *np = calloc(1, sizeof(*np));

        if (!np)
                die("dynarr_set_node_new(): calloc()");

        dp->set.ps_bytes += sizeof(*np);
        return np;
}

static void
node_push(struct dynarr_set *dp, struct node *np, ptr_t chunk)
{
        if (np->len == np->cap)
                node_grow(dp, np);

        np->kids[np->len].v = chunk;
        np->len++;
}

static void
node_grow(struct dynarr_set *dp, struct node *np)
{
        struct kid *kids = np->kids;
        size_t cap = np->cap * 2;

        if (!cap)
                cap = 1;

        kids = realloc(kids, sizeof(*kids) * cap);
        if (!kids)
                die("node_grow(): calloc()");

        dp->set.ps_bytes += sizeof(*kids) * (cap - np->cap);
        np->kids = kids;
        np->cap = cap;
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
