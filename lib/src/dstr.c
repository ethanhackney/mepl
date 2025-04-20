/**
 * dstr{}
 * dstr_init()
 */
#include <dstr.h>

/**
 * RANGE()
 * ARRAY_ZERO()
 * ZERO_P()
 * ZALLOC()
 * PP_FREE()
 * die()
 * dbug()
 * OK_DEF()
 * OK()
 */
#include <lib.h>

/**
 * realloc()
 */
#include <stdlib.h>

/**
 * strcpy()
 * memset()
 */
#include <string.h>

/**
 * validate dstr:
 *
 * args:
 *  @dsp:    pointer to dstr
 *  @caller: who called us? (for error message)
 *
 * ret:
 *  none
 */
OK_DEF(dstr_ok, const struct dstr *dsp,
        dbug(dsp == NULL, "%s(): dsp == NULL", caller);
        dbug(dsp->d_cap == 0, "%s(): d_cap == 0", caller);
        dbug(dsp->d_len > dsp->d_cap, "%s(): d_len > d_cap", caller);
        dbug(dsp->d_cur == NULL, "%s(): d_cur == NULL", caller);
        dbug(dsp->d_cur[dsp->d_len] != '\0',
             "%s(): d_cur is not nil-terminated",
             caller);

        if (dsp->d_len <= DSTR_SHORT_SIZE) {
                dbug(dsp->d_cur != dsp->d_short,
                     "%s(): d_len <= DSTR_SHORT_SIZE and d_cur != d_short",
                     caller);
                dbug(dsp->d_heap != NULL,
                     "%s(): d_len <= DSTR_SHORT_SIZE and d_heap != NULL",
                     caller);
        } else {
                dbug(dsp->d_heap == NULL,
                     "%s(): d_len > DSTR_SHORT_SIZE && d_heap == NULL",
                     caller);
                dbug(dsp->d_cur != dsp->d_heap,
                     "%s(): d_len > DSTR_SHORT_SIZE and d_cur != d_heap",
                     caller);
        }
)

/**
 * grow dstr:
 *
 * args:
 *  @dsp: pointer to dstr
 *
 * ret:
 *  none
 *
 * NOTE:
 *  may invalidate pointers to d_cur
 */
static void dstr_grow(struct dstr *dsp);

void
dstr_init(struct dstr *dsp)
{
        dbug(dsp == NULL, "dstr_init(): dsp == NULL");

        dsp->d_heap = NULL;
        dsp->d_cur = dsp->d_short;
        dsp->d_len = 0;
        dsp->d_cap = DSTR_SHORT_SIZE;
        ARRAY_ZERO(dsp->d_short);
}

void
dstr_free(struct dstr *dsp)
{
        OK(dstr_ok, dsp);

        if (dsp->d_heap != NULL)
                PP_FREE(&dsp->d_heap);

        ZERO_P(dsp);
}

void
dstr_push(struct dstr *dsp, int c)
{
        OK(dstr_ok, dsp);

        if (dsp->d_len == dsp->d_cap)
                dstr_grow(dsp);

        dsp->d_cur[dsp->d_len] = c;
        dsp->d_len++;
        dsp->d_cur[dsp->d_len] = '\0';
}

static void
dstr_grow(struct dstr *dsp)
{
        size_t cap   = dsp->d_cap * 2;
        char   *heap = dsp->d_heap;

        heap = realloc(heap, cap + 1);
        if (heap == NULL)
                die("dstr_grow(): realloc()");

        if (dsp->d_cur == dsp->d_short)
                strcpy(heap, dsp->d_short);

        dsp->d_heap = heap;
        dsp->d_cur = dsp->d_heap;
        dsp->d_cap = cap;
}

void
dstr_clear(struct dstr *dsp)
{
        OK(dstr_ok, dsp);
        PP_FREE(&dsp->d_heap);
        dstr_init(dsp);
}

char *
dstr_move(struct dstr *dsp)
{
        char *p = NULL;

        OK(dstr_ok, dsp);

        dbug(dsp->d_cur == dsp->d_short,
             "dstr_move(): cannot move when d_cur points to d_short");

        p = dsp->d_heap;
        dstr_init(dsp);
        return p;
}
