#ifndef LIST_H
#define LIST_H

#include "lib.h"

#define list_empty(head) \
        ((head)->next == (head))

#define list_for_each(head, p) \
        for (p = (head)->next; p != (head); p = p->next)

#define list_for_each_safe(head, p, next) \
        for (p = (head)->next; (next = p->next), p != (head); p = next)

#define list_for_each_entry(head, p, ep, t, f) \
        for (p = (head)->next; (ep = ctor(p, t, f)), p != (head); p = p->next)

#define list_for_each_entry_safe(head, p, next, ep, t, f) \
        for (p = (head)->next; \
             (next = p->next), (ep = ctor(p, t, f)), p != (head); \
             p = next)

struct list {
        struct list *next;
        struct list *prev;
};

static inline void
list_init(struct list *p)
{
        ASSERT(p);
        p->next = p->prev = p;
}

static inline void
list_rm(struct list *p)
{
        ASSERT(p);
        ASSERT(p->next);
        ASSERT(p->prev);
        p->next->prev = p->prev;
        p->prev->next = p->next;
}

static inline void
list_add_after(struct list *before, struct list *p)
{
        ASSERT(p);
        ASSERT(before);
        ASSERT(before->next);
        p->next = before->next;
        p->prev = before;
        before->next = before->next->prev = p;
}

static inline void
list_add_before(struct list *before, struct list *p)
{
        ASSERT(p);
        ASSERT(before);
        ASSERT(before->prev);
        list_add_after(before->prev, p);
}

#endif
