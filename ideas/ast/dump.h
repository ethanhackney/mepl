#ifndef AST_DUMP_VISITOR_H
#define AST_DUMP_VISITOR_H

#include "visitor.h"
#include <stdio.h>

struct ast_dump_visitor {
        struct ast_visitor adv_v;
        FILE               *adv_fp;
        int                adv_space;
};

void ast_dump_visitor_init(struct ast_dump_visitor *dv, FILE *fp, int start);

#endif
