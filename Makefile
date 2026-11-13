CC = gcc
CFLAGS = -Wall -Wextra -Werror -Wno-unused-variable -Wno-unused-parameter -std=c11 -pedantic -g -fsanitize=address,undefined -Iinclude

.PHONY: all test clean

all: test

test: src/allocator.o tests/main.o
	$(CC) $(CFLAGS) -o test src/allocator.o tests/main.o
	./test

example: src/allocator.o examples/log_analyzer.o
	$(CC) $(CFLAGS) -o example src/allocator.o examples/log_analyzer.o
	./example

src/allocator.o: src/allocator.c include/allocator.h
	$(CC) $(CFLAGS) -c src/allocator.c -o src/allocator.o

tests/main.o: tests/main.c include/allocator.h
	$(CC) $(CFLAGS) -c tests/main.c -o tests/main.o

examples/log_analyzer.o: examples/log_analyzer.c include/allocator.h
	$(CC) $(CFLAGS) -c examples/log_analyzer.c -o examples/log_analyzer.o

clean:
	rm -rf test example src/*.o tests/*.o examples/*.o *.dSYM