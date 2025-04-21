#include <err.h>
#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <sysexits.h>

#define ARRAY_SIZE(arr) \
        (sizeof(arr) / sizeof(*arr))

#define struct_of(p, type, field) \
        ((type *)((char *)(p) - offsetof(type, field)))

struct ast {
        void (*a_dump)(const struct ast *ap, int space);
        void (*a_gen)(const struct ast *ap, FILE *fp);
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
                        .a_gen  = ast_bin_op_gen,       \
                },                                      \
        }

#define AST_BIN_OP_CMD(op) \
        AST_BIN_OP_CMD[op->abo_type]

#define AST_BIN_OP_NAME(op) \
        AST_BIN_OP_NAMES[op->abo_type]

#define AST_BIN_OP_GEN(op, r0, r1, fp) \
        fprintf(fp, "%s %s, %s\n", AST_BIN_OP_CMD(op), r0, r1)

static const char *const AST_BIN_OP_NAMES[AST_BIN_OP_COUNT] = {
        "ast_add",
        "ast_sub",
        "ast_div",
        "ast_mod",
        "ast_mul",
};

static const char *const AST_BIN_OP_CMD[AST_BIN_OP_COUNT] = {
        "add",
        "sub",
        "div",
        "mod",
        "mul",
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

#define CONV_SPEC_SIZE 64

#define GEN_FMT(pp, dst) do {                   \
        char *p = dst;                          \
        p = stpcpy(p, "mov %s, ");              \
        p = stpcpy(p, AST_PRIM_CONV_SPEC(pp));  \
        *p++ = '\n';                            \
        *p = 0;                                 \
} while (0)

#define AST_PRIM_GEN(r, fp, pp, dtype) do {                                  \
        static char conv_spec[CONV_SPEC_SIZE];                               \
        GEN_FMT(pp, conv_spec);                                              \
        fprintf(fp, conv_spec, r, AST_PRIM_CONV_SPEC(pp), pp->ap_ ## dtype); \
} while (0)

#define AST_PRIM_GEN_SWITCH_CASE(type, r, fp, pp, dtype)        \
        case type:                                              \
                AST_PRIM_GEN(r, fp, pp, dtype);                 \
                break;

#define AST_PRIM_GEN_SWITCH(pp, r, fp)                                  \
        switch (pp->ap_type) {                                          \
        AST_PRIM_GEN_SWITCH_CASE(AST_PRIM_INT,    r, fp, pp, int)       \
        AST_PRIM_GEN_SWITCH_CASE(AST_PRIM_LONG,   r, fp, pp, long)      \
        AST_PRIM_GEN_SWITCH_CASE(AST_PRIM_FLOAT,  r, fp, pp, float)     \
        AST_PRIM_GEN_SWITCH_CASE(AST_PRIM_DOUBLE, r, fp, pp, int)       \
        }

#define AST_PRIM_INIT(type, ufield, val)                \
        {                                               \
                .ap_type       = type,                  \
                .ap_ ## ufield = val,                   \
                .ap_ast        = {                      \
                        .a_dump = ast_prim_dump,        \
                        .a_gen  = ast_prim_gen,         \
                },                                      \
        }

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

#define AST_PRIM_NAME(pp) \
        AST_PRIM_NAMES[pp->ap_type]

static const char *const AST_PRIM_NAMES[AST_PRIM_COUNT] = {
        "ast_int",
        "ast_long",
        "ast_float",
        "ast_double",
};

static const char *const AST_PRIM_CONV_SPECS[AST_PRIM_COUNT] = {
        "%d",
        "%ld",
        "%f",
        "%f",
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

static void ast_bin_op_gen(const struct ast *ap, FILE *fp)
{
        const struct ast_bin_op *op = bin_op(ap);
        const struct ast *left = op->abo_left;
        const struct ast *right = op->abo_right;
        const char *src = NULL;
        const char *dst = NULL;

        left->a_gen(left, fp);
        right->a_gen(right, fp);

        src = reg_last();
        dst = reg_get();
        AST_BIN_OP_GEN(op, dst, src, fp);
        reg_put(src);
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

static void ast_prim_gen(const struct ast *ap, FILE *fp)
{
        const struct ast_prim *pp = prim(ap);
        const char *dst = NULL;

        dst = reg_get();
        AST_PRIM_GEN_SWITCH(pp, dst, fp);
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
        ap->a_gen(ap, stdout);
}
