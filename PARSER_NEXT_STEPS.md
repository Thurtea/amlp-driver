# Parser Next Steps - Implementation Guide

## Priority 1: Arrays ({ })

### Syntax
```c
string *names = ({ "Alice", "Bob" });
int *numbers = ({ 1, 2, 3 });
```

### Implementation
1. Add `TOKEN_ARRAY_START` and `TOKEN_ARRAY_END` to lexer for `({` and `})`
2. Add `AST_ARRAY_LITERAL` node type
3. Parse in primary expression
4. Emit `MAKE_ARRAY` bytecode

### Test
```bash
echo 'void test() { string *x = ({ "a" }); }' > /tmp/test_array.lpc
./build/driver --parse-test /tmp/test_array.lpc
```

## Priority 2: Mappings ([ ])

### Syntax
```c
mapping data = ([ "name": "Bob", "hp": 100 ]);
```

### Implementation
1. Add `TOKEN_MAPPING_START` `([` and `TOKEN_MAPPING_END` `])`
2. Add `AST_MAPPING_LITERAL` with keys/values arrays
3. Parse key:value pairs
4. Emit `MAKE_MAPPING` bytecode

## Priority 3: Foreach

### Syntax
```c
foreach(string x in array) { }
```

### Implementation
1. Add `TOKEN_FOREACH` and `TOKEN_IN` keywords
2. Add `AST_FOREACH` node
3. Parse loop structure
4. Desugar to for loop or emit iterator bytecode

## Priority 4: Preprocessor

### Syntax
```c
#include <std.h>
#define MAX_HP 100
```

### Implementation
Add preprocessor pass before lexer (complex - may defer)
