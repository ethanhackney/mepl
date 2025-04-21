#include "bst_set.h"
#include "lib.h"

enum {
        RED,
        BLACK,
};

struct node;

struct kid {
        struct node *edge;
        struct kid  *parent;
        struct kid  *left;
        struct kid  *right;
        ptr_t       v;
        int         color;
};

struct node {
        struct kid *kids;
};

struct bst_set {
        struct ptrset set;
        struct node   *root;
};

static bool bst_set_has(const struct ptrset *psp, ptr_t *ptr);
static void bst_set_free(struct ptrset **pspp);
static void bst_set_add(struct ptrset *psp, ptr_t *ptr);

static bool do_bst_set_has(const struct node *np, ptr_t *ptr);
static void do_bst_set_free(struct node **npp);
static void do_bst_set_add(struct bst_set *bp, ptr_t *ptr);
static struct kid *kid_get_or_set(struct bst_set *bp,
                struct node *np, ptr_t chunk);
static struct kid *kid_find(const struct node *np, ptr_t chunk);
static struct kid *kid_rotate_left(struct kid *root, struct kid *x);
static struct kid *kid_rotate_right(struct kid *root, struct kid *x);
static struct kid *kid_fix(struct kid *root, struct kid *p);

struct ptrset *
bst_set_new(void)
{
        struct bst_set *bp = zalloc(sizeof(*bp));
        bp->set.ps_bytes = sizeof(*bp);
        bp->set.ps_has = bst_set_has;
        bp->set.ps_free = bst_set_free;
        bp->set.ps_add = bst_set_add;
        return &bp->set;
}

static bool
bst_set_has(const struct ptrset *psp, ptr_t *ptr)
{
        struct bst_set *bp = struct_of(psp, struct bst_set, set);

        return do_bst_set_has(bp->root, ptr);
}

static void
bst_set_free(struct ptrset **pspp)
{
        struct bst_set *bp = struct_of(*pspp, struct bst_set, set);

        do_bst_set_free(&bp->root);
        free(bp);
        *pspp = NULL;
}

static void
bst_set_add(struct ptrset *psp, ptr_t *ptr)
{
        struct bst_set *bp = struct_of(psp, struct bst_set, set);

        do_bst_set_add(bp, ptr);
}

static bool
do_bst_set_has(const struct node *np, ptr_t *ptr)
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

static void kid_free(struct kid **kpp);

static void
do_bst_set_free(struct node **npp)
{
        struct node *np = *npp;

        if (!np)
                return;

        kid_free(&np->kids);
        free(np);
        *npp = NULL;
}

static void
kid_free(struct kid **kpp)
{
        if (!*kpp)
                return;

        kid_free(&(*kpp)->left);
        kid_free(&(*kpp)->right);
        do_bst_set_free(&(*kpp)->edge);
        free(*kpp);
        *kpp = NULL;
}

static struct node *node_new(struct bst_set *bp);

static void
do_bst_set_add(struct bst_set *bp, ptr_t *ptr)
{
        struct node **npp = &bp->root;
        struct kid *kp = NULL;
        ptr_t *p = NULL;

        PTR_FOR_EACH(ptr, p) {
                if (!*npp)
                        *npp = node_new(bp);

                kp = kid_get_or_set(bp, *npp, *p);
                npp = &kp->edge;
        }

        *npp = node_new(bp);
}

static struct node *
node_new(struct bst_set *bp)
{
        struct node *np = zalloc(sizeof(*np));
        bp->set.ps_bytes += sizeof(*np);
        return np;
}

static struct kid *
kid_find(const struct node *np, ptr_t chunk)
{
        struct kid *kp = np->kids;

        while (kp) {
                if (chunk < kp->v)
                        kp = kp->left;
                else if (chunk > kp->v)
                        kp = kp->right;
                else
                        break;
        }

        return kp;
}

static struct kid *
kid_get_or_set(struct bst_set *bp, struct node *np, ptr_t chunk)
{
        struct kid **kpp = &np->kids;
        struct kid *newk = NULL;
        struct kid *kp = NULL;

        while (*kpp) {
                kp = *kpp;
                if (chunk < kp->v)
                        kpp = &kp->left;
                else if (chunk > kp->v)
                        kpp = &kp->right;
                else
                        return kp;
        }

        newk = zalloc(sizeof(*newk));
        newk->parent = kp;
        newk->v = chunk;
        *kpp = newk;
        np->kids = kid_fix(np->kids, newk);
        return newk;
}

static struct kid *
kid_rotate_left(struct kid *root, struct kid *x)
{
        struct kid *y = x->right;

        x->right = y->left;
        if (y->left)
                y->left->parent = x;

        y->parent = x->parent;
        if (!x->parent)
                root = y;
        else if (x == x->parent->left)
                x->parent->left = y;
        else
                x->parent->right = y;

        y->left = x;
        x->parent = y;
        return root;
}

static struct kid *
kid_rotate_right(struct kid *root, struct kid *x)
{
        struct kid *y = x->left;

        x->left = y->right;
        if (y->right)
                y->right->parent = x;

        y->parent = x->parent;
        if (!x->parent)
                root = y;
        else if (x == x->parent->right)
                x->parent->right = y;
        else
                x->parent->left = y;

        y->right = x;
        x->parent = y;
        return root;
}

static struct kid *
kid_fix(struct kid *root, struct kid *p)
{
        struct kid *parent = NULL;
        struct kid *uncle = NULL;
        struct kid *gp = NULL;

        while (p != root && p->parent->color == RED) {
                parent = p->parent;
                gp = parent->parent;

                if (parent == gp->left) {
                        uncle = gp->right;
                        if (uncle && uncle->color == RED) {
                                parent->color = BLACK;
                                uncle->color = BLACK;
                                gp->color = RED;
                                p = gp;
                        } else {
                                if (p == parent->right) {
                                        p = parent;
                                        root = kid_rotate_left(root, p);
                                }
                                parent->color = BLACK;
                                gp->color = RED;
                                root = kid_rotate_right(root, gp);
                        }
                } else {
                        uncle = gp->left;
                        if (uncle && uncle->color == RED) {
                                parent->color = BLACK;
                                uncle->color = BLACK;
                                gp->color = RED;
                                p = gp;
                        } else {
                                if (p == parent->left) {
                                        p = parent;
                                        root = kid_rotate_right(root, p);
                                }
                                parent->color = BLACK;
                                gp->color = RED;
                                root = kid_rotate_left(root, gp);
                        }
                }
        }
        root->color = BLACK;
        return root;
}
