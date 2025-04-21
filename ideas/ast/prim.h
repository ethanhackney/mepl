#ifndef AST_PRIM_H
#define AST_PRIM_H

#include "ast.h"

#define AST_PRIM_SWITCH_CASE(type, pp, dtype, func, ...)        \
        case type:                                              \
                func(pp, dtype, __VA_ARGS__);                   \
                break;

#define AST_PRIM_SWITCH(pp, func, ...) do {                                    \
        switch (pp->ap_type) {                                                 \
        AST_PRIM_SWITCH_CASE(AST_PRIM_INT,    pp, int,    func, __VA_ARGS__)   \
        AST_PRIM_SWITCH_CASE(AST_PRIM_LONG,   pp, long,   func, __VA_ARGS__)   \
        AST_PRIM_SWITCH_CASE(AST_PRIM_FLOAT,  pp, float,  func, __VA_ARGS__)   \
        AST_PRIM_SWITCH_CASE(AST_PRIM_DOUBLE, pp, double, func, __VA_ARGS__)   \
        AST_PRIM_SWITCH_CASE(AST_PRIM_CHAR,   pp, char,   func, __VA_ARGS__)   \
        AST_PRIM_SWITCH_CASE(AST_PRIM_CSTR,   pp, cstr,   func, __VA_ARGS__)   \
        }                                                                      \
} while (0)

enum {
        AST_PRIM_INT,
        AST_PRIM_LONG,
        AST_PRIM_FLOAT,
        AST_PRIM_DOUBLE,
        AST_PRIM_CHAR,
        AST_PRIM_CSTR,
        AST_PRIM_COUNT,
};

struct ast_prim {
        struct ast ap_ast;
        union {
                char   *ap_cstr;
                double ap_double;
                float  ap_float;
                long   ap_long;
                char   ap_char;
                int    ap_int;
        };
        int        ap_type;
};

static const char *const AST_PRIM_CONV_SPECS[AST_PRIM_COUNT] = {
        "%d",
        "%ld",
        "%f",
        "%f",
        "%c",
        "\"%s\"",
};
#define AST_PRIM_CONV_SPEC(pp)  \
        AST_PRIM_CONV_SPECS[pp->ap_type]

#define AST_PRIM_INIT_PROTO(name, dtype)   \
        void ast_prim_init_ ## name(struct ast_prim *pp, int type, dtype val)

AST_PRIM_INIT_PROTO(int, int);
AST_PRIM_INIT_PROTO(long, long);
AST_PRIM_INIT_PROTO(float, float);
AST_PRIM_INIT_PROTO(double, double);
AST_PRIM_INIT_PROTO(char, char);
AST_PRIM_INIT_PROTO(cstr, char *);

#endif
