#include "base.h"
#include "bitset.h"
#include "lib.h"
#include "list.h"
#include "ring.h"
#include "slab.h"

BITSET_DEF(static, myset)
RING_DEF(static, int, intbuf)

struct ast {
        void (*a_dump)(const struct ast *ap, int space);
};

struct ast_var_def {
        struct ast  avd_ast;
        struct ast *avd_init;
        char       *avd_dtype;
        char       *avd_name;
};

struct ast_str {
        struct ast  as_ast;
        char       *as_str;
};

SLAB_DEF(static, struct ast_var_def, _var_def_slab)
struct var_def_slab {
        struct _var_def_slab vds_slab;
        struct slab_base     vds_base;
};

static struct ptr *var_def_slab_get(struct slab_base *bp);
static void        var_def_slab_put(struct slab_base *bp, struct ptr **pp);
static void        var_def_slab_free(struct slab_base *bp);
static void        var_def_slab_init(struct var_def_slab *vdp,
                                     size_t arr_size,
                                     size_t ring_size);

int
main(void)
{
        struct var_def_slab vds;
        struct ast_var_def *vdp;
        struct slab_base *bp;
        struct ptr *ptr;

        var_def_slab_init(&vds, (1 << 8), (1 << 8));
        bp = &vds.vds_base;
        ptr = bp->sb_get(bp);
        vdp = ptr->p_ptr;
        vdp->avd_name = "ethan";
        puts(vdp->avd_name);
        bp->sb_put(bp, &ptr);
        bp->sb_free(bp);
        /*
        struct ast_var_def *p;
        struct ptr *ptr;
        struct _var_def_slab vds;

        _var_def_slab_init(&vds, (1 << 8), (1 << 8));

        ptr = _var_def_slab_get(&vds);
        p = ptr->p_ptr;
        p->avd_name = "ethan";
        printf("%s\n", p->avd_name);

        _var_def_slab_put(&vds, &ptr);
        _var_def_slab_free(&vds);
        */
}

static struct ptr *
var_def_slab_get(struct slab_base *bp)
{
        struct var_def_slab *vdp = ctor(bp, struct var_def_slab, vds_base);
        return _var_def_slab_get(&vdp->vds_slab);
}

static void
var_def_slab_put(struct slab_base *bp, struct ptr **pp)
{
        struct var_def_slab *vdp = ctor(bp, struct var_def_slab, vds_base);
        _var_def_slab_put(&vdp->vds_slab, pp);
}

static void
var_def_slab_init(struct var_def_slab *vdp, size_t arr_size, size_t ring_size)
{
        _var_def_slab_init(&vdp->vds_slab, arr_size, ring_size);
        vdp->vds_base.sb_get = var_def_slab_get;
        vdp->vds_base.sb_put = var_def_slab_put;
        vdp->vds_base.sb_free = var_def_slab_free;
}

static void
var_def_slab_free(struct slab_base *bp)
{
        struct var_def_slab *vdp = ctor(bp, struct var_def_slab, vds_base);
        _var_def_slab_free(&vdp->vds_slab);
}
