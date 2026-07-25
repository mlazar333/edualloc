#include "edualloc.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define EA_ALIGNMENT 16u
#define EA_MIN_PAYLOAD EA_ALIGNMENT

typedef struct ea_block {
    size_t size;
    int is_free;
    struct ea_block *prev;
    struct ea_block *next;
} ea_block;

struct ea_heap {
    unsigned char *memory;
    size_t capacity;
    ea_block *first;
};

static size_t align_up(size_t size) {
    const size_t mask = EA_ALIGNMENT - 1u;
    if (size > SIZE_MAX - mask) return 0;
    return (size + mask) & ~mask;
}

static void *payload(ea_block *block) { return (unsigned char *)block + sizeof(*block); }
static ea_block *header(void *ptr) { return (ea_block *)((unsigned char *)ptr - sizeof(ea_block)); }

static void split_block(ea_block *block, size_t wanted) {
    size_t remainder = block->size - wanted;
    if (remainder < sizeof(ea_block) + EA_MIN_PAYLOAD) return;

    ea_block *new_block = (ea_block *)((unsigned char *)payload(block) + wanted);
    new_block->size = remainder - sizeof(ea_block);
    new_block->is_free = 1;
    new_block->prev = block;
    new_block->next = block->next;
    if (new_block->next) new_block->next->prev = new_block;
    block->size = wanted;
    block->next = new_block;
}

static ea_block *merge_with_next(ea_block *block) {
    ea_block *next = block->next;
    if (next && next->is_free) {
        block->size += sizeof(ea_block) + next->size;
        block->next = next->next;
        if (block->next) block->next->prev = block;
    }
    return block;
}

ea_heap *ea_heap_create(size_t heap_size) {
    size_t aligned = align_up(heap_size);
    if (aligned == 0 || aligned <= sizeof(ea_block) + EA_MIN_PAYLOAD) return NULL;
    ea_heap *heap = calloc(1, sizeof(*heap));
    if (!heap) return NULL;
    heap->memory = malloc(aligned);
    if (!heap->memory) { free(heap); return NULL; }
    heap->capacity = aligned;
    heap->first = (ea_block *)heap->memory;
    heap->first->size = aligned - sizeof(ea_block);
    heap->first->is_free = 1;
    return heap;
}

void ea_heap_destroy(ea_heap *heap) {
    if (!heap) return;
    free(heap->memory);
    free(heap);
}

void *ea_malloc(ea_heap *heap, size_t size) {
    if (!heap) return NULL;
    size_t wanted = align_up(size == 0 ? 1 : size);
    if (wanted == 0) return NULL;
    for (ea_block *block = heap->first; block; block = block->next) {
        if (block->is_free && block->size >= wanted) {
            split_block(block, wanted);
            block->is_free = 0;
            return payload(block);
        }
    }
    return NULL;
}

void ea_free(ea_heap *heap, void *ptr) {
    (void)heap;
    if (!ptr) return;
    ea_block *block = header(ptr);
    block->is_free = 1;
    if (block->next && block->next->is_free) merge_with_next(block);
    if (block->prev && block->prev->is_free) merge_with_next(block->prev);
}

void *ea_calloc(ea_heap *heap, size_t count, size_t size) {
    if (size != 0 && count > SIZE_MAX / size) return NULL;
    size_t total = count * size;
    void *ptr = ea_malloc(heap, total);
    if (ptr) memset(ptr, 0, total);
    return ptr;
}

void *ea_realloc(ea_heap *heap, void *ptr, size_t size) {
    if (!ptr) return ea_malloc(heap, size);
    if (size == 0) { ea_free(heap, ptr); return NULL; }
    size_t wanted = align_up(size);
    if (wanted == 0) return NULL;
    ea_block *block = header(ptr);
    if (block->size >= wanted) { split_block(block, wanted); return ptr; }
    if (block->next && block->next->is_free &&
        block->size + sizeof(ea_block) + block->next->size >= wanted) {
        merge_with_next(block);
        split_block(block, wanted);
        return ptr;
    }
    void *replacement = ea_malloc(heap, size);
    if (!replacement) return NULL;
    memcpy(replacement, ptr, block->size);
    ea_free(heap, ptr);
    return replacement;
}

int ea_heap_get_stats(const ea_heap *heap, ea_heap_stats *out_stats) {
    if (!heap || !out_stats) return -1;
    memset(out_stats, 0, sizeof(*out_stats));
    out_stats->capacity = heap->capacity;
    for (const ea_block *block = heap->first; block; block = block->next) {
        out_stats->block_count++;
        if (block->is_free) { out_stats->free_block_count++; out_stats->free_bytes += block->size; }
        else out_stats->allocated_bytes += block->size;
    }
    return 0;
}
