#ifndef EDUALLOC_H
#define EDUALLOC_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ea_heap ea_heap;

typedef struct ea_heap_stats {
    size_t capacity;
    size_t allocated_bytes;
    size_t free_bytes;
    size_t block_count;
    size_t free_block_count;
} ea_heap_stats;

/* Creates an allocator over a private heap of heap_size bytes. */
ea_heap *ea_heap_create(size_t heap_size);
void ea_heap_destroy(ea_heap *heap);

void *ea_malloc(ea_heap *heap, size_t size);
void ea_free(ea_heap *heap, void *ptr);
void *ea_calloc(ea_heap *heap, size_t count, size_t size);
void *ea_realloc(ea_heap *heap, void *ptr, size_t size);

/* Returns 0 on success, -1 for an invalid heap. */
int ea_heap_get_stats(const ea_heap *heap, ea_heap_stats *out_stats);

#ifdef __cplusplus
}
#endif

#endif
