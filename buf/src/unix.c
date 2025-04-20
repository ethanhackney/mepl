/**
 * buf_unix{}
 * buf_unix_init()
 * buf_unix_free()
 */
#include <buf_unix.h>

/**
 * RANGE()
 * ARRAY_ZERO()
 * STRUCT_OF()
 * OK_DEF()
 * dbug()
 * die()
 * OK()
 */
#include <lib.h>

/**
 * bool{}
 */
#include <stdbool.h>

/**
 * ssize_t{}
 */
#include <sys/types.h>

/**
 * read()
 */
#include <unistd.h>

/**
 * get pointer to buf_unix from pointer to u_buf:
 *
 * args:
 *  @_bp: pointer to buf
 *
 * ret:
 *  pointer to buf_unix
 */
#define UNIX(_bp) \
        STRUCT_OF(_bp, struct buf_unix, u_buf)

/**
 * get next character from buf_unix:
 *
 * args:
 *  @bp: pointer to buf
 *
 * ret:
 *  character or BUF_EOF on end of file
 */
static int buf_unix_getc(struct buf *bp);

/**
 * test if u_data is empty:
 *
 * args:
 *  @bup: pointer to buf_unix
 *
 * ret:
 *  true if u_data empty
 *  false if u_data has data
 */
static bool buf_unix_empty(const struct buf_unix *bup);

/**
 * fill u_data:
 *
 * args:
 *  @bup: pointer to buf_unix
 *
 * ret:
 *  true on success
 *  false on end of file
 */
static bool buf_unix_fill(struct buf_unix *bup);

/**
 * get next character from u_data:
 *
 * args:
 *  @bup: pointer to buf_unix
 *
 * ret:
 *  next character
 */
static int buf_unix_next(struct buf_unix *bup);

/**
 * fill u_data with unix call:
 *
 * args:
 *  @bup: pointer to buf_unix
 *
 * ret:
 *  true on success
 *  false on end of file
 */
static bool buf_unix_read(struct buf_unix *bup);

/**
 * validate buf_unix:
 *
 * args:
 *  @bp:     pointer to buf
 *  @caller: who called us? (for error message)
 *
 * ret:
 *  none
 */
OK_DEF(buf_unix_ok, const struct buf *bp,
        const struct buf_unix *bup = NULL;

        dbug(bp == NULL, "%s(): bp == NULL", caller);
        dbug(bp->b_getc != buf_unix_getc,
             "%s(): b_getc != buf_unix_getc",
             caller);

        bup = UNIX(bp);
        dbug(bup->u_fd < 0, "%s(): u_fd < 0", caller);
        dbug(RANGE(bup->u_data, bup->u_data + BUF_UNIX_DATA_SIZE, bup->u_p),
             "%s(): u_p is not pointing into u_data: %p", (void *)bup->u_p,
             caller);
)

void
buf_unix_init(struct buf_unix *bup, int fd)
{
        dbug(bup == NULL, "buf_unix_init(): bup == NULL");
        dbug(fd < 0, "buf_unix_init(): fd < 0");

        bup->u_fd = fd;
        bup->u_p = bup->u_data;
        ARRAY_ZERO(bup->u_data);
        bup->u_buf.b_getc = buf_unix_getc;
}

void
buf_unix_free(struct buf_unix *bup)
{
        OK(buf_unix_ok, &bup->u_buf);
        ZERO_P(bup);
}

static int
buf_unix_getc(struct buf *bp)
{
        struct buf_unix *bup = NULL;
        int              c   = BUF_EOF;

        OK(buf_unix_ok, bp);
        bup = UNIX(bp);

        if (!buf_unix_empty(bup))
                c = buf_unix_next(bup);
        else if (buf_unix_fill(bup))
                c = buf_unix_next(bup);

        return c;
}

static bool
buf_unix_empty(const struct buf_unix *bup)
{
        return *bup->u_p == '\0';
}

static bool
buf_unix_fill(struct buf_unix *bup)
{
        return buf_unix_read(bup);
}

static bool
buf_unix_read(struct buf_unix *bup)
{
        ssize_t n = read(bup->u_fd, bup->u_data, BUF_UNIX_DATA_SIZE);

        if (n < 0)
                die("buf_unix_read(): read()");

        bup->u_data[n] = '\0';
        bup->u_p = bup->u_data;
        return !buf_unix_empty(bup);
}

static int
buf_unix_next(struct buf_unix *bup)
{
        int c = *bup->u_p;
        bup->u_p++;
        return c;
}
