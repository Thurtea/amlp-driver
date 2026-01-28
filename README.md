# AMLP Driver

A production-ready LPC MUD driver written from scratch in C. Complete lexer, parser, VM, garbage collector, and object system with comprehensive test coverage.

## Features

- **Complete LPC Implementation** - Lexer, parser, bytecode compiler, stack-based VM
- **Garbage Collection** - Reference-counted mark-and-sweep with automatic memory management
- **Object System** - Full OOP support with methods and inheritance
- **Data Structures** - GC-aware arrays and mappings (hash maps)
- **24 Built-in Functions** - String, array, math, type checking, and I/O efuns
- **220 Tests Passing** - 565 assertions, 100% pass rate, zero warnings

## Quick Start

```bash
# Build
make clean && make all

# Test
make test

# Run
./build/driver
```

## Project Stats

- **11,249 lines** of C code (6,500 core + 4,700 tests)
- **220 tests** with 565 assertions, 100% passing
- **Zero warnings** with `-Wall -Wextra -Werror`
- **Phase 6 complete** - Arrays & mappings implemented

## Architecture

```
src/
├── lexer.{h,c}      # Tokenization (50+ token types)
├── parser.{h,c}     # Recursive descent parser (25+ AST nodes)
├── codegen.{h,c}    # Bytecode generation
├── vm.{h,c}         # Stack-based VM (40+ opcodes)
├── object.{h,c}     # Object system with method dispatch
├── gc.{h,c}         # Reference-counted garbage collector
├── efun.{h,c}       # 24 built-in functions
├── array.{h,c}      # Dynamic arrays
├── mapping.{h,c}    # Hash maps
└── driver.c         # Main entry point

tests/               # Comprehensive test suites
lib/                 # LPC standard library
```

## Build Targets

```bash
make all             # Build driver and all tests
make driver          # Build driver only
make test            # Run all test suites
make clean           # Remove build artifacts
```

## Test Results

| Suite | Tests | Assertions | Status |
|-------|-------|------------|--------|
| Lexer | 20 | 41 | PASS |
| Parser | 40 | 97 | PASS |
| VM | 47 | 146 | PASS |
| Object | 16 | 33 | PASS |
| GC | 23 | 52 | PASS |
| Efun | 28 | 57 | PASS |
| Array | 23 | 70 | PASS |
| Mapping | 23 | 69 | PASS |
| **Total** | **220** | **565** | **100%** |

## Documentation

- [Development Guide](docs/DEVELOPMENT.md) - Detailed phase documentation
- [Build Summary](docs/BUILD_SUMMARY.md) - Build system details
- [VM Implementation](docs/VM_IMPLEMENTATION_SUMMARY.md) - VM architecture

## Contributing

This is an educational project demonstrating compiler construction, VM design, and garbage collection. Contributions welcome for:

- Additional efuns and language features
- Performance optimizations
- Extended test coverage
- Documentation improvements

## Requirements

- GCC 4.9+ or Clang
- GNU Make
- Standard C library (POSIX)

## License

MIT License - See LICENSE for details

---

**Status:** Production-ready | **Build:** Zero warnings | **Tests:** 100% passing
