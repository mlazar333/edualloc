# API reference

## Heap lifecycle

`ea_heap *ea_heap_create(size_t heap_size)` creates a heap backed by `heap_size` bytes (rounded up to a 16-byte boundary). It returns `NULL` if the request is too small or system allocation fails. `ea_heap_destroy` releases the entire heap; all outstanding allocation pointers become invalid.

## Allocation

All allocation functions take an `ea_heap *` so allocations are never confused with the process-wide allocator.

| Function | Result |
| --- | --- |
| `ea_malloc(heap, size)` | Allocates at least `size` bytes, or returns `NULL`. |
| `ea_free(heap, ptr)` | Releases a pointer returned by this heap. `NULL` is accepted. |
| `ea_calloc(heap, count, size)` | Allocates and zeroes `count * size` bytes; detects overflow. |
| `ea_realloc(heap, ptr, size)` | Resizes an allocation, retaining data up to the smaller size. |

`ea_heap_get_stats` fills `ea_heap_stats` with payload-space totals and block counts. Header bytes are not included in `allocated_bytes` or `free_bytes`.
