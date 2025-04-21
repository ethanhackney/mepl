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
        AST_PRIM_COUNT,
};

struct ast_prim {
        struct ast ap_ast;
        union {
                float ap_float;
                long  ap_long;
                int   ap_int;
        };
        int        ap_type;
};

#define AST_PRIM_DUMP_SWITCH_DEF(type, fmt, val)        \
        case type:                                      \
                printf(fmt, val);                       \
                break;

static const char *const AST_PRIM_NAMES[AST_PRIM_COUNT] = {
        "ast_int{}",
        "ast_long{}",
        "ast_float{}",
};

static void indent(int space)
{
        while (space--)
                putchar(' ');
}

static void ast_bin_op_dump(const struct ast *ap, int space)
{
        struct ast_bin_op *op = struct_of(ap, struct ast_bin_op, abo_ast);
        struct ast *left = op->abo_left;
        struct ast *right = op->abo_right;

        indent(space);
        printf("{\n");

        indent(space + 2);
        printf(".type = %s{},\n", AST_BIN_OP_NAMES[op->abo_type]);

        printf(".left = ");
        left->a_dump(left, space + 2);

        printf(".right = ");
        left->a_dump(left, space + 2);

        indent(space);
        printf("},\n");
}

static void ast_prim_dump(const struct ast *ap, int space)
{
        struct ast_prim *pp = struct_of(ap, struct ast_prim, ap_ast);
        int type = pp->ap_type;

        indent(space);
        printf("{\n");

        indent(space + 2);
        printf(".type = %s{},\n", AST_PRIM_NAMES[type]);

        indent(space + 2);
        printf(".val  = ");
        switch (type) {
                AST_PRIM_DUMP_SWITCH_DEF(AST_PRIM_INT,   "%d",  pp->ap_int)
                AST_PRIM_DUMP_SWITCH_DEF(AST_PRIM_LONG,  "%ld", pp->ap_long)
                AST_PRIM_DUMP_SWITCH_DEF(AST_PRIM_FLOAT, "%f",  pp->ap_float);
        }
        printf(",\n");

        indent(space);
        printf("},\n");
}

int
main(void)
{
        struct ast_prim p = {
                .ap_ast = {
                        .a_dump = ast_prim_dump,
                },
                .ap_float = 123.456,
                .ap_type = AST_PRIM_FLOAT,
        };
        struct ast *ap = &p.ap_ast;

        ap->a_dump(ap, 0);
        p.ap_long = 123;
        p.ap_type = AST_PRIM_LONG;
        ap->a_dump(ap, 0);
}
