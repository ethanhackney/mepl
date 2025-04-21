CFLAGS = -Wall -Werror -pedantic
FFLAGS = $(CFLAGS) -O3
DFLAGS = $(CFLAGS) -fsanitize=address,undefined
SRC    = main.c dump.c gen.c bin_op.c prim.c reglist.c
CC     = gcc

safe:
	$(CC) $(DFLAGS) $(SRC)

fast:
	$(CC) $(FFLAGS) $(SRC)
