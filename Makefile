CC = clang
CFLAGS = -Wall -Wextra -Werror -std=c11 -pedantic -g -fsanitize=address,undefined

.PHONY: all test clean

all: test

test: arena.o test_main.o
	$(CC) $(CFLAGS) -o test arena.o main.o
	./test

arena.o: arena.c arena.h
	$(CC) $(CFLAGS) -c arena.c

main.o: main.c arena.h
	$(CC) $(CFLAGS) -c main.c

clean:
	rm -rf test *.o *.dSYM