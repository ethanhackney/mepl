#ifndef REG_H
#define REG_H

enum {
        REG_NAME_SIZE = 3,
        REG_COUNT     = 3,
};

struct reglist {
        char   (*r_p)[REG_NAME_SIZE];
        char   r_regs[REG_COUNT][REG_NAME_SIZE];
};

const char *reglist_get(struct reglist *rp);
const char *reglist_last(struct reglist *rp);
void       reglist_init(struct reglist *rp);
void       reglist_put(struct reglist *rp, const char *reg);

#endif
