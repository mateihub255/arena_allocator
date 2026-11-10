#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

typedef struct Arena{
    uint8_t *buffer;
    size_t capacity;
    size_t offset;
} arena_t;

typedef struct ArenaTemp{
    arena_t *arena;
    size_t saved_offset;
} arena_temp_t;

/**
 * @brief Allocate a block of contiguous memory from the system.
 */
arena_t arena_create(size_t capacity);

/**
 * @brief Free the block of memory when done with it.
 */
void arena_free(arena_t *arena);

/**
 * @brief Allocate a block of contiguous memory using explicit alignment.
 */
void *arena_alloc_align(arena_t *arena, size_t size, size_t alignment);

/**
 * @brief Allocate a block of contiguous memory using default alignment.
 */
void *arena_alloc(arena_t *arena, size_t size);

/**
 * @brief Allocate a block of contiguous memory using default alignment and zero it out.
 */
void *arena_alloc_zero(arena_t *arena, size_t size);

/**
 * @brief Discard all allocations (but keep the block of memory).
 */
void arena_reset(arena_t *arena);

/**
 * @brief Capture a snapshot of the arena's current state.
 */
arena_temp_t arena_temp_begin(arena_t *arena);

/**
 * @brief Revert to snapshot taken in arena_temp_begin, used to quickly free big parts of the arena with short lifespans.
 */
void arena_temp_end(arena_temp_t *temp);