/**
 * buf_stdio{}
 * buf_stdio_init()
 * buf_stdio_free()
 */
#include <buf_stdio.h>

/**
 * buf_unix{}
 * buf_unix_init()
 * buf_unix_free()
 */
#include <buf_unix.h>

/**
 * buf_cstr{}
 * buf_cstr_init()
 * buf_cstr_free()
 */
#include <buf_cstr.h>

/**
 * die()
 */
#include <lib.h>

/**
 * putchar()
 * EOF
 */
#include <stdio.h>

/**
 * lex{}
 * lex_init()
 * lex_free()
 * lex_next()
 * lex_name()
 */
#include <lex.h>

/**
 * create a thread for each input file
 */

int
main(void)
{
        struct buf_stdio b;
        struct lex l;

        buf_stdio_init(&b, stdin);
        lex_init(&l, &b.s_buf);

        while (l.l_type != TT_EOF) {
                printf("%s", lex_name(&l));
                if (l.l_type == TT_ID || l.l_type == TT_STR_LIT)
                        printf(": %s", l.l_lex.d_cur);
                printf("\n");
                lex_next(&l);
        }

        buf_stdio_free(&b);
}
