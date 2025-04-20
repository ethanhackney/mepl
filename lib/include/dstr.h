#ifndef DSTR_H
#define DSTR_H

/**
 * size_t{}
 */
#include <stddef.h>

#ifndef DSTR_SHORT_SIZE
#define DSTR_SHORT_SIZE (1 << 5) - 1 /* size of short string */
#endif /* #ifndef DSTR_SHORT_SIZE */

/* dynamic string */
struct dstr {
        size_t d_cap;                        /* capacity of string */
        size_t d_len;                        /* length of string */
        char   *d_cur;                       /* either d_heap or d_short */
        char   *d_heap;                      /* heap allocated string */
        char   d_short[DSTR_SHORT_SIZE + 1]; /* short string optimization */
};

/**
 * initialize a dstr:
 *
 * args:
 *  @dsp: pointer to dstr
 *
 * ret:
 *  none
 */
void dstr_init(struct dstr *dsp);

/**
 * free a dstr:
 *
 * args:
 *  @dsp: pointer to dstr
 *
 * ret:
 *  none
 */
void dstr_free(struct dstr *dsp);

/**
 * add character onto end of string:
 *
 * args:
 *  @dsp: pointer to dstr
 *  @c:   character to add
 *
 * ret:
 *  none
 *
 * NOTE:
 *  may invalidate pointers to d_cur
 */
void dstr_push(struct dstr *dsp, int c);

/**
 * reset length of dstr to 0:
 *
 * args:
 *  @dsp: pointer to dstr
 *
 * ret:
 *  none
 */
void dstr_clear(struct dstr *dsp);

/**
 * transfer owernship of d_cur:
 *
 * args:
 *  @dsp: pointer to dstr
 *
 * ret:
 *  pointer to string
 */
char *dstr_move(struct dstr *dsp);

#endif /* #ifndef DSTR_H */
