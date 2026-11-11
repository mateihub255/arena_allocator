#include <stdio.h>
#include <assert.h>
#include <stdint.h>
#include <string.h>
#include "allocator.h"

int main(void) {

    printf("--- Test Arena Creation & Free ---\n");

    size_t capacity = 1024;
    arena_t arena = arena_create(capacity);

    // Check creation
    assert(arena.buffer != NULL);
    assert(arena.capacity == capacity);
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
    printf("--- Test Alloc Functions ---\n");

    arena = arena_create(capacity);

    assert(arena.buffer != NULL);
    assert(arena.capacity == capacity);
    assert(arena.offset == 0);

    char *string = "Hello Allocation";
    size_t length = strlen(string) + 1;
    char *temp = arena_alloc_align(&arena, length, 1);
    strcpy(temp, string);

    int *numbers = arena_alloc_align(&arena, sizeof(int), 8);

    assert(((uintptr_t)numbers % 8) == 0);

    arena_reset(&arena);

    assert(arena.buffer != NULL);
    assert(arena.capacity == capacity);
    assert(arena.offset == 0);

    char *one_byte = arena_alloc_align(&arena, 1, sizeof(char));
    int *big_array = arena_alloc_align(&arena, 255 * sizeof(int), sizeof(int));

    *one_byte = 'C';
    big_array[1] = 0;

    assert(((uintptr_t)numbers % 8) == 0);
    assert(arena.offset == capacity);

    arena_free(&arena);

    printf("All tests passed!\n");
    return 0;
}