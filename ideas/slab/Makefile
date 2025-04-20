CFLAGS = -Wall -Werror -pedantic
FFLAGS = $(CFLAGS) -O3
DFLAGS = $(CFLAGS) -DDBUG -fsanitize=address,undefined
SRC    = main.c slab_tab.c ast_str.c
CC     = gcc

safe:
	./gen
	$(CC) $(DFLAGS) $(SRC)

fast:
	./gen
	$(CC) $(FFLAGS) $(SRC)
