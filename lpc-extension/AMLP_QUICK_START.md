# AMLP Quick Reference Guide

## File Organization

### Driver (C Files - Keep as .c)
```
driver/
├── compiler.c       ← C language, GCC compiled
├── lexer.c          ← C language, GCC compiled
├── parser.c         ← C language, GCC compiled
├── vm.c             ← C language, GCC compiled
├── object.c         ← C language, GCC compiled
├── gc.c             ← C language, GCC compiled
└── ...              ← All .c files here stay as .c
```

### Library (LPC Files - Rename to .lpc)
```
library/
├── master.lpc       ← LPC language, driver interpreted
├── std/
│   ├── room.lpc     ← LPC language, driver interpreted
│   ├── object.lpc   ← LPC language, driver interpreted
│   └── player.lpc   ← LPC language, driver interpreted
└── domains/
    └── start/
        └── room/
            └── void.lpc  ← LPC language, driver interpreted
```

## Quick Rename Script

### One-Time Library Conversion

**Linux/macOS:**
```bash
#!/bin/bash
# Save as: convert_to_lpc.sh

cd ~/amlp-library  # or wherever your library is

# Rename all .c files to .lpc
find . -name "*.c" -type f | while read file; do
    mv "$file" "${file%.c}.lpc"
done

echo "Conversion complete!"
```

**Windows PowerShell:**
```powershell
# Save as: convert_to_lpc.ps1

Set-Location "C:\path\to\amlp-library"

# Rename all .c files to .lpc
Get-ChildItem -Recurse -Filter "*.c" -File | ForEach-Object {
    $newName = $_.Name -replace '\.c$', '.lpc'
    Rename-Item $_.FullName -NewName $newName
}

Write-Host "Conversion complete!"
```

## Example: void.c → void.lpc

**Before (void.c):**
```c
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

**After (void.lpc):**
- Same code, just renamed
- Now gets proper LPC syntax highlighting
- VS Code recognizes it as LPC, not C
- Snippets and autocomplete work correctly

## Snippet Cheat Sheet

| Trigger | What It Creates |
|---------|----------------|
| `room` | Full room template with create() |
| `object` | Full object template with create() |
| `inherit` | inherit DIR_STD + "/base"; |
| `include` | #include <header> |
| `create` | void create() with ::create(); |
| `func` | void function_name() { } |
| `exit` | add_exit("dir", path); |
| `for` | for loop |
| `foreach` | foreach loop |
| `if` | if statement |
| `switch` | switch statement |
| `to` | this_object() |
| `tp` | this_player() |
| `clone` | clone_object(path) |

## VS Code Settings for AMLP

Add to your `.vscode/settings.json` in your workspace:

```json
{
  // Associate .lpc files with LPC language
  "files.associations": {
    "*.lpc": "lpc"
  },
  
  // Exclude build artifacts from search
  "files.exclude": {
    "**/*.o": true,
    "**/driver": true,
    "**/test_*": true
  },
  
  // Set up proper search paths
  "search.exclude": {
    "**/build": true,
    "**/tmp": true
  },
  
  // Editor settings for LPC files
  "[lpc]": {
    "editor.tabSize": 4,
    "editor.insertSpaces": true,
    "editor.formatOnSave": false,
    "editor.wordWrap": "on"
  },
  
  // C file settings (for driver code)
  "[c]": {
    "editor.tabSize": 4,
    "editor.insertSpaces": true,
    "editor.formatOnSave": false
  }
}
```

## Working with Both Projects

If you have both driver and library open:

### Workspace Setup
Create a `.code-workspace` file:

```json
{
  "folders": [
    {
      "name": "AMLP Driver (C)",
      "path": "/path/to/amlp-driver"
    },
    {
      "name": "AMLP Library (LPC)",
      "path": "/path/to/amlp-library"
    }
  ],
  "settings": {
    "files.associations": {
      "*.lpc": "lpc"
    }
  }
}
```

Save as `amlp-workspace.code-workspace` and open with VS Code.

## Language Mode Indicator

Bottom-right corner of VS Code shows current language:
- **C** - Driver files (compiler.c, vm.c, etc.)
- **LPC** - Library files (void.lpc, room.lpc, etc.)

Click it to manually change if needed.

## Common Patterns

### Room with Multiple Exits
Type `room` then Tab, modify to:
```lpc
void create() {
    ::create();
    
    set_id("town_square");
    set_short("Town Square");
    set_long(
        "You are in the bustling town square.\n" +
        "Shops line the streets in all directions.\n"
    );
    
    add_exit("north", DIR_DOMAINS + "/town/room/shop");
    add_exit("south", DIR_DOMAINS + "/town/room/tavern");
    add_exit("east", DIR_DOMAINS + "/town/room/temple");
    add_exit("west", DIR_DOMAINS + "/town/room/market");
}
```

### Object with Properties
Type `object` then Tab, modify to:
```lpc
void create() {
    ::create();
    
    set_id("sword");
    set_short("a rusty sword");
    set_long("An old sword, covered in rust.");
    set_property("weapon", 1);
    set_property("damage", 5);
    set_property("value", 10);
}
```

### Inheriting Multiple Files
```lpc
#include <globals.h>

inherit DIR_STD + "/object";
inherit DIR_STD + "/combat";

void create() {
    ::create();
    // Your code
}
```

## AMLP Driver Efuns Available

Your driver provides these 24 efuns (all highlighted):

### String Operations
- `strlen(string)` - String length
- `substring(str, start, len)` - Extract substring
- `explode(str, delim)` - Split string to array
- `implode(array, delim)` - Join array to string
- `upper_case(str)` - Convert to uppercase
- `lower_case(str)` - Convert to lowercase
- `trim(str)` - Remove whitespace

### Array Operations
- `sizeof(array)` - Get size
- `arrayp(value)` - Check if array
- `sort_array(arr)` - Sort array
- `reverse_array(arr)` - Reverse array

### Math Operations
- `abs(num)` - Absolute value
- `sqrt(num)` - Square root
- `pow(base, exp)` - Power
- `random(max)` - Random number
- `min(a, b)` - Minimum
- `max(a, b)` - Maximum

### Type Checking
- `intp(val)` - Is integer
- `floatp(val)` - Is float
- `stringp(val)` - Is string
- `objectp(val)` - Is object
- `mappingp(val)` - Is mapping

### I/O
- `write(str)` - Write to output
- `printf(fmt, ...)` - Formatted output

## Testing Your Setup

1. **Open void.lpc** (after renaming from void.c)
2. **Check bottom-right** - Should say "LPC"
3. **Type `room` and Tab** - Should expand to template
4. **Look for syntax colors:**
   - `inherit` should be highlighted
   - `::create()` - :: should be colored
   - `set_short`, `set_long` should be highlighted
   - String concatenation `+` should work

## Git Configuration

Update your `.gitignore` if needed:

```gitignore
# Driver build artifacts
driver/build/
*.o
driver
test_*

# Library temp files
library/tmp/
*.bak
*.swp

# Keep track of .lpc files, not .c in library
library/**/*.c
!library/**/*.lpc
```

## Common Issues

### "LPC not recognized"
- Restart VS Code completely
- Check extension is in `.vscode/extensions/lpc-extension/`
- Verify file has `.lpc` extension

### "Snippets not working"
- Press `Tab` not `Enter`
- Try `Ctrl+Space` to force IntelliSense

### "Mixed C and LPC highlighting"
- Check file extension is correct
- Click language indicator and select manually
- Reload window: `Ctrl+Shift+P` → "Reload Window"

## Next Steps

1.  Install extension
2.  Rename library files to .lpc
3.  Test with void.lpc
4.  Try snippets
5.  Build your MUD!

---

**Remember:** Driver = .c (compiled by GCC), Library = .lpc (interpreted by driver)
