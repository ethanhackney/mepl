#include <assert.h>
#include <err.h>
#include <errno.h>
#include <malloc.h>
#include <pthread.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sysexits.h>
#include <time.h>

#define PROF
#ifdef PROF
#define INLINE
#else
#define INLINE inline
#endif

// #define DBUG
#ifdef DBUG
#define ASSERT(cond) assert(cond)
#else
#define ASSERT(cond)
#endif

static INLINE void
die(const char *fmt, ...)
{
        va_list va;

        ASSERT(fmt);
        va_start(va, fmt);
        vfprintf(stderr, fmt, va);
        va_end(va);
        fprintf(stderr, ": %s\n", strerror(errno));
        exit(EXIT_FAILURE);
}

#define pp_free(pp) do {        \
        ASSERT(*(pp));          \
        free(*(pp));            \
        *(pp) = NULL;           \
} while (0)

#ifndef DBUG
static INLINE bool
is_pow_of_2(size_t n) __attribute__((unused));
#endif

static INLINE bool
is_pow_of_2(size_t n)
{
        return n && !(n & (n - 1));
}

struct darr_ptr {
        struct darr_ptr *next;
        struct darr_ptr *prev;
        void           **ptr;
};

#define darr_ptr_ok(p) do {     \
        ASSERT((p));            \
        ASSERT((p)->next);      \
        ASSERT((p)->prev);      \
} while (0)

#define darr_ptr_for_each(head, p) \
        for (p = (head)->next; p != (head); p = p->next)

#define darr_ptr_for_each_safe(head, p, next) \
        for (p = (head)->next; (next = p->next), p != (head); p = next)

static INLINE void
darr_ptr_init(struct darr_ptr *p, void **ptr)
{
        ASSERT(p);
        p->next = p;
        p->prev = p;
        p->ptr = ptr;
}

static INLINE void
darr_ptr_add(struct darr_ptr *head, struct darr_ptr *new)
{
        struct darr_ptr *next = NULL;

        darr_ptr_ok(head);
        ASSERT(new);

        next = head->next;
        new->next = next;
        new->prev = head;
        head->next = new;
        next->prev = new;
}

static INLINE void
darr_ptr_rm(struct darr_ptr *p)
{
        struct darr_ptr *next = NULL;
        struct darr_ptr *prev = NULL;

        darr_ptr_ok(p);

        next = p->next;
        prev = p->prev;
        next->prev = prev;
        prev->next = next;
}

#ifndef DBUG
static INLINE bool
darr_ptr_is_mine(struct darr_ptr *head, struct darr_ptr *ptr) __attribute__((unused));
#endif

static INLINE bool
darr_ptr_is_mine(struct darr_ptr *head, struct darr_ptr *ptr)
{
        struct darr_ptr *p = NULL;

        darr_ptr_ok(head);
        darr_ptr_ok(ptr);

        darr_ptr_for_each(head, p) {
                if (p == ptr)
                        return true;
        }

        return false;
}

#ifndef DBUG
static INLINE bool
darr_ptr_range_check(struct darr_ptr *head, void *arr, size_t len) __attribute__((unused));
#endif

static INLINE bool
darr_ptr_range_check(struct darr_ptr *head, void *arr, size_t len)
{
        struct darr_ptr *p = NULL;
        uintptr_t ap = (uintptr_t)arr;
        uintptr_t cur = 0;

        ASSERT(arr);
        darr_ptr_ok(head);

        darr_ptr_for_each(head, p) {
                cur = (uintptr_t)*p->ptr;
                if (cur < ap || cur >= ap + len)
                        return false;
        }

        return true;
}

#define darr_for_each(dp, p) \
        for (p = (dp)->d_arr; p < (dp)->d_arr + (dp)->d_len; ++p)

#ifdef DBUG
#define darr_ptr_list_ok(dp) do {                                       \
        darr_ptr_range_check(&(dp)->d_ptrs, (dp)->d_arr, (dp)->d_len);  \
} while (0)
#else
#define darr_ptr_list_ok(dp)
#endif

#define darr_ok(dp) do {                        \
        ASSERT((dp));                           \
        ASSERT((dp)->d_arr);                    \
        ASSERT(is_pow_of_2((dp)->d_cap));       \
        ASSERT((dp)->d_len <= (dp)->d_cap);     \
        darr_ptr_list_ok((dp));                 \
} while (0)

#ifdef DBUG
#define darr_ptr_is_ok(dp, p) do {                      \
        ASSERT(darr_ptr_is_mine(&(dp)->d_ptrs, (p)));   \
} while (0)
#else
#define darr_ptr_is_ok(dp, p)
#endif

#define darr_size(dp) \
        (sizeof(*(dp)->d_arr) * (dp)->d_cap)

enum {
        DARR_CAP_INIT = (1ULL << 30),
};

#define PROT_ARG (PROT_READ | PROT_WRITE)
#define MAP_ARG  (MAP_PRIVATE | MAP_ANONYMOUS)

static INLINE void *
do_mmap(size_t size, const char *caller)
{
        void *p = NULL;

        ASSERT(size);
        p = mmap(NULL, size, PROT_ARG, MAP_ARG, -1, 0);
        if (p == MAP_FAILED)
                die("%s: mmap()");

        return p;
}

#define DARR_DEF(LINKAGE, T, NAME)                                      \
                                                                        \
struct NAME {                                                           \
        struct darr_ptr  d_ptrs;                                        \
        size_t           d_cap;                                         \
        size_t           d_len;                                         \
        T               *d_arr;                                         \
};                                                                      \
                                                                        \
LINKAGE INLINE void                                                     \
NAME ## _init(struct NAME *dp)                                          \
{                                                                       \
        ASSERT(dp);                                                     \
                                                                        \
        dp->d_cap = DARR_CAP_INIT;                                      \
        dp->d_arr = do_mmap(darr_size(dp), "darr_init");                \
        dp->d_len = 0;                                                  \
        darr_ptr_init(&dp->d_ptrs, NULL);                               \
}                                                                       \
                                                                        \
LINKAGE INLINE void                                                     \
NAME ## _unmap(struct NAME *dp)                                         \
{                                                                       \
        if (munmap(dp->d_arr, darr_size(dp)) < 0)                       \
                die("darr_unmap(): munmap()");                          \
}                                                                       \
                                                                        \
LINKAGE INLINE void                                                     \
NAME ## _free(struct NAME *dp)                                          \
{                                                                       \
        struct darr_ptr *next = NULL;                                   \
        struct darr_ptr *p = NULL;                                      \
                                                                        \
        darr_ok(dp);                                                    \
                                                                        \
        darr_ptr_for_each_safe(&dp->d_ptrs, p, next) {                  \
                *p->ptr = NULL;                                         \
                pp_free(&p);                                            \
        }                                                               \
        darr_ptr_init(&dp->d_ptrs, NULL);                               \
                                                                        \
        NAME ## _unmap(dp);                                             \
        dp->d_arr = NULL;                                               \
}                                                                       \
                                                                        \
LINKAGE INLINE struct darr_ptr *                                        \
NAME ## _add_ptr(struct NAME *dp, T **pp)                               \
{                                                                       \
        struct darr_ptr *p = NULL;                                      \
                                                                        \
        darr_ok(dp);                                                    \
        ASSERT(dp->d_arr <= *pp && *pp < dp->d_arr + dp->d_len);        \
                                                                        \
        p = malloc(sizeof(*p));                                         \
        if (!p)                                                         \
                die("darr_get_ptr(): malloc()");                        \
                                                                        \
        darr_ptr_init(p, (void **)pp);                                  \
        darr_ptr_add(&dp->d_ptrs, p);                                   \
        return p;                                                       \
}                                                                       \
                                                                        \
LINKAGE INLINE void                                                     \
NAME ## _rm_ptr(struct NAME *dp, struct darr_ptr **pp)                  \
{                                                                       \
        darr_ok(dp);                                                    \
        darr_ptr_is_ok(dp, *pp);                                        \
        darr_ptr_rm(*pp);                                               \
        pp_free(pp);                                                    \
}                                                                       \
                                                                        \
LINKAGE INLINE void                                                     \
NAME ## _grow(struct NAME *dp)                                          \
{                                                                       \
        struct darr_ptr *p = NULL;                                      \
        size_t cap = 0;                                                 \
        size_t i = 0;                                                   \
        T *arr = NULL;                                                  \
        T *new = NULL;                                                  \
                                                                        \
        darr_ok(dp);                                                    \
                                                                        \
        cap = dp->d_cap << 1;                                           \
        new = do_mmap(sizeof(T) * cap, "darr_grow");                    \
        memcpy(new, dp->d_arr, darr_size(dp));                          \
                                                                        \
        darr_ptr_for_each(&dp->d_ptrs, p) {                             \
                i = ((T *)*p->ptr) - dp->d_arr;                         \
                *p->ptr = &new[i];                                      \
        }                                                               \
                                                                        \
        NAME ## _unmap(dp);                                             \
        dp->d_arr = arr;                                                \
        dp->d_cap = cap;                                                \
}                                                                       \
                                                                        \
LINKAGE INLINE void                                                     \
NAME ## _push(struct NAME *dp, T v)                                     \
{                                                                       \
        if (dp->d_len == dp->d_cap)                                     \
                NAME ## _grow(dp);                                      \
                                                                        \
        dp->d_arr[dp->d_len] = v;                                       \
        ++dp->d_len;                                                    \
}

DARR_DEF(static, int, intarr)
static pthread_mutex_t nums_mut = PTHREAD_MUTEX_INITIALIZER;
static struct intarr   nums;
static size_t          nswitches;

struct range {
        int low, high;
};

struct ptr {
        struct ptr *next;
        int *p;
};

static struct ptr *ptrs;
static pthread_mutex_t ptrs_mut = PTHREAD_MUTEX_INITIALIZER;

static void
ptrs_free(void)
{
        struct ptr *p = NULL;
        struct ptr *next = NULL;

        for (p = ptrs; p; p = next) {
                next = p->next;
                pp_free(&p);
        }
}

static void
add_num(int n)
{
        struct ptr *p = NULL;
        long r = 0;
        int *old = NULL;

        pthread_mutex_lock(&nums_mut);

        old = nums.d_arr;
        intarr_push(&nums, n);
        if (nums.d_arr != old)
                ++nswitches;

        if (!(n % 2)) {
                pthread_mutex_unlock(&nums_mut);

                p = malloc(sizeof(*p));
                if (!p)
                        die("add_num(): malloc()");

                pthread_mutex_lock(&ptrs_mut);
                p->next = ptrs;
                ptrs = p;
                pthread_mutex_unlock(&ptrs_mut);

                r = rand();
                pthread_mutex_lock(&nums_mut);
                /* NOTE: fine with skewed distribution from bit operation */
                r %= nums.d_len;
                ASSERT(0 <= r && r < nums.d_len);
                p->p = &nums.d_arr[r];
                intarr_add_ptr(&nums, &p->p);
        }

        pthread_mutex_unlock(&nums_mut);
}

static void *
workerbee(void *arg)
{
        struct range *rp = arg;
        int i = 0;

        for (i = rp->low; i < rp->high; ++i)
                add_num(i);

        return NULL;
}

enum {
        NR_THREADS = (1 << 5),
};

int
main(int argc, char **argv)
{
        pthread_t tids[NR_THREADS];
        struct range ranges[NR_THREADS];
        struct range *rp = ranges;
        long low = 0;
        long high = 0;
        long n = 0;
        int i = 0;

        ASSERT(argv[1]);
        n = strtol(argv[1], NULL, 10);
        high = n / NR_THREADS;

        srand(time(NULL));
        intarr_init(&nums);

        for (i = 0; i < NR_THREADS; ++i) {
                rp->low = low;
                rp->high = high;
                pthread_create(&tids[i], NULL, workerbee, rp);
                low = high;
                high += (n / NR_THREADS);
                ++rp;
        }

        for (i = 0; i < NR_THREADS; i++)
                pthread_join(tids[i], NULL);

        (void)intarr_rm_ptr;
        intarr_free(&nums);
        ptrs_free();
        printf("%zu switches\n", nswitches);
}
