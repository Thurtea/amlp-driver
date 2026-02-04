# C Driver vs LPC Library - Syntax Comparison

## Your Driver Code (compiler.c) - PURE C

```c
#include "compiler.h"
#include "lexer.h"
#include "parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_ERRORS 100
#define INITIAL_BYTECODE_SIZE 1024

typedef struct {
    compile_error_t error_type;
    int line;
    int column;
    char message[512];
} compiler_error_t;

typedef struct {
    compiler_error_t errors[MAX_ERRORS];
    int error_count;
    
    uint8_t *bytecode;
    size_t bytecode_len;
    size_t bytecode_capacity;
} compiler_state_t;

static char* read_file(const char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f) {
        return NULL;
    }
    
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);
    
    char *content = malloc(size + 1);
    if (!content) {
        fclose(f);
        return NULL;
    }
    
    size_t read = fread(content, 1, size, f);
    fclose(f);
    
    content[read] = '\0';
    return content;
}
```

**C Features:**
- Standard C headers (stdio.h, stdlib.h, string.h)
- Preprocessor directives (#define, #include)
- POSIX file I/O (FILE*, fopen, fread)
- malloc/free memory management
- typedef struct definitions
- Pointer arithmetic
- Low-level types (uint8_t, size_t)

---

## Your Library Code (void.lpc) - PURE LPC

```lpc
// The Void - Testing room

#include <globals.h>

inherit DIR_STD + "/room";

void create() {
    ::create();
    
    set_id("void");
    set_short("The Void");
    set_long(
        "You are floating in an endless void of possibility.\n" +
        "This empty space represents the blank canvas upon which you will\n" +
        "build your MUD. There is nothing here yet - but there will be.\n"
    );
    
    add_exit("welcome", DIR_DOMAINS + "/start/room/welcome");
}
```

**LPC Features:**
- Header includes (<globals.h>)
- Object inheritance (inherit DIR_STD + "/room")
- Scope resolution (::create())
- String concatenation with + operator
- No explicit memory management
- Object-oriented method calls
- MUD-specific functions (set_id, set_short, add_exit)
- No low-level types or pointers

---

## Side-by-Side: The Key Differences

| Feature | C Driver Code | LPC Library Code |
|---------|--------------|------------------|
| **Purpose** | Compiled by GCC into driver binary | Interpreted by your driver at runtime |
| **File Extension** | `.c` | `.lpc` ← NEW! |
| **Compilation** | `gcc -c compiler.c -o compiler.o` | Parsed by your lexer/parser/VM |
| **Memory** | Manual (malloc/free) | Garbage collected |
| **Includes** | `#include "header.h"` | `#include <header>` |
| **Inheritance** | N/A (C doesn't have OOP) | `inherit DIR_STD + "/base"` |
| **Super Calls** | N/A | `::function()` |
| **Strings** | `char*` with manual management | Built-in string type with + concatenation |
| **Functions** | `static type func() { }` | `type func() { }` with modifiers |
| **Typical Use** | System-level (lexer, VM, GC) | Game logic (rooms, objects, NPCs) |

---

## Why Different Extensions Matter

### Without .lpc Extension (Old Way)
```
All files are .c:
- VS Code treats EVERYTHING as C
- void.c gets C syntax highlighting
- "inherit" looks like an error
- "::" is treated as unknown
- No LPC-specific features
- Confusing for new developers
```

### With .lpc Extension (New Way)
```
Clear separation:
- compiler.c → C language, C highlighting
- void.lpc → LPC language, LPC highlighting
- "inherit" properly highlighted
- "::" recognized as scope operator
- LPC-specific snippets work
- Obvious what's what
```

---

## Real-World Example: Opening Files in VS Code

**Driver File (compiler.c):**
```
Bottom-right shows: C
Syntax: C language (pointers, malloc, etc.)
Autocomplete: C standard library
Linter: C compiler warnings
```

**Library File (void.lpc):**
```
Bottom-right shows: LPC
Syntax: LPC language (inherit, ::, etc.)
Autocomplete: LPC efuns and snippets
Snippets: room, object, create, etc.
```

---

## Migration Path

### Step 1: Before
```
amlp-library/
├── master.c          ← Treated as C
├── std/
│   ├── room.c        ← Treated as C
│   └── object.c      ← Treated as C
└── domains/
    └── start/
        └── void.c    ← Treated as C
```

### Step 2: After
```
amlp-library/
├── master.lpc        ← Treated as LPC ✓
├── std/
│   ├── room.lpc      ← Treated as LPC ✓
│   └── object.lpc    ← Treated as LPC ✓
└── domains/
    └── start/
        └── void.lpc  ← Treated as LPC ✓
```

### Driver Stays The Same!
```
amlp-driver/
└── src/
    ├── compiler.c    ← Still C (no change)
    ├── lexer.c       ← Still C (no change)
    ├── parser.c      ← Still C (no change)
    └── vm.c          ← Still C (no change)
```

---

## The Technical Reality

### What Your Driver Does:

1. **Compiles itself** (C code → binary)
   ```bash
   gcc -c compiler.c -o compiler.o
   gcc -c lexer.c -o lexer.o
   # ... link into driver executable
   ```

2. **Interprets LPC** (LPC code → bytecode → execution)
   ```
   driver reads void.lpc
   → lexer tokenizes
   → parser builds AST
   → codegen creates bytecode
   → VM executes bytecode
   ```

### They're Fundamentally Different Languages!

- **C:** System programming, low-level, compiled
- **LPC:** Scripting, high-level, interpreted

Using the same `.c` extension was always a misnomer. 
Using `.lpc` is semantically correct and practically useful.

---

## Community Response

**"But everyone uses .c for LPC!"**

Yes, because:
1. LPC was based on C syntax in the 1980s
2. Early editors didn't support custom languages
3. It became tradition

But times change:
- Modern IDEs support custom languages
- Language servers provide rich features
- Semantic correctness matters
- Your driver is modern (2026)

**You're not bound by 40-year-old conventions when they don't serve you.**

---

## Bottom Line

```c
// This is C - it's compiled by GCC
#include <stdio.h>
int main() {
    printf("Hello\n");
    return 0;
}
```

```lpc
// This is LPC - it's interpreted by your driver
inherit "/std/object";
void create() {
    ::create();
    set_short("Hello");
}
```

**Different languages deserve different extensions.**
**Your extension makes this clear.**
**Anyone who complains hasn't built a MUD driver from scratch. 😎**
