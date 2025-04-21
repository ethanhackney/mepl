#include "bin_op.h"

static void bin_op_visit(const struct ast *ap, struct ast_visitor *vp);

void ast_bin_op_init(struct ast_bin_op *op,
                     int type,
                     struct ast *left,
                     struct ast *right)
{
        op->abo_type = type;
        op->abo_left = left;
        op->abo_right = right;
        op->abo_ast.a_visit = bin_op_visit;
}

static void
bin_op_visit(const struct ast *ap, struct ast_visitor *vp)
{
        const struct ast_bin_op *op = bin_op(ap);

        vp->av_bin_op(vp, op);
}
