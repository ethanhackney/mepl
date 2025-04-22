#include "lib.h"
#include "var_def.h"

#define VAR_DEF(ap) \
        STRUCT_OF(ap, struct ast_var_def, avd_ast)

static void var_def_visit(const struct ast *ap, struct ast_visitor *vp);

void
ast_var_def_init(struct ast_var_def *dp, char **dtypepp, char **namepp)
{
        dp->avd_ast.a_visit = var_def_visit;
        dp->avd_dtype = *dtypepp;
        dp->avd_name = *namepp;
        *dtypepp = NULL;
        *namepp = NULL;
}

static void
var_def_visit(const struct ast *ap, struct ast_visitor *vp)
{
        const struct ast_var_def *dp = VAR_DEF(ap);

        vp->av_var_def(vp, dp);
}

void
ast_var_def_free(struct ast_var_def *dp, char **dtypepp, char **namepp)
{
        if (dtypepp != NULL)
                *dtypepp = dp->avd_dtype;

        if (namepp != NULL)
                *namepp = dp->avd_name;

        memset(dp, 0, sizeof(*dp));
}
