#include "allocator.h"

arena_t arena_create(size_t capacity) {
    uint8_t *buffer = malloc(capacity * sizeof(uint8_t));
    if (buffer == NULL) {
        return (arena_t){0};
    }
    arena_t new_arena;
    new_arena.buffer = buffer;
    new_arena.capacity = capacity;
    new_arena.offset = 0;

    return new_arena;
}

void arena_free(arena_t *arena) {
    if (arena == NULL) {
        return;
    }
    arena->offset = 0;
    arena->capacity = 0;
    free(arena->buffer);
    arena->buffer = NULL;
}

void *arena_alloc_align(arena_t *arena, size_t size, size_t alignment) {
    if (arena == NULL){
            return NULL;
        }
        size_t aligned_offset = (arena->offset + (alignment - 1)) & ~(alignment - 1);
        if (aligned_offset + size > arena->capacity) {
            return NULL;
        } else {
            void *return_ptr = arena->buffer + aligned_offset;
            arena->offset = aligned_offset + size;
            return return_ptr;
        }
}

void *arena_alloc(arena_t *arena, size_t size) {
    if (arena == NULL){
        return NULL;
    }
    return arena_alloc_align(arena, size, sizeof(void *));
}

// void *arena_alloc_zero(arena_t *arena, size_t size) {
//     if (arena == NULL){
//         return NULL;
//     }
// }

void arena_reset(arena_t *arena) {
    if (arena == NULL) {
        return;
    }
    arena->offset = 0;
}

// arena_temp_t arena_temp_begin(arena_t *arena) {

// }

// void arena_temp_end(arena_temp_t *temp) {

// }