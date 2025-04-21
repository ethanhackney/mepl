#include "lib.h"
#include "prim.h"

#define PRIM(ap) \
        struct_of(ap, struct ast_prim, ap_ast)

static void prim_visit(const struct ast *ap, struct ast_visitor *vp);
static void prim_init(struct ast_prim *pp, int type);

#define AST_PRIM_INIT_DEF(name, dtype)                                  \
                                                                        \
void                                                                    \
ast_prim_init_ ## name(struct ast_prim *pp, int type, dtype val)        \
{                                                                       \
        prim_init(pp, type);                                            \
        pp->ap_ ## name = val;                                          \
}

AST_PRIM_INIT_DEF(int, int)
AST_PRIM_INIT_DEF(long, long)
AST_PRIM_INIT_DEF(float, float)
AST_PRIM_INIT_DEF(double, double)
AST_PRIM_INIT_DEF(char, char)

static void
prim_init(struct ast_prim *pp, int type)
{
        pp->ap_ast.a_visit = prim_visit;
        pp->ap_type = type;
}

static void
prim_visit(const struct ast *ap, struct ast_visitor *vp)
{
        const struct ast_prim *pp = PRIM(ap);

        vp->av_prim(vp, pp);
}
