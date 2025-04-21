#ifndef AST_VISITOR_H
#define AST_VISITOR_H

struct ast_bin_op;
struct ast_prim;

struct ast_visitor {
        void (*av_bin_op)(struct ast_visitor *vp, const struct ast_bin_op *op);
        void (*av_prim)(struct ast_visitor *vp, const struct ast_prim *pp);
};

#endif
