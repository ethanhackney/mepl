#ifndef BUF_CSTR_H
#define BUF_CSTR_H

/**
 * buf{}
 * BUF_EOF
 */
#include <buf.h>

/* c-style nil-terminated string buf */
struct buf_cstr {
        struct buf c_buf;  /* base buf */
        const char *c_p;   /* next char to read */
        char       *c_str; /* nil-terminated string */
};

/**
 * initialize buf_cstr:
 *
 * args:
 *  @bsp:   pointer to buf_cstr
 *  @strpp: pointer to pointer to string
 *
 * ret:
 *  *strpp set to NULL
 */
void buf_cstr_init(struct buf_cstr *bsp, char **strpp);

/**
 * free buf_cstr:
 *
 * args:
 *  @bsp:   pointer to buf_cstr
 *  @strpp: pointer to pointer to string
 *
 * ret:
 *  *strpp set to bsp->c_str if strpp != NULL
 */
void buf_cstr_free(struct buf_cstr *bsp, char **strpp);

#endif /* #ifndef BUF_CSTR_H */
