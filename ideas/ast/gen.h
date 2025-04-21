#ifndef GEN_H
#define GEN_H

#include "reglist.h"
#include "visitor.h"
#include <stdio.h>

struct ast_gen_visitor {
        struct ast_visitor agv_v;
        struct reglist     agv_reg;
        FILE               *agv_fp;
};

void ast_gen_visitor_init(struct ast_gen_visitor *gv, FILE *fp);

#endif
