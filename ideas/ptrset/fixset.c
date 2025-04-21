#include "fixset.h"
#include "lib.h"

struct node {
        struct node *kids[FIXSET_SIZE];
};

struct fixset {
        struct ptrset set;
        struct node   *root;
};

static bool fixset_has(const struct ptrset *psp, ptr_t *ptr);
static void fixset_free(struct ptrset **pspp);
static void fixset_add(struct ptrset *psp, ptr_t *ptr);

static bool do_fixset_has(const struct node *np, ptr_t *ptr);
static void do_fixset_free(struct node **npp);
static void do_fixset_add(struct fixset *fp, ptr_t *ptr);

struct ptrset *
fixset_new(void)
{
        struct fixset *fp = calloc(1, sizeof(*fp));

        if (!fp)
                die("fixset_new(): calloc()");

        fp->set.ps_bytes = sizeof(*fp);
        fp->set.ps_has = fixset_has;
        fp->set.ps_free = fixset_free;
        fp->set.ps_add = fixset_add;
        return &fp->set;
}

static bool
fixset_has(const struct ptrset *psp, ptr_t *ptr)
{
        struct fixset *fp = struct_of(psp, struct fixset, set);

        return do_fixset_has(fp->root, ptr);
}

static void
fixset_free(struct ptrset **pspp)
{
        struct fixset *fp = struct_of(*pspp, struct fixset, set);

        do_fixset_free(&fp->root);
        free(fp);
        *pspp = NULL;
}

static void
fixset_add(struct ptrset *psp, ptr_t *ptr)
{
        struct fixset *fp = struct_of(psp, struct fixset, set);

        do_fixset_add(fp, ptr);
}

static bool
do_fixset_has(const struct node *np, ptr_t *ptr)
{
        ptr_t *p = NULL;

        PTR_FOR_EACH(ptr, p) {
                if (!np)
                        return false;
                np = np->kids[*p];
        }

        return true;
}

static void
do_fixset_free(struct node **npp)
{
        struct node *np = *npp;
        int i = 0;

        if (!np)
                return;

        for (i = 0; i < FIXSET_SIZE; i++)
                do_fixset_free(&np->kids[i]);

        free(np);
        *npp = NULL;
}

static struct node *node_new(struct fixset *fp);

static void
do_fixset_add(struct fixset *fp, ptr_t *ptr)
{
        struct node **npp = &fp->root;
        ptr_t *p = NULL;

        PTR_FOR_EACH(ptr, p) {
                if (!*npp)
                        *npp = node_new(fp);

                npp = &(*npp)->kids[*p];
        }

        *npp = node_new(fp);
}

static struct node *
node_new(struct fixset *fp)
{
        struct node *np = calloc(1, sizeof(*np));

        if (!np)
                die("node_new(): calloc()");

        fp->set.ps_bytes += sizeof(*np);
        return np;
}
