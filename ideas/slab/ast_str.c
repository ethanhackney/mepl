#include "ast_str.h"
#include "lib.h"
#include "slab_tab.h"

static void ast_str_free(struct ast **app);

struct ast_str *ast_str_new_move(char **spp);
struct ast_str *ast_str_new_copy(const char *sp);

struct ast_str *
ast_str_move(char **spp)
{
        struct ast_str *sp = NULL;

        sp = slab_tab_get("AST_STR");
        sp->as_str = *spp;
        *spp = NULL;
        sp->as_ast.a_free = ast_str_free;

        return sp;
}

struct ast_str *
ast_str_new_copy(const char *s)
{
        struct ast_str *sp = NULL;

        sp = slab_tab_get("AST_STR");
        sp->as_str = strdup(s);
        if (sp->as_str == NULL)
                die("ast_str_new_copy(): strdup()");

        sp->as_ast.a_free = ast_str_free;
        return sp;
}

static void
ast_str_free(struct ast **app)
{
        struct ast_str *sp = STRUCT_OF(*app, struct ast_str, as_ast);

        free(sp->as_str);
        memset(sp, 0, sizeof(*sp));
        slab_tab_put("AST_STR", (void **)app);
}
