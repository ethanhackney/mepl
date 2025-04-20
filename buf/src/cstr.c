/**
 * buf_cstr{}
 * buf_cstr_init()
 * buf_cstr_free()
 */
#include <buf_cstr.h>

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
 * strlen()
 */
#include <string.h>

/**
 * bool{}
 */
#include <stdbool.h>

/**
 * get pointer to buf_cstr from pointer to c_buf:
 *
 * args:
 *  @_bp: pointer to buf
 *
 * ret:
 *  pointer to buf_cstr
 */
#define CSTR(_bp) \
        STRUCT_OF(_bp, struct buf_cstr, c_buf)

/**
 * get next character from buf_cstr:
 *
 * args:
 *  @bp: pointer to buf
 *
 * ret:
 *  character or BUF_EOF on end of file
 */
static int buf_cstr_getc(struct buf *bp);

/**
 * test if buf_cstr is empty:
 *
 * args:
 *  @bp: pointer to buf
 *
 * ret:
 *  true if empty
 *  false if not
 */
static bool buf_cstr_empty(const struct buf_cstr *bsp);

/**
 * validate buf_cstr:
 *
 * args:
 *  @bp:     pointer to buf
 *  @caller: who called us? (for error message)
 *
 * ret:
 *  none
 */
OK_DEF(buf_cstr_ok, const struct buf *bp,
        const struct buf_cstr *bsp = NULL;

        dbug(bp == NULL, "%s(): bp == NULL", caller);
        dbug(bp->b_getc != buf_cstr_getc,
             "%s(): b_getc != buf_cstr_getc",
             caller);

        bsp = CSTR(bp);
        dbug(bsp->c_str == NULL, "%s(): c_str == NULL");
        dbug(RANGE(bsp->c_str, bsp->c_str + strlen(bsp->c_str), bsp->c_p),
             "%s(): c_p is not pointing into c_data: %p", (void *)bsp->c_p,
             caller);
)

void
buf_cstr_init(struct buf_cstr *bsp, char **strpp)
{
        dbug(bsp == NULL, "buf_cstr_init(): bsp == NULL");
        dbug(strpp == NULL, "buf_cstr_init(): strpp == NULL");
        dbug(*strpp == NULL, "buf_cstr_init(): *strpp == NULL");

        bsp->c_str = *strpp;
        bsp->c_p = bsp->c_str;
        bsp->c_buf.b_getc = buf_cstr_getc;
        *strpp = NULL;
}

void
buf_cstr_free(struct buf_cstr *bsp, char **strpp)
{
        OK(buf_cstr_ok, &bsp->c_buf);

        if (strpp != NULL)
                *strpp = bsp->c_str;

        ZERO_P(bsp);
}

int
buf_cstr_getc(struct buf *bp)
{
        struct buf_cstr *bsp = NULL;
        int            c    = BUF_EOF;

        OK(buf_cstr_ok, bp);
        bsp = CSTR(bp);

        if (!buf_cstr_empty(bsp)) {
                c = *bsp->c_p;
                bsp->c_p++;
        }

        return c;
}

static bool
buf_cstr_empty(const struct buf_cstr *bsp)
{
        return *bsp->c_p == '\0';
}
