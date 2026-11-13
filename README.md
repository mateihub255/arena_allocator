# Arena Allocator
This is my implementation of an arena allocator for C.

## Prerequisites

- **C Compiler:** `gcc` or `clang` with C11 support
- **Build Tool:** GNU `make`
- **Sanitizers:** `libasan` & `libubsan`

## Build & Run

```bash
# Build and run the unit test suite
make test

# Build and run the log analyzer example
make example

# Remove compiled binaries and object files
make clean