#include "base.h"
#include "bitset.h"
#include "lib.h"
#include "list.h"
#include "ring.h"
#include "slab.h"

BITSET_DEF(static, myset)
RING_DEF(static, int, intbuf)

struct ast {
        void (*a_dump)(const struct ast *ap, int space);
};

struct ast_var_def {
        struct ptr *avd_ptr;
        struct ast  avd_ast;
        struct ast *avd_init;
        char       *avd_dtype;
        char       *avd_name;
};

struct ast_str {
        struct ptr *as_ptr;
        struct ast  as_ast;
        char       *as_str;
};

#define SLAB_DERIVE(LINKAGE, T, NAME)                                           \
                                                                                \
SLAB_DEF(LINKAGE, T, _ ## NAME)                                                 \
struct NAME {                                                                   \
        struct slab_base base;                                                  \
        struct _ ## NAME slab;                                                  \
};                                                                              \
                                                                                \
LINKAGE struct ptr *                                                            \
NAME ## _get(struct slab_base *bp)                                              \
{                                                                               \
        struct NAME *sp = ctor(bp, struct NAME, base);                          \
        return _ ## NAME ## _get(&sp->slab);                                    \
}                                                                               \
                                                                                \
LINKAGE void                                                                    \
NAME ## _put(struct slab_base *bp, struct ptr **pp)                             \
{                                                                               \
        struct NAME *sp = ctor(bp, struct NAME, base);                          \
        _ ## NAME ## _put(&sp->slab, pp);                                       \
}                                                                               \
                                                                                \
LINKAGE void                                                                    \
NAME ## _free(struct slab_base *bp)                                             \
{                                                                               \
        struct NAME *sp = ctor(bp, struct NAME, base);                          \
        _ ## NAME ## _free(&sp->slab);                                          \
}                                                                               \
                                                                                \
LINKAGE void                                                                    \
NAME ## _init(struct slab_base *bp, size_t arr_size, size_t ring_size)          \
{                                                                               \
        struct NAME *sp = ctor(bp, struct NAME, base);                          \
        _ ## NAME ## _init(&sp->slab, arr_size, ring_size);                     \
}

SLAB_DERIVE(static, struct ast_var_def, var_def_slab)
SLAB_DERIVE(static, struct ast_str,     str_slab)

enum {
        NR_SLABS = 2,
};

/** TODO: write a script to generate all this stuff from
 * a tabular file
 */
struct slab_arg {
        size_t sa_arr_size;
        size_t sa_ring_size;
};

static void slab_tab_init(const struct slab_arg *args);
static void slab_tab_free(void);
static struct ptr *slab_tab_get(const char *type);
static void slab_tab_put(const char *type, struct ptr **p);
static size_t hashfn(const char *type);

#define SLAB_BASE_INIT(LINKAGE, NAME, ID)       \
LINKAGE struct NAME ID = {                      \
        .base = {                               \
                .sb_init = NAME ## _init,       \
                .sb_get  = NAME ## _get,        \
                .sb_put  = NAME ## _put,        \
                .sb_free = NAME ## _free,       \
        }                                       \
}

SLAB_BASE_INIT(static, var_def_slab, vds);
SLAB_BASE_INIT(static, str_slab,     ss );

static struct slab_tab_entry {
        char *key;
        struct slab_base *base;
} slabtab[3] = {
	{ "AST_VAR_DEF", &vds.base },
	{ "AST_STR",     &ss.base  },
	{ NULL,          NULL      },
};

static struct slab_tab_entry *def[] = {
        &slabtab[0],
        &slabtab[1],
        NULL,
};

int
main(void)
{
        struct slab_arg args[sizeof(def) / sizeof(*def)] = {
                { (1 << 8), (1 << 8)  },
                { (1 << 4), (1 << 10) },
        };
        struct ptr *ptr;
        struct ast_var_def *dp;

        slab_tab_init(args);

        ptr = slab_tab_get("AST_VAR_DEF");
        dp = ptr->p_ptr;
        dp->avd_ptr = ptr;
        dp->avd_name = "ethan";
        puts(dp->avd_name);

        slab_tab_put("AST_VAR_DEF", &ptr);

        slab_tab_free();
}

static void
slab_tab_init(const struct slab_arg *args)
{
        const struct slab_arg *ap = args;
        struct slab_tab_entry **spp = NULL;
        struct slab_base *bp = NULL;

        for (spp = def; *spp; spp++, ap++) {
                bp = (*spp)->base;
                bp->sb_init(bp, ap->sa_arr_size, ap->sa_ring_size);
        }
}

static void
slab_tab_free(void)
{
        struct slab_tab_entry **spp = NULL;
        struct slab_base *bp = NULL;

        for (spp = def; *spp; spp++) {
                bp = (*spp)->base;
                bp->sb_free(bp);
        }
}

static struct ptr *
slab_tab_get(const char *type)
{
        struct slab_base *bp = NULL;
        size_t i = hashfn(type) % 4;

        bp = slabtab[i].base;
        ASSERT(bp);
        ASSERT(!strcmp(slabtab[i].key, type));
        return bp->sb_get(bp);
}

static size_t
hashfn(const char *type)
{
        size_t hash = 0;

        for (; *type; type++)
                hash = hash * 31 + *type;

        return hash;
}

static void
slab_tab_put(const char *type, struct ptr **p)
{
        struct slab_base *bp = NULL;
        size_t i = hashfn(type) % 4;

        bp = slabtab[i].base;
        ASSERT(bp);
        ASSERT(!strcmp(slabtab[i].key, type));
        bp->sb_put(bp, p);
}
