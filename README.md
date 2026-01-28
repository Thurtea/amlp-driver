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

## Quick Start for MUD Admins

Want to run a live MUD server? See the [Administrator Guide](docs/ADMIN_GUIDE.md) for complete setup instructions.

**Basic Setup:**

```bash
# Build the server
make clean && make all

# Start the MUD server on port 3000
./mud.ctl start

# Check server status
./mud.ctl status

# Connect and create first admin account
telnet localhost 3000
```

**Key Features:**
- First player automatically gets admin privileges
- Three privilege levels: Player (0), Wizard (1), Admin (2)
- Use `promote <player> <level>` to grant privileges
- Supports telnet and Mudlet clients
- Full server management via `mud.ctl` script

See [docs/ADMIN_GUIDE.md](docs/ADMIN_GUIDE.md) for detailed instructions on:
- Port configuration
- Privilege system
- Connecting with telnet/Mudlet
- Server management commands
- Troubleshooting

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

- [Administrator Guide](docs/ADMIN_GUIDE.md) - Complete server setup and management
- [Wiztool Guide](docs/WIZTOOL.md) - In-game building and object creation
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
