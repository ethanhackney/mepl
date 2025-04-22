CFLAGS = -Wall -Werror -pedantic
FFLAGS = $(CFLAGS) -O3
DFLAGS = $(CFLAGS) -fsanitize=address,undefined
SRC    = main.c
CC     = gcc

safe:
	$(CC) $(DFLAGS) $(SRC)

fast:
	$(CC) $(FFLAGS) $(SRC)
