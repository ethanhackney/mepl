#include "ast.h"
#include "ast_str.h"
#include "lib.h"
#include "slab_tab.h"

static void exit_fn(void);

int
main(void)
{
        struct ast_str *sp = NULL;
        struct ast *ap = NULL;
        char s[] = "hello world";

        if (atexit(exit_fn) < 0)
                die("atexit()");

        slab_tab_init();

        sp = ast_str_new_copy(s);
        ap = &sp->as_ast;

        printf("%s\n", sp->as_str);
        ap->a_free(&ap);
        printf("%p\n", (void *)sp);
        printf("%p\n", (void *)ap);
        printf("%p\n", sp->as_str);

        sp = slab_tab_get("AST_WHILE");
}

static void
exit_fn(void)
{
        slab_tab_exit_check();
}
