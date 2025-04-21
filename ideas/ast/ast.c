#include <stddef.h>
#include <stdio.h>

#define struct_of(p, type, field) \
        ((type *)((char *)(p) - offsetof(type, field)))

struct ast {
        void (*a_dump)(const struct ast *ap, int space);
};

enum {
        AST_BIN_OP_ADD,
        AST_BIN_OP_SUB,
        AST_BIN_OP_DIV,
        AST_BIN_OP_MOD,
        AST_BIN_OP_MUL,
        AST_BIN_OP_COUNT,
};

struct ast_bin_op {
        struct ast abo_ast;
        struct ast *abo_left;
        struct ast *abo_right;
        int        abo_type;
};

#define bin_op(ap) \
        struct_of(ap, struct ast_bin_op, abo_ast)

#define AST_BIN_OP_INIT(type, left, right)              \
        {                                               \
                .abo_left  = left,                      \
                .abo_right = right,                     \
                .abo_ast   = {                          \
                        .a_dump = ast_bin_op_dump,      \
                },                                      \
        }

#define AST_BIN_OP_NAME(op) \
        AST_BIN_OP_NAMES[op->abo_type]

static const char *const AST_BIN_OP_NAMES[AST_BIN_OP_COUNT] = {
        "ast_add",
        "ast_sub",
        "ast_div",
        "ast_mod",
        "ast_mul",
};

enum {
        AST_PRIM_INT,
        AST_PRIM_LONG,
        AST_PRIM_FLOAT,
        AST_PRIM_DOUBLE,
        AST_PRIM_COUNT,
};

struct ast_prim {
        struct ast ap_ast;
        union {
                double ap_double;
                float  ap_float;
                long   ap_long;
                int    ap_int;
        };
        int        ap_type;
};

#define prim(ap) \
        struct_of(ap, struct ast_prim, ap_ast)

#define AST_PRIM_INIT(type, ufield, val)                \
        {                                               \
                .ap_type       = type,                  \
                .ap_ ## ufield = val,                   \
                .ap_ast        = {                      \
                        .a_dump = ast_prim_dump,        \
                },                                      \
        }

#define AST_PRIM_DUMP_SWITCH_CASE(type, fmt, val)       \
        case type:                                      \
                printf(fmt, val);                       \
                break;

#define AST_PRIM_DUMP_SWITCH(pp) do {                                    \
        switch (pp->ap_type) {                                           \
        AST_PRIM_DUMP_SWITCH_CASE(AST_PRIM_INT,    "%d",  pp->ap_int)    \
        AST_PRIM_DUMP_SWITCH_CASE(AST_PRIM_LONG,   "%ld", pp->ap_long)   \
        AST_PRIM_DUMP_SWITCH_CASE(AST_PRIM_FLOAT,  "%f",  pp->ap_float)  \
        AST_PRIM_DUMP_SWITCH_CASE(AST_PRIM_DOUBLE, "%f",  pp->ap_double) \
        }                                                                \
} while (0)

#define AST_PRIM_NAME(pp) \
        AST_PRIM_NAMES[pp->ap_type]

static const char *const AST_PRIM_NAMES[AST_PRIM_COUNT] = {
        "ast_int",
        "ast_long",
        "ast_float",
        "ast_double",
};

#define AST_DUMP_START(space) do {      \
        indent(space);                  \
        printf("{\n");                  \
} while (0)

#define AST_DUMP_END(space) do {        \
        indent(space);                  \
        printf("},\n");                 \
} while (0)

#define AST_DUMP_TYPE(space, type) do {         \
        indent(space + 2);                      \
        printf(".type = %s{},\n", type);        \
} while (0)

#define AST_DUMP_FIELD(fmt) do {        \
        indent(space + 2);              \
        printf(fmt);                    \
} while (0)

static void indent(int space)
{
        while (space--)
                putchar(' ');
}

static void ast_bin_op_dump(const struct ast *ap, int space)
{
        const struct ast_bin_op *op = bin_op(ap);
        const struct ast *left = op->abo_left;
        const struct ast *right = op->abo_right;

        AST_DUMP_START(space);
        AST_DUMP_TYPE(space, AST_BIN_OP_NAME(op));

        AST_DUMP_FIELD(".left = \n");
        left->a_dump(left, space + 4);

        AST_DUMP_FIELD(".right = \n");
        right->a_dump(right, space + 4);

        AST_DUMP_END(space);
}

static void ast_prim_dump(const struct ast *ap, int space)
{
        const struct ast_prim *pp = prim(ap);

        AST_DUMP_START(space);
        AST_DUMP_TYPE(space, AST_PRIM_NAME(pp));

        AST_DUMP_FIELD(".val = ");
        AST_PRIM_DUMP_SWITCH(pp);
        printf(",\n");

        AST_DUMP_END(space);
}

int
main(void)
{
        struct ast_prim p = AST_PRIM_INIT(AST_PRIM_FLOAT, float, 123.456);
        struct ast_prim p2 = AST_PRIM_INIT(AST_PRIM_INT, int, 5);
        struct ast_bin_op op = AST_BIN_OP_INIT(AST_BIN_OP_TYPE,
                        &p.ap_ast, &p2.ap_ast);
        struct ast *ap = &op.abo_ast;

        ap->a_dump(ap, 0);
}
