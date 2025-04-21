#include "ast.h"
#include "bin_op.h"
#include "prim.h"
#include "dump.h"
#include "gen.h"
#include "lib.h"
#include <err.h>
#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <sysexits.h>

int
main(void)
{
        struct ast_prim p;
        struct ast_prim p2;
        struct ast_bin_op op;
        struct ast *ap = &op.abo_ast;
        struct ast_dump_visitor dv;
        struct ast_gen_visitor gv;

        ast_prim_init_double(&p, AST_PRIM_DOUBLE, 123.456);
        ast_prim_init_int(&p2, AST_PRIM_INT, 5);

        ast_bin_op_init(&op, AST_BIN_OP_ADD, &p.ap_ast, &p2.ap_ast);

        ast_dump_visitor_init(&dv, 0);
        ast_gen_visitor_init(&gv, stdout);

        ap->a_visit(ap, &dv.adv_v);
        ap->a_visit(ap, &gv.agv_v);
}
