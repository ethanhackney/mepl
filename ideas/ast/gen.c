#include "bin_op.h"
#include "dump.h"
#include "gen.h"
#include "prim.h"
#include <err.h>
#include <errno.h>
#include <string.h>
#include <sysexits.h>

#define GEN(vp) \
        struct_of(vp, struct ast_gen_visitor, agv_v)

#define AST_BIN_OP_GEN(op, r0, r1, fp) \
        fprintf(fp, "%s %s, %s\n", AST_BIN_OP_CMD(op), r0, r1)

#define GEN_FMT(pp, dst) do {                   \
        char *p = dst;                          \
        p = stpcpy(p, "mov %s, ");              \
        p = stpcpy(p, AST_PRIM_CONV_SPEC(pp));  \
        *p++ = '\n';                            \
        *p = 0;                                 \
} while (0)

#define AST_PRIM_GEN(r, fp, pp, dtype) do {                                  \
        static char conv_spec[64];                                           \
        GEN_FMT(pp, conv_spec);                                              \
        fprintf(fp, conv_spec, r, AST_PRIM_CONV_SPEC(pp), pp->ap_ ## dtype); \
} while (0)

#define AST_PRIM_GEN_SWITCH_CASE(type, r, fp, pp, dtype)        \
        case type:                                              \
                AST_PRIM_GEN(r, fp, pp, dtype);                 \
                break;

#define AST_PRIM_GEN_SWITCH(pp, dst, fp)                                \
        switch (pp->ap_type) {                                          \
        AST_PRIM_GEN_SWITCH_CASE(AST_PRIM_INT,    dst, fp, pp, int)     \
        AST_PRIM_GEN_SWITCH_CASE(AST_PRIM_LONG,   dst, fp, pp, long)    \
        AST_PRIM_GEN_SWITCH_CASE(AST_PRIM_FLOAT,  dst, fp, pp, float)   \
        AST_PRIM_GEN_SWITCH_CASE(AST_PRIM_DOUBLE, dst, fp, pp, double)  \
        }

static const char *const AST_BIN_OP_CMDS[AST_BIN_OP_COUNT] = {
        "add",
        "sub",
        "div",
        "mod",
        "mul",
};
#define AST_BIN_OP_CMD(op) \
        AST_BIN_OP_CMDS[op->abo_type]

static void bin_op_gen(struct ast_visitor *vp, const struct ast_bin_op *op);
static void prim_gen(struct ast_visitor *vp, const struct ast_prim *pp);

void
ast_gen_visitor_init(struct ast_gen_visitor *gv, FILE *fp)
{
        gv->agv_v.av_bin_op = bin_op_gen;
        gv->agv_v.av_prim = prim_gen;
        gv->agv_fp = fp;
}

static const char *regstk[] = {
        "r0",
        "r1",
        "r2",
};
static const char **sp = regstk + ARRAY_SIZE(regstk);

static inline const char *
reg_get(void)
{
        if (sp == regstk) {
                errno = ENOMEM;
                err(EX_SOFTWARE, "reg_get: stack underflow");
        }
        return *--sp;
}

static inline void
reg_put(const char *reg)
{
        if (sp == regstk + ARRAY_SIZE(regstk)) {
                errno = EOVERFLOW;
                err(EX_SOFTWARE, "reg_put: stack overflow");
        }
        *sp++ = reg;
}

static inline const char *
reg_last(void)
{
        if (sp == regstk + ARRAY_SIZE(regstk)) {
                errno = EOVERFLOW;
                err(EX_SOFTWARE, "reg_last: no allocated registers");
        }
        return *(sp + 1);
}

static void bin_op_gen(struct ast_visitor *vp, const struct ast_bin_op *op)
{
        struct ast_gen_visitor *gv = GEN(vp);
        const struct ast *left = op->abo_left;
        const struct ast *right = op->abo_right;
        const char *src = NULL;
        const char *dst = NULL;

        left->a_visit(left, vp);
        right->a_visit(right, vp);

        src = reg_last();
        dst = reg_get();
        AST_BIN_OP_GEN(op, dst, src, gv->agv_fp);
        reg_put(src);
}

static void prim_gen(struct ast_visitor *vp, const struct ast_prim *pp)
{
        struct ast_gen_visitor *gv = GEN(vp);
        const char *dst = NULL;

        dst = reg_get();
        AST_PRIM_GEN_SWITCH(pp, dst, gv->agv_fp);
}
