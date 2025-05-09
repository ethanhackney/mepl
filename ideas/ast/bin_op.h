#ifndef AST_BIN_OP_H
#define AST_BIN_OP_H

#include "ast.h"

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

void ast_bin_op_init(struct ast_bin_op *op,
                     int type,
                     struct ast *left,
                     struct ast *right);

#endif
