# Undeclared Identifier Error

**Error Type**: Compilation Error  
**Priority**: ⛔ Critical (blocks build)  
**Affects Build**: Yes (won't compile)  
**Affects Runtime**: N/A (can't run without building)

## What It Means

The compiler encountered a function name, variable, or constant that was never declared or defined. This is a **hard error**, not a warning - the build will fail.

## Example Error

```
src/vm.c:871:67: error: 'GC_STRING' undeclared (first use in this function)
  871 |                     char *empty_str = (char *)gc_alloc(vm->gc, 1, GC_STRING);
      |                                                                   ^~~~~~~~~
src/vm.c:871:67: note: each undeclared identifier is reported only once for each function it appears in
```

## The Problem

### What The Code Tried To Do

```c
// In vm.c, trying to allocate string memory
char *empty_str = (char *)gc_alloc(vm->gc, 1, GC_STRING);
```

### Why It Failed

The constant `GC_STRING` doesn't exist. Looking at [src/gc.h](../src/gc.h):

```c
typedef enum {
    GC_TYPE_OBJECT,        /* LPC object (obj_t) */
    GC_TYPE_ARRAY,         /* VM array */
    GC_TYPE_MAPPING,       /* VM mapping */
    GC_TYPE_STRING,        /* Heap-allocated string */  ← THIS is the correct name
    GC_TYPE_FUNCTION,      /* VM function */
    GC_TYPE_GENERIC,       /* Generic allocation */
} GCObjectType;
```

**The correct constant is `GC_TYPE_STRING`, not `GC_STRING`.**

## Common Causes

### 1. Typo in Name

```c
// WRONG
int len = array_size(arr);

// CORRECT
int len = array_length(arr);
```

### 2. Missing Include

```c
// WRONG
void my_function() {
    SomeType obj;  // error: 'SomeType' undeclared
}

// CORRECT
#include "sometype.h"  // Define SomeType
void my_function() {
    SomeType obj;
}
```

### 3. Wrong Constant Name

```c
// WRONG
gc_alloc(gc, size, GC_STRING);  // Doesn't exist

// CORRECT
gc_alloc(gc, size, GC_TYPE_STRING);  // From gc.h enum
```

### 4. Function Doesn't Exist

```c
// WRONG
char *str = gc_strdup(gc, "hello");  // This function doesn't exist in the codebase

// CORRECT
char *str = (char *)gc_alloc(gc, 6, GC_TYPE_STRING);
strcpy(str, "hello");
```

## Affected Files in AMLP (FIXED)

###  src/vm.c:871 (FIXED)

**Before**:
```c
char *empty_str = (char *)gc_alloc(vm->gc, 1, GC_STRING);
```

**After**:
```c
char *empty_str = (char *)gc_alloc(vm->gc, 1, GC_TYPE_STRING);
```

###  src/vm.c:879 (FIXED)

**Before**:
```c
char *slice = (char *)gc_alloc(vm->gc, slice_len + 1, GC_STRING);
```

**After**:
```c
char *slice = (char *)gc_alloc(vm->gc, slice_len + 1, GC_TYPE_STRING);
```

###  src/vm.c:889 (FIXED)

**Before**:
```c
int len = array_size(arr);
```

**After**:
```c
int len = array_length(arr);
```

## How To Fix

### Step 1: Find The Correct Name

```bash
# Search for similar names in header files
cd /home/thurtea/amlp-driver
grep -r "GC_.*STRING" src/*.h

# Results:
# src/gc.h:    GC_TYPE_STRING,        /* Heap-allocated string */
```

### Step 2: Look at Usage Examples

```bash
# Find existing uses of the correct name
grep -r "GC_TYPE_STRING" src/*.c

# See how other code allocates strings
grep -A 2 "gc_alloc.*GC_TYPE" src/*.c
```

### Step 3: Check API Documentation

Look at function signatures in header files:

```c
// From gc.h
void* gc_alloc(GarbageCollector *gc, size_t size, GCObjectType type);
//                                                  ^^^^^^^^^^^^^
//                                    This is an enum from gc.h
```

### Step 4: Apply Fix

```c
// Replace wrong name with correct name
- gc_alloc(vm->gc, 1, GC_STRING);
+ gc_alloc(vm->gc, 1, GC_TYPE_STRING);
```

## Debugging Undeclared Identifiers

### Check 1: Is It Defined?

```bash
# Search entire codebase
grep -r "GC_STRING" /home/thurtea/amlp-driver/src/

# If no results: Identifier doesn't exist
# If results in .h files: Need to include that header
# If results in other .c files: They might be wrong too!
```

### Check 2: What's Similar?

```bash
# Find similar identifiers
grep -r "GC.*STRING\|GC_.*STR" /home/thurtea/amlp-driver/src/*.h

# This would find:
# - GC_TYPE_STRING (the correct one)
# - Any other GC-related string constants
```

### Check 3: Check Git History

Maybe the function/constant was removed or renamed:

```bash
git log --all --full-history -- "*gc.h"
git blame src/gc.h
```

## Prevention

### 1. Use IDE Auto-Complete

If your editor supports it, auto-complete will only suggest valid identifiers.

### 2. Check Headers Before Using APIs

```c
// Before writing:
gc_alloc(gc, size, ???);

// Open gc.h and check the enum:
typedef enum {
    GC_TYPE_STRING,  ← Use this
} GCObjectType;
```

### 3. Compile Frequently

Don't write 500 lines then compile. Compile after small changes to catch errors early.

### 4. Grep Is Your Friend

```bash
# Always check if something exists before using it
grep -r "function_name" src/*.h
```

## Build Process

When this error occurs:

```
╔════════════════════════════════════════════════════════════════════════════╗
║                         ✗ BUILD FAILED                                     ║
╚════════════════════════════════════════════════════════════════════════════╝

Errors:
src/vm.c:871:67: error: 'GC_STRING' undeclared (first use in this function)
make: *** [Makefile:72: build/driver] Error 1
```

**No binary is created**. Must fix error and rebuild.

## Similar Errors Fixed in AMLP

### gc_strdup → gc_alloc

**Wrong**: `gc_strdup(gc, "string")`  
**Right**: Manual allocation with `gc_alloc` + `strcpy`

```c
// This function doesn't exist in our codebase
char *str = gc_strdup(gc, "hello");

// Do this instead
char *str = (char *)gc_alloc(gc, 6, GC_TYPE_STRING);
if (str) strcpy(str, "hello");
```

### gc_malloc → gc_alloc

**Wrong**: `gc_malloc(gc, size)`  
**Right**: `gc_alloc(gc, size, GC_TYPE_STRING)`

```c
// Wrong: gc_malloc doesn't exist
char *buf = gc_malloc(gc, 100);

// Correct: gc_alloc with type parameter
char *buf = (char *)gc_alloc(gc, 100, GC_TYPE_STRING);
```

### array_size → array_length

**Wrong**: `array_size(arr)`  
**Right**: `array_length(arr)`

```c
// Wrong function name
int len = array_size(my_array);

// Correct function from array.h
int len = array_length(my_array);
```

## Files Affected (All Fixed)

| File | Line | Wrong Identifier | Correct Identifier | Status |
|------|------|------------------|-------------------|--------|
| src/vm.c | 871 | `GC_STRING` | `GC_TYPE_STRING` |  Fixed |
| src/vm.c | 879 | `GC_STRING` | `GC_TYPE_STRING` |  Fixed |
| src/vm.c | 889 | `array_size` | `array_length` |  Fixed |

**Total**: 3 errors (all fixed)

## If This Happens Again

1. **Read the error message carefully** - It tells you exactly what's missing
2. **Search for similar names** - `grep -r "partial_name" src/*.h`
3. **Check the header** - Look at the API definition
4. **Look at test code** - `tests/test_*.c` shows correct usage
5. **Ask git** - `git log` or `git blame` might show renames

---

**Navigation**: [← Quick Reference](quick-reference.md) | [← README](README.md)
