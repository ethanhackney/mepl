#include "bin_op.h"
#include "dump.h"
#include "lib.h"
#include "prim.h"
#include "var_def.h"

#define DUMP(vp) \
        STRUCT_OF(vp, struct ast_dump_visitor, adv_v)

#define AST_PRIM_DUMP(pp, dtype, fp) \
        fprintf(fp, AST_PRIM_CONV_SPEC(pp), pp->ap_ ## dtype)

static void indent(struct ast_dump_visitor *dv, int space);

#define AST_DUMP_START(dv) do {         \
        indent(dv, dv->adv_space);      \
        fprintf(dv->adv_fp, "{\n");     \
} while (0)

static void indent(struct ast_dump_visitor *dv, int space)
{
        fprintf(stderr, "%*s", space, "");
}

#define AST_DUMP_END(dv) do {           \
        indent(dv, dv->adv_space);      \
        fprintf(dv->adv_fp, "},\n");    \
} while (0)

#define AST_DUMP_TYPE(dv, type) do {                    \
        indent(dv, dv->adv_space + 2);                  \
        fprintf(dv->adv_fp, ".type = %s{},\n", type);   \
} while (0)

#define AST_DUMP_FIELD(dv, fmt) do {    \
        indent(dv, dv->adv_space + 2);  \
        fprintf(dv->adv_fp, fmt);       \
} while (0)

static const char *const AST_BIN_OP_NAME[AST_BIN_OP_COUNT] = {
        "ast_add",
        "ast_sub",
        "ast_div",
        "ast_mod",
        "ast_mul",
};

static const char *const AST_PRIM_NAME[AST_PRIM_COUNT] = {
        "ast_int",
        "ast_long",
        "ast_float",
        "ast_double",
        "ast_char",
        "ast_cstr",
};

static void bin_op_dump(struct ast_visitor *vp, const struct ast_bin_op *op);
static void prim_dump(struct ast_visitor *vp, const struct ast_prim *pp);
static void var_def_dump(struct ast_visitor *vp, const struct ast_var_def *dp);
static void inc(struct ast_dump_visitor *dv, int amt);
static void dec(struct ast_dump_visitor *dv, int amt);

void
ast_dump_visitor_init(struct ast_dump_visitor *dv, FILE *fp, int start)
{
        dv->adv_v.av_bin_op = bin_op_dump;
        dv->adv_v.av_prim = prim_dump;
        dv->adv_v.av_var_def = var_def_dump;
        dv->adv_space = start;
        dv->adv_fp = fp;
}

static void
bin_op_dump(struct ast_visitor *vp, const struct ast_bin_op *op)
{
        struct ast_dump_visitor *dv = DUMP(vp);
        const struct ast *left = op->abo_left;
        const struct ast *right = op->abo_right;

        AST_DUMP_START(dv);
        AST_DUMP_TYPE(dv, AST_BIN_OP_NAME[op->abo_type]);

        AST_DUMP_FIELD(dv, ".left = \n");
        inc(dv, 4);
        left->a_visit(left, vp);
        dec(dv, 4);

        AST_DUMP_FIELD(dv, ".right = \n");
        inc(dv, 4);
        right->a_visit(right, vp);
        dec(dv, 4);

        AST_DUMP_END(dv);
}

static void
inc(struct ast_dump_visitor *dv, int amt)
{
        dv->adv_space += amt;
}

static void
dec(struct ast_dump_visitor *dv, int amt)
{
        dv->adv_space -= amt;
}

static void
prim_dump(struct ast_visitor *vp, const struct ast_prim *pp)
{
        struct ast_dump_visitor *dv = DUMP(vp);

        AST_DUMP_START(dv);
        AST_DUMP_TYPE(dv, AST_PRIM_NAME[pp->ap_type]);

        AST_DUMP_FIELD(dv, ".val = ");
        AST_PRIM_SWITCH(pp, AST_PRIM_DUMP, dv->adv_fp);
        printf(",\n");

        AST_DUMP_END(dv);
}

static void
var_def_dump(struct ast_visitor *vp, const struct ast_var_def *dp)
{
        struct ast_dump_visitor *dv = DUMP(vp);
        const struct ast *expr = dp->avd_init;

        AST_DUMP_START(dv);
        AST_DUMP_TYPE(dv, "ast_var_def");

        indent(dv, dv->adv_space + 2);
        fprintf(dv->adv_fp, ".dtype = %s\n", dp->avd_dtype);

        indent(dv, dv->adv_space + 2);
        fprintf(dv->adv_fp, ".name = %s\n", dp->avd_name);

        if (expr != NULL) {
                inc(dv, 4);
                expr->a_visit(expr, vp);
                dec(dv, 4);
        }

        AST_DUMP_END(dv);
}
