#ifndef BUF_UNIX_H
#define BUF_UNIX_H

/**
 * buf{}
 * BUF_EOF
 */
#include <buf.h>

#ifndef BUF_UNIX_DATA_SIZE
#define BUF_UNIX_DATA_SIZE ((1 << 13) - 1) /* buf_unix data size */
#endif /* #ifndef BUF_UNIX_DATA_SIZE */

/* unix buf */
struct buf_unix {
        struct buf u_buf;                          /* base buf */
        const char *u_p;                           /* next char to read */
        char       u_data[BUF_UNIX_DATA_SIZE + 1]; /* data array */
        int        u_fd;                           /* file descriptor */
};

/**
 * initialize buf_unix:
 *
 * args:
 *  @bup: pointer to buf_unix
 *  @fd:  file descriptor
 *
 * ret:
 *  none
 */
void buf_unix_init(struct buf_unix *bup, int fd);

/**
 * free buf_unix:
 *
 * args:
 *  @bup: pointer to buf_unix
 *
 * ret:
 *  none
 */
void buf_unix_free(struct buf_unix *bup);

#endif /* #ifndef BUF_UNIX_H */
