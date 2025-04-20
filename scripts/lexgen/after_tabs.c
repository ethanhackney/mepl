void
lex_init(struct lex *lp, struct buf *bp)
{
        dbug(lp == NULL, "lex_init(): lp == NULL");
        dbug(bp == NULL, "lex_init(): bp == NULL");
        dbug(bp->b_getc == NULL, "lex_init(): bp->b_getc == NULL");

        dstr_init(&lp->l_lex);
        lp->l_buf = bp;
        lp->l_type = TT_EOF;
        lp->l_putback = '\0';
        lex_next(lp);
}

void
lex_free(struct lex *lp)
{
        OK(lex_ok, lp);

        dstr_free(&lp->l_lex);
        ZERO_P(lp);
}

void
lex_next(struct lex *lp)
{
        int c = 0;

        OK(lex_ok, lp);

        c = lex_getc(lp);
        while (isspace(c))
                c = lex_getc(lp);

        dstr_clear(&lp->l_lex);

        if (c == BUF_EOF)
                lp->l_type = TT_EOF;
        else
                CHAR_FN[c](lp, c);
}

static void
id(struct lex *lp, int c)
{
        size_t i = 0;

        while (isalpha(c)) {
                dstr_push(&lp->l_lex, c);
                c = lex_getc(lp);
        }

        lp->l_type = TT_ID;

        i = strhash(lp->l_lex.d_cur) % ARRAY_SIZE(KWORDS);
        if (strcmp(lp->l_lex.d_cur, KWORDS[i].k_id) == 0)
                lp->l_type = KWORDS[i].k_type;
}

static void
str(struct lex *lp, int c)
{
        while ((c = lex_getc(lp)) != BUF_EOF && c != '"')
                dstr_push(&lp->l_lex, c);

        lp->l_type = TT_STR_LIT;
        if (c != '"')
                usage("malformed string: \"%s\"", lp->l_lex.d_cur);
}

static void
single(struct lex *lp, int c)
{
        lp->l_type = SINGLE[c];
}

static void
err(struct lex *lp, int c)
{
        usage("lexer: character invalid: %c", c);
}

static int
lex_getc(struct lex *lp)
{
        int c = lp->l_putback;

        if (c != '\0')
                lp->l_putback = '\0';
        else
                c = lp->l_buf->b_getc(lp->l_buf);

        return c;
}

const char *
lex_name(struct lex *lp)
{
        OK(lex_ok, lp);
        return TT_NAMES[lp->l_type];
}
