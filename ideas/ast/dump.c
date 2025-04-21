#include "bin_op.h"
#include "dump.h"
#include "prim.h"
#include <stdio.h>

#define DUMP(vp) \
        struct_of(vp, struct ast_dump_visitor, adv_v)

#define AST_PRIM_DUMP_SWITCH_CASE(type, pp, dtype)                      \
        case type:                                                      \
                printf(AST_PRIM_CONV_SPEC(pp), pp->ap_ ## dtype);       \
                break;

#define AST_PRIM_CONV_SPEC(pp) \
        AST_PRIM_CONV_SPECS[pp->ap_type]

#define AST_PRIM_DUMP_SWITCH(pp) do {                           \
        switch (pp->ap_type) {                                  \
        AST_PRIM_DUMP_SWITCH_CASE(AST_PRIM_INT,    pp, int)     \
        AST_PRIM_DUMP_SWITCH_CASE(AST_PRIM_LONG,   pp, long)    \
        AST_PRIM_DUMP_SWITCH_CASE(AST_PRIM_FLOAT,  pp, float)   \
        AST_PRIM_DUMP_SWITCH_CASE(AST_PRIM_DOUBLE, pp, double)  \
        }                                                       \
} while (0)

static void indent(int space);

#define AST_DUMP_START(dv) do { \
        indent(dv->adv_space);  \
        printf("{\n");          \
} while (0)

static void indent(int space)
{
        while (space--)
                putchar(' ');
}

#define AST_DUMP_END(dv) do {   \
        indent(dv->adv_space);  \
        printf("},\n");         \
} while (0)

#define AST_DUMP_TYPE(dv, type) do {            \
        indent(dv->adv_space + 2);              \
        printf(".type = %s{},\n", type);        \
} while (0)

#define AST_DUMP_FIELD(dv, fmt) do {    \
        indent(dv->adv_space + 2);      \
        printf(fmt);                    \
} while (0)

static const char *const AST_BIN_OP_NAMES[AST_BIN_OP_COUNT] = {
        "ast_add",
        "ast_sub",
        "ast_div",
        "ast_mod",
        "ast_mul",
};
#define AST_BIN_OP_NAME(op) \
        AST_BIN_OP_NAMES[op->abo_type]

static const char *const AST_PRIM_NAMES[AST_PRIM_COUNT] = {
        "ast_int",
        "ast_long",
        "ast_float",
        "ast_double",
};
#define AST_PRIM_NAME(pp) \
        AST_PRIM_NAMES[pp->ap_type]

static void bin_op_dump(struct ast_visitor *vp, const struct ast_bin_op *op);
static void prim_dump(struct ast_visitor *vp, const struct ast_prim *pp);
static void inc(struct ast_dump_visitor *dv, int amt);
static void dec(struct ast_dump_visitor *dv, int amt);

void
ast_dump_visitor_init(struct ast_dump_visitor *dv, int start)
{
        dv->adv_v.av_bin_op = bin_op_dump;
        dv->adv_v.av_prim = prim_dump;
        dv->adv_space = start;
}

static void
bin_op_dump(struct ast_visitor *vp, const struct ast_bin_op *op)
{
        struct ast_dump_visitor *dv = DUMP(vp);
        const struct ast *left = op->abo_left;
        const struct ast *right = op->abo_right;

        AST_DUMP_START(dv);
        AST_DUMP_TYPE(dv, AST_BIN_OP_NAME(op));

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
        AST_DUMP_TYPE(dv, AST_PRIM_NAME(pp));

        AST_DUMP_FIELD(dv, ".val = ");
        AST_PRIM_DUMP_SWITCH(pp);
        printf(",\n");

        AST_DUMP_END(dv);
}
