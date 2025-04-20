/**
 * buf_nstr{}
 * buf_nstr_init()
 * buf_nstr_free()
 */
#include <buf_nstr.h>

/**
 * buf_stdio{}
 * buf_stdio_init()
 * buf_stdio_free()
 */
#include <buf_stdio.h>

/**
 * unlink()
 * close()
 */
#include <unistd.h>

/**
 * open()
 * O_RDONLY
 */
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

/**
 * system()
 * atexit()
 */
#include <stdlib.h>

/**
 * BUF_EOF
 */
#include <buf.h>

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
 * ZALLOC()
 * IGNORE_RET
 * err_exit()
 * die()
 */
#include <lib.h>

/**
 * FILE{}
 * fopen()
 * fclose()
 * putchar()
 * EOF
 */
#include <stdio.h>

/* source of copy */
#define SRC_PATH  "main.c"
/* destination of copy */
#define COPY_PATH "copy.c"
/* diff command line */
#define DIFF_CMD  "cmp "SRC_PATH" "COPY_PATH" >/dev/null"

/**
 * cleanup function:
 *
 * args:
 *  none
 *
 * ret:
 *  none
 */
static void cleanup(void);

/**
 * test buf_stdio:
 *
 * args:
 *  none
 *
 * ret:
 *  none
 *
 * NOTE:
 *  may exit process
 */
static void buf_stdio_test(void);

/**
 * test buf
 *
 * args:
 *  @bp: pointer to buf
 *
 * ret:
 *  none
 *
 * NOTE:
 *  may exit process
 */
static void buf_test(struct buf *bp);

/**
 * test buf_unix:
 *
 * args:
 *  none
 *
 * ret:
 *  none
 *
 * NOTE:
 *  may exit process
 */
static void buf_unix_test(void);

/**
 * test buf_cstr:
 *
 * args:
 *  none
 *
 * ret:
 *  none
 *
 * NOTE:
 *  may exit process
 */
static void buf_cstr_test(void);

/**
 * test buf_nstr:
 *
 * args:
 *  none
 *
 * ret:
 *  none
 *
 * NOTE:
 *  may exit process
 */
static void buf_nstr_test(void);


/**
 * diff test files:
 *
 * args:
 *  none
 *
 * ret:
 *  none
 *
 * NOTE:
 *  may exit process
 */
static void diff(void);

/**
 * call fopen() and die if it fails:
 *
 * args:
 *  @path: path of file to open
 *  @mode: file mode
 *
 * ret:
 *  pointer to FILE
 *
 * NOTE:
 *  may exit process
 */
static FILE *e_fopen(const char *path, const char *mode);

/**
 * call fclose() and die if it fails:
 *
 * args:
 *  @fp: pointer to FILE
 *
 * ret:
 *  may exit process
 */
static void e_fclose(FILE *fp);

int
main(void)
{
        if (atexit(cleanup) < 0)
                die("main(): atexit()");

        buf_stdio_test();
        buf_unix_test();
        buf_cstr_test();
        buf_nstr_test();
}

static void
cleanup(void)
{
        IGNORE_RET(unlink(COPY_PATH));
}

static void
buf_stdio_test(void)
{
        struct buf_stdio b   = {0};
        FILE             *fp = e_fopen(SRC_PATH, "r");

        buf_stdio_init(&b, fp);
        buf_test(&b.s_buf);
        buf_stdio_free(&b);
        e_fclose(fp);
}

static FILE *
e_fopen(const char *path, const char *mode)
{
        FILE *fp = fopen(path, mode);

        if (fp == NULL)
                die("e_fopen(): fopen()");

        return fp;
}

static void
e_fclose(FILE *fp)
{
        if (fclose(fp) < 0)
                die("e_fclose(): fclose()");
}

static void
buf_test(struct buf *bp)
{
        FILE *copyfp = e_fopen(COPY_PATH, "w");
        int  c       = 0;

        while ((c = bp->b_getc(bp)) != BUF_EOF) {
                if (fputc(c, copyfp) == EOF)
                        die("buf_test(): fputc()");
        }

        e_fclose(copyfp);
        diff();
}

static void
diff(void)
{
        switch (system(DIFF_CMD)) {
        case -1:
                die("diff(): system()");
        case 127:
                err_exit("diff(): shell could not be executed");
        case 1:
                err_exit("diff(): main.c != %s", COPY_PATH);
        }
}

static void
buf_unix_test(void)
{
        struct buf_unix b  = {0};
        int             fd = open(SRC_PATH, O_RDONLY);

        if (fd < 0)
                die("buf_unix_test(): open()");

        buf_unix_init(&b, fd);
        buf_test(&b.u_buf);
        buf_unix_free(&b);

        if (close(fd) < 0)
                die("buf_unix_test(): close()");
}

static void
buf_cstr_test(void)
{
        struct buf_cstr b    = {0};
        struct stat    stats = {0};
        off_t          sz    = 0;
        char           *data = NULL;
        FILE           *fp   = NULL;

        if (stat(SRC_PATH, &stats) < 0)
                die("buf_str_test(): stat()");

        sz = stats.st_size;
        data = ZALLOC(sz + 1);
        fp = e_fopen(SRC_PATH, "r");
        if (fread(data, 1, sz, fp) != sz)
                die("buf_str_test(): fread()");
        data[sz] = '\0';
        e_fclose(fp);

        buf_cstr_init(&b, &data);
        buf_test(&b.c_buf);
        buf_cstr_free(&b, &data);

        PP_FREE(&data);
}

static void
buf_nstr_test(void)
{
        struct buf_nstr b    = {0};
        struct stat    stats = {0};
        off_t          sz    = 0;
        char           *data = NULL;
        FILE           *fp   = NULL;

        if (stat(SRC_PATH, &stats) < 0)
                die("buf_nstr_test(): stat()");

        sz = stats.st_size;
        data = ZALLOC(sz);
        fp = e_fopen(SRC_PATH, "r");
        if (fread(data, 1, sz, fp) != sz)
                die("buf_str_test(): fread()");
        e_fclose(fp);

        buf_nstr_init(&b, &data, sz);
        buf_test(&b.n_buf);
        buf_nstr_free(&b, &data);

        PP_FREE(&data);
}
