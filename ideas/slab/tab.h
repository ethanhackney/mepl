struct slab_tab_entry slab_tab[3] = {
	SLAB_TAB_ENTRY_INIT(&ast_str_slab.asb_parent, "AST_STR"),
	SLAB_TAB_ENTRY_INIT(&ast_var_def_slab.avds_parent, "AST_VAR_DEF"),
	SLAB_TAB_ENTRY_INIT(&ast_if_slab.aif_parent, "AST_IF"),
};

static struct slab_tab_entry *slab_tab_def[] = {
	&slab_tab[0],
	&slab_tab[1],
	&slab_tab[2],
	NULL,
};
