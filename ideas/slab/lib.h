#ifndef LIB_H
#define LIB_H

#include <err.h>
#include <errno.h>
#include <pthread.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sysexits.h>

#ifndef HASH_MULT
#define HASH_MULT 31
#endif /* #ifndef HASH_MULT */

#define MUTEX_TYPE \
        pthread_mutex_t

#define COND_TYPE \
        pthread_cond_t

#define MUTEX_INIT \
        PTHREAD_MUTEX_INITIALIZER

#define COND_INIT \
        PTHREAD_COND_INITIALIZER

#define MUTEX_LOCK(MUT) do {                                    \
        errno = pthread_mutex_lock(MUT);                        \
        if (errno != 0)                                         \
                die("%s(): pthread_mutex_lock()", __func__);    \
} while (0)

#define MUTEX_UNLOCK(MUT) do {                                  \
        errno = pthread_mutex_unlock(MUT);                      \
        if (errno != 0)                                         \
                die("%s(): pthread_mutex_unlock()", __func__);  \
} while (0)

#define MUTEX_DESTROY(MUT) do {                                 \
        errno = pthread_mutex_destroy(MUT);                     \
        if (errno != 0)                                         \
                die("%s(): pthread_mutex_destroy()", __func__); \
} while (0)

#define COND_SIGNAL(COND) do {                                  \
        errno = pthread_cond_signal(COND);                      \
        if (errno != 0)                                         \
                die("%s(): pthread_cond_signal()", __func__);   \
} while (0)

#define COND_WAIT(TEST, COND, MUT) do {                                 \
        MUTEX_LOCK(MUT);                                                \
        while (!(TEST)) {                                               \
                errno = pthread_cond_wait(COND, MUT);                   \
                if (errno != 0)                                         \
                        die("%s(): pthread_cond_wait()", __func__);     \
        }                                                               \
} while (0)

#define COND_DESTROY(COND) do {                                 \
        errno = pthread_cond_destroy(COND);                     \
        if (errno != 0)                                         \
                die("%s(): pthread_cond_destroy()", __func__);  \
} while (0)

#define STRUCT_OF(P, TYPE, FIELD) \
        ((TYPE *)((char *)(P) - offsetof(TYPE, FIELD)))

#ifdef DBUG
static inline void
dbug(bool cond, const char *fmt, ...)
{
        va_list va;

        if (!cond)
                return;

        va_start(va, fmt);
        vfprintf(stderr, fmt, va);
        va_end(va);
        fprintf(stderr, "\n");
        exit(EXIT_FAILURE);
}
#else
static inline void
dbug(bool cond, const char *fmt, ...)
{
}
#endif /* #ifdef DBUG */

static inline void
die(const char *fmt, ...)
{
        va_list va;

        va_start(va, fmt);
        vfprintf(stderr, fmt, va);
        va_end(va);
        fprintf(stderr, ": %s\n", strerror(errno));
        exit(EXIT_FAILURE);
}

static inline size_t
strhash(const char *s)
{
        const char *p = NULL;
        size_t hash = 0;

        for (p = s; *p != '\0'; p++)
                hash = hash * HASH_MULT + *p;

        return hash;
}

#endif /* #ifndef LIB_H */
