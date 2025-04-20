/* AUTO GENERATED */
#ifndef LEX_H
#define LEX_H

/**
 * dstr{}
 * dstr_init()
 * dstr_free()
 * dstr_push()
 * dstr_clear()
 */
#include <dstr.h>

/**
 * buf{}
 * BUF_EOF
 */
#include <buf.h>

/* token types */
enum {
        TT_EOF,     /* end of file */
        TT_STR,     /* str */
        TT_ID,      /* identifier */
        TT_ASSIGN,  /* = */
        TT_STR_LIT, /* "literal" */
        TT_COUNT,   /* count of token types */
};

/* lexer */
struct lex {
        struct dstr l_lex;     /* current lexeme */
        struct buf  *l_buf;    /* buf */
        int         l_type;    /* current token type */
        int         l_putback; /* putback character */
};

/**
 * initialize lex:
 *
 * args:
 *  @lp: pointer to lex
 *  @bp: pointer to buf
 *
 * ret:
 *  none
 */
void lex_init(struct lex *lp, struct buf *bp);

/**
 * free lex:
 *
 * args:
 *  @lp: pointer to lex
 *
 * ret:
 *  none
 */
void lex_free(struct lex *lp);

/**
 * advance to next token:
 *
 * args:
 *  @lp: pointer to lex
 *
 * ret:
 *  none
 */
void lex_next(struct lex *lp);

/**
 * get current tokens type names:
 *
 * args:
 *  @lp: pointer to lex
 *
 * ret:
 *  pointer to name of type
 */
const char *lex_name(struct lex *lp);

#endif /* #ifndef LEX_H */
