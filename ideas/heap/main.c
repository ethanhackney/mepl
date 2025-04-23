#include <assert.h>
#include <err.h>
#include <stdalign.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <sysexits.h>
#include <unistd.h>

#define SBRK_FAILED     ((void *)(-1))
#define UNUSED(expr)    ((void)(expr))

#ifndef HEAP_NODE_SIZE
#define HEAP_NODE_SIZE (1 << 10)
#endif

#define heap_node_for_each(head, p) \
        for (p = (head)->hn_next; p != (head); p = p->hn_next)

union heap_node {
        struct {
                union heap_node *hn_next;
                union heap_node *hn_prev;
                size_t           hn_size;
        };
        size_t hn_align;
};
static_assert(alignof(union heap_node) == alignof(size_t));

struct heap {
        union heap_node h_head;
        size_t          h_count;
};

static union heap_node *heap_node_new(void);
static void            *heap_get(struct heap *hp, size_t size);
static void            *heap_grow(intptr_t amt);
static void            *do_sbrk(intptr_t amt);
static void             heap_init(struct heap *hp);
static void             heap_node_init(union heap_node *hnp);
static void             heap_node_free(union heap_node **hnpp);
static void             heap_shrink(intptr_t amt);
static void             heap_free(struct heap *hp);
static void             heap_node_add(struct heap *hp, union heap_node *hnp);
static void             heap_node_rm(union heap_node *hnp);
static void             heap_put(struct heap *hp, void **pp);

int
main(void)
{
        struct heap h;
        char *s = NULL;
        
        (void)heap_node_free;
        heap_init(&h);
        s = heap_get(&h, 64);
        strcpy(s, "hello world");
        puts(s);
        heap_put(&h, (void **)&s);
        heap_free(&h);
}

static union heap_node *
heap_node_new(void)
{
        union heap_node *hnp = heap_grow(HEAP_NODE_SIZE);
        heap_node_init(hnp);
        return hnp;
}

static void *
heap_grow(intptr_t size)
{
        return do_sbrk(size);
}

static void *
do_sbrk(intptr_t amt)
{
        void *p = sbrk(amt);

        if (p == SBRK_FAILED)
                err(EX_OSERR, "do_sbrk(): sbrk()");

        return p;
}

static void
heap_node_init(union heap_node *hnp)
{
        hnp->hn_size = HEAP_NODE_SIZE;
        hnp->hn_next = hnp;
        hnp->hn_prev = hnp;
}

static void
heap_shrink(intptr_t amt)
{
        UNUSED(do_sbrk(-amt));
}

static void
heap_node_free(union heap_node **hnpp)
{
        heap_shrink(HEAP_NODE_SIZE);
        *hnpp = NULL;
}

static void
heap_init(struct heap *hp)
{
        heap_node_init(&hp->h_head);
        hp->h_count = 0;
}

static void
heap_free(struct heap *hp)
{
        heap_shrink(hp->h_count * HEAP_NODE_SIZE);
}

static void *
heap_get(struct heap *hp, size_t size)
{
        union heap_node *hnp = NULL;

        heap_node_for_each(&hp->h_head, hnp) {
                if (hnp->hn_size >= size)
                        break;
        }

        if (hnp == &hp->h_head) {
                hnp = heap_node_new();
                heap_node_add(hp, hnp);
                ++hp->h_count;
        }

        heap_node_rm(hnp);
        return (void *)(hnp + 1);
}

static void
heap_node_add(struct heap *hp, union heap_node *hnp)
{
        union heap_node *head = &hp->h_head;
        union heap_node *next = head->hn_next;

        hnp->hn_next = next;
        hnp->hn_prev = head;
        head->hn_next = hnp;
        next->hn_prev = hnp;
}

static void
heap_node_rm(union heap_node *hnp)
{
        union heap_node *next = hnp->hn_next;
        union heap_node *prev = hnp->hn_prev;

        next->hn_prev = prev;
        prev->hn_next = next;
}

static void
heap_put(struct heap *hp, void **pp)
{
        union heap_node *hnp = (union heap_node *)(*pp) - 1;

        heap_node_add(hp, hnp);
        *pp = NULL;
}
