#ifndef AST_VAR_DEF_H
#define AST_VAR_DEF_H

#include "ast.h"

struct ast_var_def {
        struct ast avd_ast;
        struct ast *avd_init;
        char       *avd_dtype;
        char       *avd_name;
};

void ast_var_def_init(struct ast_var_def *dp, char **dtypepp, char **namepp);
void ast_var_def_free(struct ast_var_def *dp, char **dtypepp, char **namepp);

#endif
