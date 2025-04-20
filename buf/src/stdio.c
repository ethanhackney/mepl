/**
 * buf_stdio{}
 * buf_stdio_init()
 * buf_stdio_free()
 */
#include <buf_stdio.h>

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
 * get pointer to buf_stdio from pointer to s_buf:
 *
 * args:
 *  @_bp: pointer to buf
 *
 * ret:
 *  pointer to buf_stdio
 */
#define STDIO(_bp) \
        STRUCT_OF(_bp, struct buf_stdio, s_buf)

/**
 * get next character from buf_stdio:
 *
 * args:
 *  @bp: pointer to buf
 *
 * ret:
 *  character or BUF_EOF on end of file
 */
static int buf_stdio_getc(struct buf *bp);

/**
 * test if s_data is empty:
 *
 * args:
 *  @bsp: pointer to buf_stdio
 *
 * ret:
 *  true if s_data empty
 *  false if s_data has data
 */
static bool buf_stdio_empty(const struct buf_stdio *bsp);

/**
 * fill s_data:
 *
 * args:
 *  @bsp: pointer to buf_stdio
 *
 * ret:
 *  true on success
 *  false on end of file
 */
static bool buf_stdio_fill(struct buf_stdio *bsp);

/**
 * get next character from s_data:
 *
 * args:
 *  @bsp: pointer to buf_stdio
 *
 * ret:
 *  next character
 */
static int buf_stdio_next(struct buf_stdio *bsp);

/**
 * fill s_data with stdio call:
 *
 * args:
 *  @bsp: pointer to buf_stdio
 *
 * ret:
 *  true on success
 *  false on end of file
 */
static bool buf_stdio_read(struct buf_stdio *bsp);

/**
 * validate buf_stdio:
 *
 * args:
 *  @bp:     pointer to buf
 *  @caller: who called us? (for error message)
 *
 * ret:
 *  none
 */
OK_DEF(buf_stdio_ok, const struct buf *bp,
        const struct buf_stdio *bsp = NULL;

        dbug(bp == NULL, "%s(): bp == NULL", caller);
        dbug(bp->b_getc != buf_stdio_getc,
             "%s(): b_getc != buf_stdio_getc",
             caller);

        bsp = STDIO(bp);
        dbug(bsp->s_fp == NULL, "%s(): s_fp == NULL", caller);
        dbug(RANGE(bsp->s_data, bsp->s_data + BUF_STDIO_DATA_SIZE, bsp->s_p),
             "%s(): s_p is not pointing into s_data: %p", (void *)bsp->s_p,
             caller);
)

void
buf_stdio_init(struct buf_stdio *bsp, FILE *fp)
{
        dbug(bsp == NULL, "buf_stdio_init(): bsp == NULL");
        dbug(fp == NULL, "buf_stdio_init(): fp == NULL");

        bsp->s_fp = fp;
        bsp->s_p = bsp->s_data;
        ARRAY_ZERO(bsp->s_data);
        bsp->s_buf.b_getc = buf_stdio_getc;
}

void
buf_stdio_free(struct buf_stdio *bsp)
{
        OK(buf_stdio_ok, &bsp->s_buf);
        ZERO_P(bsp);
}

static int
buf_stdio_getc(struct buf *bp)
{
        struct buf_stdio *bsp = NULL;
        int              c    = BUF_EOF;

        OK(buf_stdio_ok, bp);
        bsp = STDIO(bp);

        if (!buf_stdio_empty(bsp))
                c = buf_stdio_next(bsp);
        else if (buf_stdio_fill(bsp))
                c = buf_stdio_next(bsp);

        return c;
}

static bool
buf_stdio_empty(const struct buf_stdio *bsp)
{
        return *bsp->s_p == '\0';
}

static bool
buf_stdio_fill(struct buf_stdio *bsp)
{
        if (!buf_stdio_read(bsp))
                return false;

        bsp->s_data[BUF_STDIO_DATA_SIZE] = '\0';
        bsp->s_p = bsp->s_data;
        return true;
}

static bool
buf_stdio_read(struct buf_stdio *bsp)
{
        const char *p = fgets(bsp->s_data, BUF_STDIO_DATA_SIZE, bsp->s_fp);

        if (ferror(bsp->s_fp))
                die("buf_stdio_read(): fgets()");

        return p != NULL;
}

static int
buf_stdio_next(struct buf_stdio *bsp)
{
        int c = *bsp->s_p;
        bsp->s_p++;
        return c;
}
