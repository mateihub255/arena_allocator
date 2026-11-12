CC = gcc
CFLAGS = -Wall -Wextra -Werror -Wno-unused-variable -Wno-unused-parameter -std=c11 -pedantic -g -fsanitize=address,undefined -Iinclude

.PHONY: all test clean

all: test

test: src/allocator.o tests/main.o
	$(CC) $(CFLAGS) -o test src/allocator.o tests/main.o
	./test

src/allocator.o: src/allocator.c include/allocator.h
	$(CC) $(CFLAGS) -c src/allocator.c -o src/allocator.o

tests/main.o: tests/main.c include/allocator.h
	$(CC) $(CFLAGS) -c tests/main.c -o tests/main.o

clean:
	rm -rf test src/*.o tests/*.o *.dSYM