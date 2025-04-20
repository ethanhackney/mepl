# AUTO GENERATED
SHELL   = bash
DEFS    = -DBUF_EOF=-1                                                   \
          -DBUF_STDIO_DATA_SIZE=$(shell echo "$$(( (1 << 13) - 1 ))") \
          -DBUF_UNIX_DATA_SIZE=$(shell echo "$$(( (1 << 13) - 1 ))")
CFLAGS  = $(DEFS) -Wall -Werror -pedantic
PREFIX  = /home/ethanhackney/code/mepl
FFLAGS  = $(CFLAGS) -O3
DFLAGS  = $(CFLAGS) -DDBUG -fsanitize=address,undefined
INCLUDE = -I$(PREFIX)/lib/include \
          -I$(PREFIX)/buf/include
SRC     = ./main.c                  \
          $(PREFIX)/buf/src/stdio.c \
          $(PREFIX)/buf/src/unix.c  \
          $(PREFIX)/buf/src/cstr.c  \
          $(PREFIX)/buf/src/nstr.c  \
          $(PREFIX)/lib/src/dstr.c
CC      = gcc

safe:
	$(CC) $(INCLUDE) $(DFLAGS) $(SRC)

fast:
	$(CC) $(INCLUDE) $(FFLAGS) $(SRC)

tests:
	$(CC) $(INCLUDE) $(DFLAGS) $(SRC)
	./a.out
	rm a.out
