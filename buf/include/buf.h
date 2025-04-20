#ifndef BUF_H
#define BUF_H

#ifndef BUF_EOF
#define BUF_EOF -1 /* end of file marker */
#endif /* #ifndef BUF_EOF */

/* base buf */
struct buf {
        /**
         * get next character from buf:
         *
         * args:
         *  @bp: pointer to buf
         *
         * ret:
         *  character or BUF_EOF on end of file
         *
         * NOTE:
         *  may exit process
         */
        int (*b_getc)(struct buf *bp);
};

#endif /* #ifndef BUF_H */
