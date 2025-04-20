/**
 * create a thread for each input file that builds
 * a table of defined symbols
 *
 * for each file:
 *      thread_create(threadfn, file)
 *
 * thread_join(all)
 *
 * NOTE bad algorithm
 * ==================
 * for t int tab:
 *      for u in t.undef:
 *              for _t int tab:
 *                      if _t.def[u]:
 *                              delete t.undef[v]
 *                              delete t.def[v] = _t.def[u]
 *
 * generate linked executable
 *      
 *
 * threadfn(void *arg):
 *      build_def_tab(arg)
 *      maybe use condition variable for undefs instead
 *
 */
