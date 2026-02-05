# LPC Language Support for VS Code

Comprehensive language support for LPC (Lars Pensjö C) files, specifically designed for the AMLP MUD Driver project.

## Overview

This extension provides syntax highlighting, code snippets, and language features for `.lpc` files used in MUD (Multi-User Dungeon) development. It's optimized for the AMLP Driver, which uses `.lpc` extensions for library files to distinguish them from the C driver code.

**Features:**
- Full syntax highlighting for LPC keywords and constructs
- Code snippets for rooms, objects, NPCs, and common patterns
- Support for AMLP driver built-in functions (efuns)
- Bracket matching, auto-closing, and code folding
- Comment toggling support

## Installation

### Quick Install

1. Copy the extension to VS Code:
   ```bash
   cp -r lpc-extension ~/.vscode/extensions/
   ```

2. Restart VS Code

3. Open any `.lpc` file - syntax highlighting activates automatically!

### Development Mode

1. Open `lpc-extension` folder in VS Code
2. Press `F5` to launch Extension Development Host
3. Test with your AMLP project

## Usage with AMLP Driver

The AMLP Driver uses two file types:

```
amlp-driver/
├── src/              # C files (.c) - Driver implementation
│   ├── lexer.c       # Uses standard C highlighting
│   ├── parser.c      # Uses standard C highlighting
│   └── vm.c          # Uses standard C highlighting
└── lib/              # LPC files (.lpc) - Game library
    ├── master.lpc    # Uses LPC highlighting ✓
    ├── std/
    │   ├── room.lpc  # Uses LPC highlighting ✓
    │   └── player.lpc # Uses LPC highlighting ✓
    └── domains/
```

**Note:** C files (`.c`) automatically use VS Code's built-in C/C++ highlighting. This extension only activates for `.lpc` files.

## Code Snippets

Type these prefixes and press `Tab` to expand:

### Room Creation
**Trigger:** `room`
```lpc
inherit "/std/room";

void create() {
    ::create();
    set_short("Room Title");
    set_long("Room description.");
    add_exit("north", "/path/to/room");
}
```

### Object/NPC Creation  
**Trigger:** `object`
```lpc
inherit "/std/object";

void create() {
    ::create();
    set_short("a short description");
    set_long("A longer description.");
}
```

### Function Templates
- `func` - Void function
- `funcr` - Function with return value
- `create` - Create function with super call

### Control Structures  
- `if` - If statement
- `for` - For loop
- `foreach` - Foreach loop
- `switch` - Switch statement
- `while` - While loop

### Common Operations
- `inherit` - Inherit statement
- `include` - Include statement
- `to` - this_object()
- `tp` - this_player()
- `clone` - clone_object()
- `call` - call_other()

## Syntax Highlighting

### LPC Keywords
**Control:** if, else, while, for, foreach, switch, case, default, break, continue, return

**Modifiers:** static, private, protected, public, varargs, nomask, nosave, virtual

**Special:** inherit, include

**Operators:** :: (scope resolution), ->, +=, -=, *=, /=

### AMLP Driver Built-in Functions
The extension recognizes all AMLP driver efuns:

**String:** strlen, substring, explode, implode, upper_case, lower_case, trim

**Array:** sizeof, arrayp, sort_array, reverse_array

**Math:** abs, sqrt, pow, random, min, max

**Type Checking:** intp, floatp, stringp, objectp, mappingp

**Object:** clone_object, destruct, call_other, this_object, this_player

**I/O:** write, printf, read_file, write_file

## File Association

This extension automatically activates for files with the `.lpc` extension. If you have existing files with `.c` extension that contain LPC code, rename them:

**Linux/macOS:**
```bash
cd /path/to/amlp-library
find . -name "*.c" -type f -exec sh -c 'mv "$1" "${1%.c}.lpc"' _ {} \;
```

**Windows PowerShell:**
```powershell
cd C:\path\to\amlp-library
Get-ChildItem -Recurse -Filter "*.c" | Rename-Item -NewName {$_.Name -replace '\.c$','.lpc'}
```

## Language Configuration

The extension provides:
- Line comments: `//`
- Block comments: `/* */`
- Auto-closing brackets: `()`, `[]`, `{}`
- Surrounds: Automatic wrapping with brackets and quotes

## Benefits of .lpc Extension

✓ **Clear separation** between C driver code and LPC library code  
✓ **Proper highlighting** for LPC-specific constructs  
✓ **Better tooling** and IDE integration  
✓ **Industry standard** for LPC MUD development  
✓ **Prevents confusion** when working with both C and LPC files

## Troubleshooting

### Extension Not Working
1. Verify extension is in `~/.vscode/extensions/lpc-extension/`
2. Restart VS Code completely
3. Check file has `.lpc` extension (not `.c`)
4. Look for "LPC" in the bottom-right language indicator

### Snippets Not Expanding
1. Type the snippet trigger word (e.g., `room`)
2. Press `Tab` (not Enter)
3. If nothing happens, press `Ctrl+Space` to show IntelliSense

### Wrong Syntax Highlighting
1. Click language indicator in bottom-right corner
2. Select "LPC" from the dropdown
3. To remember choice: "Configure File Association for '.lpc'"

## AMLP Driver Compatibility  

This extension is specifically designed for:

void create() {
    ::create();
    
    set_id("object_id");
    set_short("a short description");
    set_long("A longer description.");
}
```

### Other Snippets
- `inherit` - Add inherit statement
- `include` - Add include statement
- `create` - Create function with super call
- `func` - Void function template
- `funcr` - Function with return value
- `exit` - Add room exit
- `if`, `for`, `foreach`, `switch` - Control structures
- `mapping`, `array` - Data structure declarations
- `to` - this_object()
- `tp` - this_player()
- `clone` - clone_object()
- `call` - call_other()

## Syntax Highlighting

### LPC-Specific Keywords
- **Control:** if, else, while, for, foreach, switch, etc.
- **Modifiers:** static, private, protected, public, varargs, nomask, nosave
- **Special:** inherit, include
- **Operators:** :: (scope resolution)

### AMLP Driver Efuns
All 24 built-in functions from your driver are recognized:

**String Functions:** strlen, substring, explode, implode, upper_case, lower_case, trim

**Array Functions:** sizeof, arrayp, sort_array, reverse_array

**Math Functions:** abs, sqrt, pow, random, min, max

**Type Checking:** intp, floatp, stringp, objectp, mappingp

**I/O Functions:** write, printf

### Common Room/Object Functions
Functions like `set_short`, `set_long`, `set_id`, `add_exit`, `query_short`, `query_long` are highlighted as they're commonly used in MUD development.

## Customization

### Adding Your Own Keywords

Edit `syntaxes/lpc.tmLanguage.json`:

```json
"keywords": {
  "patterns": [
    {
      "name": "keyword.control.lpc",
      "match": "\\b(if|else|your_custom_keyword)\\b"
    }
  ]
}
```

### Adding Custom Snippets

Edit `snippets/lpc.json`:

```json
"Your Snippet Name": {
  "prefix": "trigger",
  "body": [
    "code line 1",
    "code line 2"
  ],
  "description": "What this snippet does"
}
```

## Project Integration

### Renaming Files in Bulk

**Linux/macOS:**
```bash
cd /path/to/your/amlp-library
find . -name "*.c" -type f -exec sh -c 'mv "$1" "${1%.c}.lpc"' _ {} \;
```

**Windows PowerShell:**
```powershell
cd C:\path\to\your\amlp-library
Get-ChildItem -Recurse -Filter "*.c" | Rename-Item -NewName {$_.Name -replace '\.c$','.lpc'}
```

**Note:** Only rename files in your library directory, not your driver!

## Why .lpc Extension?

**Benefits:**
-  Clear separation between C driver code and LPC library code
-  Proper syntax highlighting for LPC-specific constructs
-  No confusion between what's compiled (C) vs interpreted (LPC)
-  Better tooling support and IDE features
-  Follows modern best practices for language-specific extensions

## Troubleshooting

### Extension Not Activating
1. Verify the extension is in `.vscode/extensions/lpc-extension/`
2. Restart VS Code completely (not just reload window)
3. Check file extension is `.lpc` not `.c`

### Snippets Not Working
1. Type the trigger word (e.g., `room`)
2. Press `Tab` (not Enter)
3. If no suggestions appear, press `Ctrl+Space` to trigger IntelliSense

### Wrong Language Selected
1. Click the language indicator in bottom-right
2. Select "LPC" from the list
3. Or use "Configure File Association for '.lpc'" to make it permanent

## AMLP Driver Compatibility  

This extension is specifically designed for:
- **AMLP Driver** - Modern LPC MUD driver written from scratch in C
- **AMLP Library** - Standard LPC mudlib with complete game systems  
- **Repository:** https://github.com/Thurtea/amlp-driver

All syntax highlighting, snippets, and efuns match the AMLP Driver implementation.

## Contributing

Contributions welcome:
- Additional LPC snippets
- Enhanced syntax patterns  
- Driver-specific features
- Documentation improvements

## License

MIT License - Free for AMLP and other LPC MUD projects

---

**Version:** 1.0.0 | **Author:** AMLP Project | **Language:** LPC

