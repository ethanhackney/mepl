#ifndef SLAB_TAB_H
#define SLAB_TAB_H

void slab_tab_exit_check(void);
void slab_tab_init(void);
void *slab_tab_get(const char *type);
void slab_tab_put(const char *type, void **pp);

#endif /* #ifndef SLAB_TAB_H */
