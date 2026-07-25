#include "edualloc.h"

#include <stdio.h>
#include <time.h>

int main(void) {
    const size_t iterations = 100000;
    ea_heap *heap = ea_heap_create(16 * 1024 * 1024);
    if (!heap) return 1;
    clock_t start = clock();
    for (size_t i = 0; i < iterations; ++i) {
        size_t size = 16 + (i % 32) * 16;
        void *p = ea_malloc(heap, size);
        if (!p) { fprintf(stderr, "allocation failed at %zu\n", i); ea_heap_destroy(heap); return 1; }
        ea_free(heap, p);
    }
    double seconds = (double)(clock() - start) / CLOCKS_PER_SEC;
    printf("%zu allocate/free pairs in %.3f seconds (%.0f ops/s)\n", iterations, seconds,
           seconds > 0.0 ? iterations / seconds : 0.0);
    ea_heap_destroy(heap);
    return 0;
}
