#include "ast.h"
#include "bin_op.h"
#include "dump.h"
#include "gen.h"
#include "lib.h"
#include "prim.h"

#define fn(a, b, fp, c) \
        fprintf(fp, "%d, %d, %d\n", a, b, c)

#define vfn(a, b, func, ...) \
        func(a, b, __VA_ARGS__)

int
main(void)
{
        struct ast_prim p;
        struct ast_prim p2;
        struct ast_bin_op op;
        struct ast *ap = &op.abo_ast;
        struct ast_dump_visitor dv;
        struct ast_gen_visitor gv;

        vfn(1, 2, fn, stdout, 3);

        ast_prim_init_double(&p, AST_PRIM_DOUBLE, 123.456);
        ast_prim_init_cstr(&p2, AST_PRIM_CSTR, "hello world");
        ast_bin_op_init(&op, AST_BIN_OP_ADD, &p.ap_ast, &p2.ap_ast);

        ast_dump_visitor_init(&dv, stdout, 0);
        ap->a_visit(ap, &dv.adv_v);

        ast_gen_visitor_init(&gv, stdout);
        ap->a_visit(ap, &gv.agv_v);
}
