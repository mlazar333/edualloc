#include "edualloc.h"

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

static ea_heap *new_heap(void) { return ea_heap_create(64 * 1024); }

static void test_allocate_and_free(void) {
    ea_heap *heap = new_heap(); assert(heap);
    int *value = ea_malloc(heap, sizeof(*value)); assert(value);
    assert(((uintptr_t)value % 16u) == 0); *value = 42; assert(*value == 42);
    ea_free(heap, value); ea_heap_stats stats; assert(ea_heap_get_stats(heap, &stats) == 0);
    assert(stats.allocated_bytes == 0 && stats.free_block_count == 1);
    ea_heap_destroy(heap);
}

static void test_calloc_and_overflow(void) {
    ea_heap *heap = new_heap(); assert(heap);
    unsigned char *p = ea_calloc(heap, 128, 1); assert(p);
    for (size_t i = 0; i < 128; ++i) assert(p[i] == 0);
    assert(ea_calloc(heap, SIZE_MAX, 2) == NULL);
    ea_free(heap, p); ea_heap_destroy(heap);
}

static void test_realloc(void) {
    ea_heap *heap = new_heap(); assert(heap);
    char *p = ea_malloc(heap, 32); assert(p); strcpy(p, "allocator");
    char *larger = ea_realloc(heap, p, 512); assert(larger);
    assert(strcmp(larger, "allocator") == 0);
    char *smaller = ea_realloc(heap, larger, 16); assert(smaller);
    assert(strcmp(smaller, "allocator") == 0);
    assert(ea_realloc(heap, smaller, 0) == NULL);
    ea_heap_destroy(heap);
}

static void test_coalescing(void) {
    ea_heap *heap = new_heap(); assert(heap);
    void *a = ea_malloc(heap, 1000); void *b = ea_malloc(heap, 1000); assert(a && b);
    ea_free(heap, a); ea_free(heap, b);
    void *large = ea_malloc(heap, 1900); assert(large);
    ea_free(heap, large); ea_heap_destroy(heap);
}

int main(void) {
    test_allocate_and_free(); test_calloc_and_overflow(); test_realloc(); test_coalescing();
    puts("All edualloc tests passed."); return 0;
}
