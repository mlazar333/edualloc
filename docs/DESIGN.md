# Design notes

## Block layout

Each allocation is represented as `[ block header | aligned payload ]`. The header is private and stores the payload length, allocation state, and doubly linked list pointers. The linked list reflects physical heap order, which makes neighboring blocks inexpensive to identify.

## Allocation and fragmentation

Allocation uses first-fit: walk from the first block and choose the first free block large enough. A chosen block is split only if the remainder can form a valid header and an aligned minimum payload. Freeing joins with next and previous free neighbors, reducing external fragmentation.

This is intentionally an O(n) allocator. It trades scalability for transparent control flow suitable for code reading and experiments. Possible extensions include segregated free lists, a best-fit policy, boundary tags, canaries, locking, and a user-provided backing buffer.

## Safety model

The allocator checks size arithmetic for alignment and `calloc` multiplication. As with C's standard allocator, an invalid pointer, double free, or use after free is undefined behavior. This project does not attempt to detect every misuse because doing so would complicate the central teaching implementation.
