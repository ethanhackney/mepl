#include "ast.h"
#include "ast_str.h"
#include "lib.h"
#include "slab.h"

#ifndef AST_STR_SLAB_SIZE
#define AST_STR_SLAB_SIZE (1 << 8)
#endif /* #ifndef AST_STR_SLAB_SIZE */

#define SLAB_DEF_FOR_EACH(P) \
        for (P = slab_tab_def; *P != NULL; P++)

#define SLAB_TAB_ENTRY_INIT(SLAB, NAME) \
{                                       \
        .ste_cond = COND_INIT,          \
        .ste_slab = SLAB,               \
        .ste_name = NAME,               \
        .ste_mut  = MUTEX_INIT,         \
}

#define SLAB_NEW(T, PREFIX, NAME, SIZE)                                 \
                                                                        \
SLAB_TYPE_DEF(struct T, _ ## NAME ## _slab, SIZE)                       \
struct NAME ## _slab {                                                  \
        struct _ ## NAME ## _slab PREFIX ## _slab;                      \
        struct slab               PREFIX ## _parent;                    \
};                                                                      \
SLAB_DEF(struct T, NAME ## _slab, PREFIX ## _parent, PREFIX ## _slab)   \
static struct NAME ## _slab NAME ## _slab = {                           \
        .PREFIX ## _parent = SLAB_INIT(NAME ## _slab),                  \
};

struct slab_tab_entry {
        struct slab *const        ste_slab;
        const char         *const ste_name;
        MUTEX_TYPE                ste_mut;
        COND_TYPE                 ste_cond;
};

static bool slab_init;

#ifdef DBUG
static inline void
ptr_mine(const struct slab_tab_entry *ep, void *p)
{
        struct slab *sp = ep->ste_slab;

        if (sp->s_mine(sp, p))
                return;

        errno = EFAULT;
        die("%p does not belong to %s", p, ep->ste_name);
}
static inline void
init_check(void)
{
        if (slab_init)
                return;

        errno = EINVAL;
        die("slab_tab not initialized");
}
static inline void
full_check(const struct slab_tab_entry *ep)
{
        const struct slab *sp = ep->ste_slab;

        if (sp->s_full(sp))
                return;

        errno = EINVAL;
        warn("%s is not full on exit", ep->ste_name);
}
#else
static inline void
ptr_mine(const struct slab_tab_entry *ep, void *p)
{
}
static inline void
init_check(void)
{
}
static inline void
full_check(const struct slab_tab_entry *ep)
{
}
#endif /* #ifdef DBUG */

enum {
        SLAB_TAB_SIZE = 3,
};

SLAB_NEW( ast_str,     asb,  ast_str,     AST_STR_SLAB_SIZE     )
SLAB_NEW( ast_var_def, avds, ast_var_def, AST_VAR_DEF_SLAB_SIZE )
SLAB_NEW( ast_if,      aif,  ast_if,      AST_IF_SLAB_SIZE      )

#include "tab.h"
        /*
static struct slab_tab_entry slab_tab[SLAB_TAB_SIZE] = {
        SLAB_TAB_ENTRY_INIT( &ast_str_slab.asb_parent,      "AST_STR"     ),
        SLAB_TAB_ENTRY_INIT( &ast_var_def_slab.avds_parent, "AST_VAR_DEF" ),
        SLAB_TAB_ENTRY_INIT( &ast_if_slab.aif_parent,       "AST_IF"      ),
};

static struct slab_tab_entry *slab_tab_def[] = {
        &slab_tab[0],
        &slab_tab[1],
        &slab_tab[2],
        NULL,
};
*/

static struct slab_tab_entry *slab_tab_find(const char *type);

void
slab_tab_exit_check(void)
{
        struct slab_tab_entry **epp = NULL;
        struct slab_tab_entry *ep = NULL;

        SLAB_DEF_FOR_EACH(epp) {
                ep = *epp;
                full_check(ep);
                MUTEX_DESTROY(&ep->ste_mut);
                COND_DESTROY(&ep->ste_cond);
        }
}

void
slab_tab_init(void)
{
        struct slab_tab_entry **epp = NULL;
        struct slab *sp = NULL;

        SLAB_DEF_FOR_EACH(epp) {
                sp = (*epp)->ste_slab;
                sp->s_init(sp);
        }

        slab_init = true;
}

void *
slab_tab_get(const char *type)
{
        struct slab_tab_entry *ep = slab_tab_find(type);
        struct slab *sp = ep->ste_slab;
        void *p = NULL;

        init_check();

        COND_WAIT(!sp->s_empty(sp), &ep->ste_cond, &ep->ste_mut);
        p = sp->s_get(sp);
        MUTEX_UNLOCK(&ep->ste_mut);

        return p;
}

static struct slab_tab_entry *
slab_tab_find(const char *type)
{
        struct slab_tab_entry *ep = NULL;

        dbug(type == NULL, "slab_tab_find(): type  == NULL");
        dbug(*type == '\0', "slab_tab_find(): *type == '\0'");

        ep = &slab_tab[strhash(type) % SLAB_TAB_SIZE];
        dbug(ep->ste_slab == NULL, "slab_tab_find(): bad ast type: %s", type);
        dbug(strcmp(ep->ste_name, type) != 0, "slab_tab_find(): no slab named: %s", type);

        return ep;
}

void
slab_tab_put(const char *type, void **pp)
{
        struct slab_tab_entry *ep = slab_tab_find(type);
        struct slab *sp = ep->ste_slab;

        dbug(pp == NULL, "slab_tab_put(): pp == NULL");
        init_check();
        ptr_mine(ep, *pp);

        sp->s_put(sp, pp);
        COND_SIGNAL(&ep->ste_cond);
}
