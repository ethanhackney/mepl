#ifndef BUF_NSTR_H
#define BUF_NSTR_H

/**
 * buf{}
 * BUF_EOF
 */
#include <buf.h>

/**
 * size_t{}
 */
#include <stddef.h>

/* non nil-terminated string buf */
struct buf_nstr {
        struct buf n_buf;  /* base buf */
        size_t     n_len;  /* length of string */
        char       *n_str; /* string */
        char       *n_p;   /* next byte to read */
};

/**
 * initialize buf_nstr:
 *
 * args:
 *  @bnp:   pointer to buf_nstr
 *  @strpp: pointer to pointer to string
 *  @len:   length of string
 *
 * ret:
 *  none
 */
void buf_nstr_init(struct buf_nstr *bnp, char **strpp, size_t len);

/**
 * free buf_nstr:
 *
 * args:
 *  @bnp:   pointer to buf_nstr
 *  @strpp: pointer to pointer to string
 *
 * ret:
 *  *strpp set to bnp->n_str if strpp != NULL
 */
void buf_nstr_free(struct buf_nstr *bnp, char **strpp);

#endif /* #ifndef BUF_NSTR_H */
