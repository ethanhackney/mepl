#ifndef AST_STR_H
#define AST_STR_H

#include "ast.h"
#include <stdbool.h>

struct ast_str {
        struct ast as_ast;
        char       *as_str;
};

struct ast_str *ast_str_new_move(char **spp);
struct ast_str *ast_str_new_copy(const char *s);

#endif /* #ifndef AST_STR_H */
