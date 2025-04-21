#ifndef AST_H
#define AST_H

#include <stdio.h>
#include "visitor.h"

struct ast {
        void (*a_gen)(const struct ast *ap, FILE *fp);
        void (*a_visit)(const struct ast *ap, struct ast_visitor *vp);
};

#endif
