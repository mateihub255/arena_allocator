#include <stdio.h>
#include <assert.h>
#include <stdint.h>
#include "allocator.h"

int main(void) {

    printf("--- Test Arena Creation & Free ---\n");

    size_t capacity = 1024;
    arena_t arena = arena_create(capacity);

    // Check creation
    assert(arena.buffer != NULL);
    assert(arena.capacity = capacity);
    assert(arena.offset == 0);

    // Check the whole buffer can be used
    arena.buffer[0] = 0xAA;
    arena.buffer[capacity - 1] = 0xBB;
    assert(arena.buffer[0] == 0xAA);
    assert(arena.buffer[capacity - 1] == 0xBB);

    // Check freeing
    arena_free(&arena);

    // Check null safety
    arena_free(NULL);

    printf("All tests passed!\n");
    return 0;
}