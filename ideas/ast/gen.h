#ifndef GEN_H
#define GEN_H

#include "visitor.h"
#include <stdio.h>

struct ast_gen_visitor {
        struct ast_visitor agv_v;
        FILE               *agv_fp;
};

void ast_gen_visitor_init(struct ast_gen_visitor *gv, FILE *fp);

#endif
