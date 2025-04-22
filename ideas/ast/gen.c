#include "bin_op.h"
#include "dump.h"
#include "gen.h"
#include "lib.h"
#include "prim.h"

#define GEN(vp) \
        STRUCT_OF(vp, struct ast_gen_visitor, agv_v)

#define GEN_PRIM_FMT(pp, dst) do {              \
        char *p = dst;                          \
        p = stpcpy(p, "mov %s, ");              \
        p = stpcpy(p, AST_PRIM_CONV_SPEC(pp));  \
        *p++ = '\n';                            \
        *p = 0;                                 \
} while (0)

#define AST_PRIM_GEN(pp, dtype, r, fp) do {             \
        static char conv_spec[64];                      \
        GEN_PRIM_FMT(pp, conv_spec);                    \
        fprintf(fp, conv_spec, r, pp->ap_ ## dtype);    \
} while (0)

static const char *const AST_BIN_OP_CMD[AST_BIN_OP_COUNT] = {
        "add",
        "sub",
        "div",
        "mod",
        "mul",
};
#define AST_BIN_OP_GEN(op, r0, r1, fp) \
        fprintf(fp, "%s %s, %s\n", AST_BIN_OP_CMD[op->abo_type], r0, r1)

static void bin_op_gen(struct ast_visitor *vp, const struct ast_bin_op *op);
static void prim_gen(struct ast_visitor *vp, const struct ast_prim *pp);

void
ast_gen_visitor_init(struct ast_gen_visitor *gv, FILE *fp)
{
        reglist_init(&gv->agv_reg);
        gv->agv_v.av_bin_op = bin_op_gen;
        gv->agv_v.av_prim = prim_gen;
        gv->agv_fp = fp;
}

static void bin_op_gen(struct ast_visitor *vp, const struct ast_bin_op *op)
{
        struct ast_gen_visitor *gv = GEN(vp);
        struct reglist *rp = &gv->agv_reg;
        const struct ast *left = op->abo_left;
        const struct ast *right = op->abo_right;
        const char *src = NULL;
        const char *dst = NULL;

        left->a_visit(left, vp);
        right->a_visit(right, vp);

        src = reglist_last(rp);
        dst = reglist_get(rp);
        AST_BIN_OP_GEN(op, dst, src, gv->agv_fp);
        reglist_put(rp, src);
}

static void prim_gen(struct ast_visitor *vp, const struct ast_prim *pp)
{
        struct ast_gen_visitor *gv = GEN(vp);
        const char *dst = NULL;

        dst = reglist_get(&gv->agv_reg);
        AST_PRIM_SWITCH(pp, AST_PRIM_GEN, dst, gv->agv_fp);
}
