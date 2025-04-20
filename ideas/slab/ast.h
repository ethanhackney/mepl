#ifndef AST_H
#define AST_H

#include <stddef.h>

struct ast {
        void (*a_free)(struct ast **app);
};

#ifndef AST_VAR_DEF_SLAB_SIZE
#define AST_VAR_DEF_SLAB_SIZE (1 << 8)
#endif /* #ifndef AST_VAR_DEF_SLAB_SIZE */
struct ast_var_def {
        struct ast avd_ast;
        struct ast *avd_expr;
        char       *avd_type;
        char       *avd_id;
};

#ifndef AST_IF_SLAB_SIZE
#define AST_IF_SLAB_SIZE (1 << 8)
#endif /* #ifndef AST_IF_SLAB_SIZE */
struct ast_if {
        struct ast ai_ast;
        struct ast *ai_cond;
        struct ast **ai_stmts;
        size_t     ai_stmts_cap;
        size_t     ai_stmts_len;
};

#endif /* #ifndef AST_H */
