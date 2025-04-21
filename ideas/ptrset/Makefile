CFLAGS = -Wall -Werror -pedantic
FFLAGS = $(CFLAGS) -O3
DFLAGS = $(CFLAGS) -fsanitize=address,undefined
SRC    = main.c fixset.c dynarr_set.c list_set.c bst_set.c
CC     = gcc

safe:
	$(CC) $(DFLAGS) $(SRC)

fast:
	$(CC) $(FFLAGS) $(SRC)
