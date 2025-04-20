#ifndef LIB_H
#define LIB_H

/**
 * va_list{}
 * va_start()
 * va_end()
 */
#include <stdarg.h>

/**
 * bool{}
 */
#include <stdbool.h>

/**
 * stderr
 * vfprintf()
 * fprintf()
 */
#include <stdio.h>

/**
 * calloc()
 * free()
 * exit()
 * EXIT_FAILURE
 */
#include <stdlib.h>

/**
 * offsetof()
 * size_t
 */
#include <stddef.h>

/**
 * memset()
 * strerror()
 */
#include <string.h>

/**
 * errno
 */
#include <errno.h>

#ifndef HASH_MULT
#define HASH_MULT 31 /* hash multiplier */
#endif /* #ifndef HASH_MULT */

/**
 * get number of elements in array:
 *
 * args:
 *  @_arr: array
 *
 * ret:
 *  number of elements in array
 */
#define ARRAY_SIZE(_arr) \
        (sizeof(_arr) / sizeof(*_arr))

/**
 * mark expression result as ignored:
 *
 * args:
 *  @_expr: expression
 *
 * ret:
 *  none
 */
#define IGNORE_RET(_expr) \
        ((void)(_expr))

/**
 * range check:
 *
 * args:
 *  @_low:  low end
 *  @_high: high end
 *  @_arg:  argument
 * 
 * ret:
 *  true if in range
 *  false if not in range
 *
 * NOTE:
 *  evaluates _arg twice
 */
#define RANGE(_low, _high, _arg) \
        ((_arg) < (_low) || (_arg) > (_high))

/**
 * zero out _size bytes pointed to by _p:
 *
 * args:
 *  @_p:    pointer
 *  @_size: number of bytes at _p
 *
 * ret:
 *  none
 */
#define ZERO(_p, _size) \
        memset(_p, 0, _size)

/**
 * zero out memory pointed to by _p:
 *
 * args:
 *  @_p: pointer
 *
 * ret:
 *  none
 */
#define ZERO_P(_p) \
        memset(_p, 0, sizeof(*(_p)))

/**
 * zero out array:
 *
 * args:
 *  @_arr: array
 *
 * ret:
 *  none
 */
#define ARRAY_ZERO(_arr) \
        ZERO(_arr, sizeof(_arr))

/**
 * get pointer to struct from pointer to field:
 *
 * args:
 *  @_p:     pointer to field
 *  @_type:  type of struct
 *  @_field: field identifier
 *
 * ret:
 *  pointer to _type
 */
#define STRUCT_OF(_p, _type, _field) \
        ((_type *)((char *)(_p) - offsetof(_type, _field)))

/**
 * call ok() function with caller set to __func__:
 *
 * args:
 *  @_id:   name of function
 *  @_args: arguments
 *
 * ret:
 *  none
 */
#define OK(_id, _args) \
        _id(_args, __func__)

/**
 * call zalloc() with caller set to __func__:
 *
 * args:
 *  @_size: number of bytes to allocate
 *
 * ret:
 *  pointer to allocated region
 */
#define ZALLOC(_size) \
        zalloc(_size, __func__)

/**
 * free memory pointed to by *_pp:
 *
 * args:
 *  @_pp: pointer to pointer
 *
 * ret:
 *  *_pp set to NULL
 */
#define PP_FREE(_pp) do { \
        free(*(_pp));     \
        *(_pp) = NULL;    \
} while (0)

/**
 * define a new ok() function:
 *
 * args:
 *  @_name: name of function
 *  @_args: args
 *  @_body: body of function
 *
 * ret:
 *  none
 */
#ifdef DBUG
#define OK_DEF(_name, _args, _body) \
/**                                 \
 * run a series of tests:           \
 *                                  \
 * args:                            \
 *  @_args:                         \
 *  @_caller:                       \
 */                                 \
static inline void                  \
_name(_args, const char *caller)    \
{                                   \
        _body                       \
}
#else
#define OK_DEF(_name, _args, _body) \
static inline void                  \
_name(_args, const char *caller)    \
{                                   \
}
#endif /* #ifdef DBUG */

/**
 * print message to stderr and exit if condition is true:
 *
 * args:
 *  @cond: condition to test
 *  @fmt:  format string
 *  @...:  variadic list
 *
 * ret:
 *  none
 */
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

/**
 * print message + errno message and exit:
 *
 * args:
 *  @fmt: format string
 *  @...: variadic list
 *
 * ret:
 *  none
 */
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

/**
 * print message and exit:
 *
 * args:
 *  @fmt: format string
 *  @...: variadic list
 *
 * ret:
 *  none
 */
static inline void
err_exit(const char *fmt, ...)
{
        va_list va;

        va_start(va, fmt);
        vfprintf(stderr, fmt, va);
        va_end(va);
        fprintf(stderr, "\n");
        exit(EXIT_FAILURE);
}

/**
 * call calloc() and die if it fails:
 *
 * args:
 *  @size:   number of bytes to allocate
 *  @caller: who called us? (for error message)
 *
 * ret:
 *  pointer to allocated region
 */
static inline void *
zalloc(size_t size, const char *caller)
{
        void *p = calloc(1, size);

        if (p == NULL)
                die("%s: zalloc()", caller);

        return p;
}

/**
 * print message + EINVAL message and exit:
 *
 * args:
 *  @fmt: format string
 *  @...: variadic list
 *
 * ret:
 *  none
 */
static inline void
usage(const char *fmt, ...)
{
        va_list va;

        va_start(va, fmt);
        vfprintf(stderr, fmt, va);
        va_end(va);
        fprintf(stderr, ": %s\n", strerror(EINVAL));
        exit(EXIT_FAILURE);
}

/**
 * hash a nil-terminated string:
 *
 * args:
 *  @s: nil-terminated string
 *
 * ret:
 *  hash of string
 */
static inline size_t
strhash(const char *s)
{
        const char *p   = NULL;
        size_t     hash = 0;

        for (p = s; *p != '\0'; p++)
                hash = hash * HASH_MULT + *p;

        return hash;
}

#endif /* #ifndef LIB_H */
