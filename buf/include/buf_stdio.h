#ifndef BUF_STDIO_H
#define BUF_STDIO_H

/**
 * buf{}
 * BUF_EOF
 */
#include <buf.h>

/**
 * FILE{}
 * fgets()
 */
#include <stdio.h>

#ifndef BUF_STDIO_DATA_SIZE
#define BUF_STDIO_DATA_SIZE ((1 << 13) - 1) /* buf_stdio data size */
#endif /* #ifndef BUF_STDIO_DATA_SIZE */

/* stdio buf */
struct buf_stdio {
        struct buf s_buf;                           /* base buf */
        const char *s_p;                            /* next char to read */
        FILE       *s_fp;                           /* stdio stream */
        char       s_data[BUF_STDIO_DATA_SIZE + 1]; /* data array */
};

/**
 * initialize buf_stdio:
 *
 * args:
 *  @bsp: pointer to buf_stdio
 *  @fp:  stdio stream
 *
 * ret:
 *  none
 */
void buf_stdio_init(struct buf_stdio *bsp, FILE *fp);

/**
 * free buf_stdio:
 *
 * args:
 *  @bsp: pointer to buf_stdio
 *
 * ret:
 *  none
 */
void buf_stdio_free(struct buf_stdio *bsp);

#endif /* #ifndef BUF_STDIO_H */
