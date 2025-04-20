/**
 * buf_nstr{}
 * buf_nstr_init()
 * buf_nstr_free()
 */
#include <buf_nstr.h>

/**
 * OK_DEF()
 * RANGE()
 * ARRAY_ZERO()
 * STRUCT_OF()
 * ZERO_P()
 * dbug()
 * OK()
 */
#include <lib.h>

/**
 * bool{}
 */
#include <stdbool.h>

/**
 * get pointer to buf_nstr from pointer to n_buf:
 *
 * args:
 *  @_bp: pointer to buf
 *
 * ret:
 *  pointer to buf_nstr
 */
#define NSTR(_bp) \
        STRUCT_OF(_bp, struct buf_nstr, n_buf)

/**
 * get next character from buf_nstr:
 *
 * args:
 *  @bp: pointer to buf
 *
 * ret:
 *  character or BUF_EOF on end of file
 */
static int buf_nstr_getc(struct buf *bp);

/**
 * test if buf_nstr is empty:
 *
 * args:
 *  @bp: pointer to buf
 *
 * ret:
 *  true if empty
 *  false if not
 */
static bool buf_nstr_empty(const struct buf_nstr *bnp);

/**
 * validate buf_nstr:
 *
 * args:
 *  @bp:     pointer to buf
 *  @caller: who called us? (for error message)
 *
 * ret:
 *  none
 */
OK_DEF(buf_nstr_ok, const struct buf *bp,
        const struct buf_nstr *bnp = NULL;

        dbug(bp == NULL, "%s(): bp == NULL", caller);
        dbug(bp->b_getc != buf_nstr_getc,
             "%s(): b_getc != buf_nstr_getc",
             caller);

        bnp = NSTR(bp);
        dbug(bnp->n_str == NULL, "%s(): n_str == NULL");
        dbug(RANGE(bnp->n_str, bnp->n_str + bnp->n_len, bnp->n_p),
             "%s(): n_p is not pointing into n_data: %p", (void *)bnp->n_p,
             caller);
)

void
buf_nstr_init(struct buf_nstr *bnp, char **strpp, size_t len)
{
        dbug(bnp == NULL, "buf_nstr_init(): bnp == NULL");
        dbug(strpp == NULL, "buf_nstr_init(): strpp == NULL");
        dbug(*strpp == NULL, "buf_nstr_init(): *strpp == NULL");

        bnp->n_str = *strpp;
        bnp->n_len = len;
        bnp->n_p = bnp->n_str;
        bnp->n_buf.b_getc = buf_nstr_getc;
        *strpp = NULL;
}

void
buf_nstr_free(struct buf_nstr *bnp, char **strpp)
{
        OK(buf_nstr_ok, &bnp->n_buf);

        if (strpp != NULL)
                *strpp = bnp->n_str;

        ZERO_P(bnp);
}

int
buf_nstr_getc(struct buf *bp)
{
        struct buf_nstr *bnp = NULL;
        int            c     = BUF_EOF;

        OK(buf_nstr_ok, bp);
        bnp = NSTR(bp);

        if (!buf_nstr_empty(bnp)) {
                c = *bnp->n_p;
                bnp->n_p++;
        }

        return c;
}

static bool
buf_nstr_empty(const struct buf_nstr *bnp)
{
        return bnp->n_p == bnp->n_str + bnp->n_len;
}
