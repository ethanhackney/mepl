#ifndef LIST_H
#define LIST_H

#define LIST_FOR_EACH(head, p) \
        for (p = (head)->next; p != (head); p = p->next)

#define LIST_FOR_EACH_SAFE(head, p, next) \
        for (p = (head)->next; (next = p->next), p != (head); p = next)

#define LIST_DEF(name)          \
        struct list name = {    \
                .next = &name,  \
                .prev = &name,  \
        }

struct list {
        struct list *next;
        struct list *prev;
};

static inline void
list_init(struct list *p)
{
        p->next = p;
        p->prev = p;
}

static inline void
list_rm(struct list *p)
{
        struct list *next = p->next;
        struct list *prev = p->prev;

        next->prev = prev;
        prev->next = next;
}

static inline void
list_add_after(struct list *before, struct list *p)
{
        struct list *next = before->next;

        p->next = next;
        p->prev = before;
        before->next = p;
        next->prev = p;
}

#endif
