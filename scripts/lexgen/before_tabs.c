/* AUTO GENERATED */
/**
 * lex{}
 * lex_init()
 * lex_free()
 * lex_next()
 */
#include <lex.h>

/**
 * OK_DEF()
 * dbug()
 * die()
 * usage()
 * OK()
 */
#include <lib.h>

/**
 * isspace()
 * isalpha()
 * isdigit()
 */
#include <ctype.h>

/**
 * UCHAR_MAX
 */
#include <limits.h>

/* keyword */
struct kword {
        const char *k_id;  /* identifier */
        int        k_type; /* token type */
};

/**
 * validate lex:
 *
 * args:
 *  @lp:     pointer to lex
 *  @caller: who called us? (for error message)
 *
 * ret:
 *  none
 */
OK_DEF(lex_ok, const struct lex *lp,
        dbug(lp == NULL, "%s(): lp == NULL", caller);
        dbug(lp->l_type < TT_EOF || lp->l_type >= TT_COUNT,
             "%s(): l_type invalid: %d", lp->l_type,
             caller);
        dbug(lp->l_buf == NULL, "%s(): l_buf == NULL", caller);
        dbug(lp->l_buf->b_getc == NULL, "%s(): l_buf->b_getc == NULL", caller);
        dbug(lp->l_putback < 0 || lp->l_putback > UCHAR_MAX,
             "%s(): l_putback not a char",
             caller);
)

/**
 * read an identifier:
 *
 * args: 
 *  @lp: pointer to lex
 *  @c:  character that triggered this function
 *
 * ret:
 *  none
 */
static void id(struct lex *lp, int c);

/**
 * read a string literal:
 *
 * args: 
 *  @lp: pointer to lex
 *  @c:  character that triggered this function
 *
 * ret:
 *  none
 */
static void str(struct lex *lp, int c);

/**
 * read a single special character:
 *
 * args: 
 *  @lp: pointer to lex
 *  @c:  character that triggered this function
 *
 * ret:
 *  none
 */
static void single(struct lex *lp, int c);

/**
 * run when input character is invalid:
 *
 * args:
 *  @lp: pointer to lex
 *  @c:  character that triggered this function
 *
 * ret:
 *  none
 */
static void err(struct lex *lp, int c);

/**
 * get next character:
 *
 * args:
 *  @lp: pointer to lex
 *
 * ret:
 *  next character or BUF_EOF on end of file
 */
static int lex_getc(struct lex *lp);
