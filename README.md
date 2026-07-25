# edualloc

`edualloc` is a small, readable memory allocator written in C. It implements allocation, zero-initialized allocation, resizing, and release over one contiguous heap that it manages itself. The project is intended for learning, experiments, and small embedded-style demonstrations—not as a replacement for the platform allocator in production.

## Features

- First-fit allocation with 16-byte alignment
- Splits oversized free blocks and coalesces adjacent free blocks
- `ea_malloc`, `ea_free`, `ea_calloc`, and `ea_realloc`
- Explicit heap lifecycle and statistics API
- Unit tests, a simple benchmark, CMake, and GitHub Actions
- No third-party runtime dependencies

## Quick start

```sh
cmake -S . -B build -DEA_BUILD_TESTS=ON -DEA_BUILD_BENCHMARK=ON
cmake --build build
ctest --test-dir build --output-on-failure
./build/edualloc_benchmark
```

On Windows, executables are under `build/Release` when using a multi-config generator.

## Usage

```c
#include <stdio.h>
#include "edualloc.h"

int main(void) {
    ea_heap *heap = ea_heap_create(1024 * 1024);
    if (heap == NULL) return 1;

    int *values = ea_calloc(heap, 100, sizeof(*values));
    if (values == NULL) {
        ea_heap_destroy(heap);
        return 1;
    }
    values[42] = 7;
    printf("%d\n", values[42]);

    ea_free(heap, values);
    ea_heap_destroy(heap);
}
```

Link the `edualloc` CMake target and add `include/` to your include paths.

## How it works

The heap begins with a single free block. Every block has a private header containing its payload size, free flag, and links to its neighbors. Allocation scans the blocks from the start and selects the first free block large enough for the request. If the spare space can hold another header plus a minimally aligned payload, it is split into a new free block. `ea_free` marks a block free and merges it with free neighbors.

`ea_realloc` preserves the old contents. It first tries to shrink in place or extend into the immediately following free block; otherwise it allocates a new block, copies the smaller size, and releases the old one.

The header never appears in the public API. Pointers returned by `edualloc` are aligned to `EA_ALIGNMENT` (currently 16 bytes).

## API and behavior

See [the API reference](docs/API.md) and [design notes](docs/DESIGN.md). Important semantics match the familiar C allocation functions where practical:

- A zero-byte allocation is promoted to one aligned byte and may return a non-null unique allocation.
- `ea_free(heap, NULL)` is a no-op.
- `ea_calloc` returns `NULL` on multiplication overflow.
- `ea_realloc(heap, NULL, size)` behaves like `ea_malloc`.
- `ea_realloc(heap, ptr, 0)` frees `ptr` and returns `NULL`.
- Passing a pointer not obtained from the same heap, or using a pointer after it is freed, is undefined behavior.

## Scope and limitations

This implementation is deliberately compact: it is not thread-safe, has no guard pages, does not return memory to the operating system before heap destruction, and uses linear first-fit searches. Those tradeoffs make the allocator easy to inspect. For a production allocator, use the system allocator or a specialized, audited alternative.

## Project layout

```
include/       public header
src/           allocator implementation
tests/         unit tests
benchmarks/    repeatable timing example
docs/          API and implementation notes
.github/       continuous-integration workflow
```

## Contributing

Contributions are welcome. Please read [CONTRIBUTING.md](CONTRIBUTING.md), run the tests, and keep changes focused. The project is licensed under MIT; see [LICENSE](LICENSE).
