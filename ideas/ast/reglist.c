#include "lib.h"
#include "reglist.h"

static const char REGS[REG_COUNT][REG_NAME_SIZE] = {
        "r0",
        "r1",
        "r2",
};

void
reglist_init(struct reglist *rp)
{
        memcpy(rp->r_regs, REGS, sizeof(REGS));
        rp->r_p = rp->r_regs + REG_COUNT;
}

const char *
reglist_get(struct reglist *rp)
{
        if (rp->r_p == rp->r_regs)
                die_c(ENOMEM, "reglist_get: stack underflow");

        return *--rp->r_p;
}

const char *
reglist_last(struct reglist *rp)
{
        if (rp->r_p == &rp->r_regs[REG_COUNT])
                die_c(EOVERFLOW, "reglist_last: no allocated registers");

        return *(rp->r_p + 1);
}

void
reglist_put(struct reglist *rp, const char *reg)
{
        if (rp->r_p == &rp->r_regs[REG_COUNT])
                die_c(EOVERFLOW, "reglist_put: stack overflow");

        strcpy(*rp->r_p++, reg);
}

static const char *regstk[] = {
        "r0",
        "r1",
        "r2",
};
static const char **sp = regstk + ARRAY_SIZE(regstk);

const char *
reg_get(void)
{
        if (sp == regstk)
                die_c(ENOMEM, "reg_get: stack underflow");

        return *--sp;
}

void
reg_put(const char *reg)
{
        if (sp == regstk + ARRAY_SIZE(regstk))
                die_c(EOVERFLOW, "reg_put: stack overflow");

        *sp++ = reg;
}

const char *
reg_last(void)
{
        if (sp == regstk + ARRAY_SIZE(regstk))
                die_c(EOVERFLOW, "reg_last: no allocated registers");

        return *(sp + 1);
}
