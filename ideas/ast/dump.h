#ifndef AST_DUMP_VISITOR_H
#define AST_DUMP_VISITOR_H

#include "visitor.h"

struct ast_dump_visitor {
        struct ast_visitor adv_v;
        int                adv_space;
};

void ast_dump_visitor_init(struct ast_dump_visitor *dv, int start);

#endif
