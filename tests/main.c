#include <stdio.h>
#include <assert.h>
#include <stdint.h>
#include <string.h>
#include "allocator.h"

static void test_creation_and_free(void) {
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

    // Check free works
    arena_free(&arena);

    // Check null safety
    arena_free(NULL);
}

static void test_alloc_align(arena_t *arena) {
    char *string = "Hello Allocation";
    size_t length = strlen(string) + 1;
    char *temp = arena_alloc_align(arena, length, 1);
    assert(temp != NULL);

    strcpy(temp, string);

    int *numbers = arena_alloc_align(arena, sizeof(int), 8);
    assert(numbers != NULL);

    // Test alignment
    assert(((uintptr_t)numbers % sizeof(int)) == 0);
}

static void test_reset(arena_t *arena) {
    size_t capacity = 1024;

    arena_reset(arena);

    assert(arena->buffer != NULL);
    assert(arena->capacity == capacity);
    assert(arena->offset == 0);
    
    arena_reset(NULL);
}

static void test_alignment_overflow(arena_t *arena) {
    size_t capacity = 1024;
    char *one_byte = arena_alloc_align(arena, 1, sizeof(char));
    int *big_array = arena_alloc_align(arena, 255 * sizeof(int), sizeof(int));

    assert(one_byte != NULL);
    assert(big_array != NULL);
    assert(((uintptr_t)big_array % sizeof(int)) == 0);
    // Check maximum capacity with alignment
    assert(arena->offset == capacity);

    arena_reset(arena);

    char *byte = arena_alloc_align(arena, 1, sizeof(char));
    assert(byte != NULL);
    assert(arena->offset == 1);

    int *bigger_array = arena_alloc_align(arena, 256 * sizeof(int), sizeof(int));
    assert(bigger_array == NULL); // check it wasnt alloced

    // Check buffer can not overflow from alignment logic
    assert(arena->offset == 1);
}

int main(void) {

    printf("--- Testing Arena Creation & Free ---\n");
    test_creation_and_free();

    arena_t arena = arena_create(1024);

    printf("--- Testing Alloc Functions ---\n");
    test_alloc_align(&arena);
    
    printf("--- Testing Reset ---\n");
    test_reset(&arena);

    printf("--- Testing Alignment Overflow ---\n");
    test_alignment_overflow(&arena);

    arena_free(&arena);

    printf("All Tests Passed!\n");
    return 0;
}