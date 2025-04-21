#include "hash.h"

#define HASH_LOAD_FACTOR ((double)0.75)

#define HASH_LIST_FOR_EACH(head, p) \
        for (p = head; p; p = p->next)

#define HASH_FOR_EACH_SAFE(hp, i, count, p, next)       \
        for (i = count = 0; count < (hp)->len; i++)     \
                for (p = (hp)->tab[i];                  \
                     p && ((next = p->next), 1);        \
                     p = next, count++)

struct hlink {
        struct hlink *next;
        size_t       hash;
        void         *ptr;
};

struct hash {
        struct hlink **tab;
        size_t       cap;
        size_t       len;
        size_t       seed;
};

static size_t hashfn(const struct hash *hp, const void *ptr);
static size_t next_prime(size_t n);
static void   hash_grow(struct hash *hp);
static bool   is_prime(size_t n);

struct hash *
hash_new(void)
{
        struct hash *hp = ZALLOC(sizeof(*hp));
        hp->seed = (size_t)time(NULL);
        hp->cap = next_prime(0);
        hp->tab = ZALLOC(sizeof(*hp->tab) * hp->cap);
        hp->len = 0;
        return hp;
}

static size_t
hashfn(const struct hash *hp, const void *ptr)
{
        uintptr_t hash = (uintptr_t)ptr;

        hash ^= hp->seed;
        hash = (~hash) + (hash << 21);
        hash = hash ^ (hash >> 24);
        hash = (hash + (hash << 3)) + (hash << 8);
        hash = hash ^ (hash >> 14);
        hash = (hash + (hash << 2)) + (hash << 4);
        hash = hash ^ (hash >> 28);
        hash = hash + (hash << 31);

        return (size_t)hash;
}

static size_t
next_prime(size_t n)
{
        while (!is_prime(n))
                n++;
        return n;
}

static bool
is_prime(size_t n)
{
        size_t m = 0;

        if (n <= 1)
                return false;

        for (m = 2; m * m <= n; m++)
                if (!(n % m))
                        return false;

        return true;
}

void
hash_free(struct hash **hpp, void (*dtor)(void *ptr))
{
        struct hlink *p = NULL;
        struct hlink *next = NULL;
        struct hash *hp = *hpp;
        size_t count = 0;
        size_t i = 0;

        HASH_FOR_EACH_SAFE(hp, i, count, p, next) {
                if (dtor)
                        dtor(p->ptr);
                free(p);
        }

        free(hp->tab);
        free(hp);
        *hpp = NULL;
}

void
hash_set(struct hash *hp, void *ptr)
{
        struct hlink *newln =  NULL;
        struct hlink *p = NULL;
        size_t hash = hashfn(hp, ptr);
        size_t i = hash % hp->cap;

        HASH_LIST_FOR_EACH(hp->tab[i], p)
                if (p->ptr == ptr)
                        return;

        if (hp->len > (HASH_LOAD_FACTOR * hp->cap)) {
                hash_grow(hp);
                i = hash % hp->cap;
        }

        newln = ZALLOC(sizeof(*newln));
        newln->hash = hash;
        newln->ptr = ptr;
        newln->next = hp->tab[i];
        hp->tab[i] = newln;
        hp->len++;
}

static void
hash_grow(struct hash *hp)
{
        struct hlink **tab = NULL;
        struct hlink *p = NULL;
        struct hlink *next = NULL;
        size_t count = 0;
        size_t cap = 0;
        size_t j = 0;
        size_t i = 0;

        cap = next_prime(hp->cap * 2);
        tab = ZALLOC(sizeof(*tab) * cap);

        HASH_FOR_EACH_SAFE(hp, i, count, p, next) {
                j = p->hash % cap;
                p->next = tab[j];
                tab[j] = p;
        }

        free(hp->tab);
        hp->tab = tab;
        hp->cap = cap;
}

void
hash_stat(const struct hash *hp, FILE *fp)
{
        struct hlink *p = NULL;
        size_t maxchain = 0;
        size_t nbytes = 0;
        size_t len = 0;
        size_t i = 0;

        if (!fp)
                fp = stdout;

        for (i = 0; i < hp->cap; i++) {
                if (!hp->tab[i]) {
                        nbytes += sizeof(void *);
                        continue;
                }
                len = 0;
                HASH_LIST_FOR_EACH(hp->tab[i], p) {
                        nbytes += sizeof(*p);
                        len++;
                }
                if (len > maxchain)
                        maxchain = len;
        }

        fprintf(fp, "hash address      : %p\n", (void *)hp);
        fprintf(fp, "table address     : %p\n", (void *)hp->tab);
        fprintf(fp, "seed:             : %zu\n", hp->seed);
        fprintf(fp, "number of entries : %zu\n", hp->len);
        fprintf(fp, "number of buckets : %zu\n", hp->cap);
        fprintf(fp, "average chain len : %zu\n", hp->len / hp->cap);
        fprintf(fp, "total size        : %zu\n", nbytes);
        fprintf(fp, "max chain length  : %zu\n", maxchain);
}

bool
hash_has(const struct hash *hp, const void *ptr)
{
        struct hlink *p = NULL;
        size_t hash = hashfn(hp, ptr);
        size_t i = hash % hp->cap;

        HASH_LIST_FOR_EACH(hp->tab[i], p)
                if (p->ptr == ptr)
                        break;

        return p != NULL;
}
