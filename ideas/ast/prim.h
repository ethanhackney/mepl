#ifndef AST_PRIM_H
#define AST_PRIM_H

#include "ast.h"
#include "lib.h"

#define prim(ap) \
        struct_of(ap, struct ast_prim, ap_ast)

#define AST_PRIM_INIT(type, ufield, val)                \
        {                                               \
                .ap_type       = type,                  \
                .ap_ ## ufield = val,                   \
                .ap_ast        = {                      \
                        .a_visit = ast_prim_visit,      \
                },                                      \
        }

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

static const char *const AST_PRIM_CONV_SPECS[AST_PRIM_COUNT] = {
        "%d",
        "%ld",
        "%f",
        "%f",
};
#define AST_PRIM_CONV_SPEC(pp)  \
        AST_PRIM_CONV_SPECS[pp->ap_type]

#define AST_PRIM_INIT_PROTO(name, dtype)   \
        void ast_prim_init_ ## name(struct ast_prim *pp, int type, dtype val)

AST_PRIM_INIT_PROTO(int, int);
AST_PRIM_INIT_PROTO(long, long);
AST_PRIM_INIT_PROTO(float, float);
AST_PRIM_INIT_PROTO(double, double);

#endif
