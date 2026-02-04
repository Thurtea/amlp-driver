# Step-by-Step Installation Guide

## Method 1: Quick Installation (Recommended)

### Step 1: Locate Your VS Code Extensions Folder

**Windows:**
```
C:\Users\YourUsername\.vscode\extensions\
```

**macOS:**
```
~/.vscode/extensions/
```

**Linux:**
```
~/.vscode/extensions/
```

### Step 2: Copy the Extension

1. Copy the entire `lpc-extension` folder into your extensions directory
2. The final path should look like:
   - Windows: `C:\Users\YourUsername\.vscode\extensions\lpc-extension\`
   - macOS/Linux: `~/.vscode/extensions/lpc-extension/`

### Step 3: Restart VS Code

Close and reopen VS Code completely (not just reload window).

### Step 4: Rename Your LPC Library Files

Rename your LPC library files from `.c` to `.lpc`:
- Before: `/mudlib/std/room.c`
- After: `/mudlib/std/room.lpc`

You can do this in bulk using:

**Linux/macOS:**
```bash
cd /path/to/your/mudlib
find . -name "*.c" -path "*/mudlib/*" -exec sh -c 'mv "$1" "${1%.c}.lpc"' _ {} \;
```

**Windows PowerShell:**
```powershell
cd C:\path\to\your\mudlib
Get-ChildItem -Recurse -Filter "*.c" | Rename-Item -NewName {$_.Name -replace '\.c$','.lpc'}
```

### Step 5: Verify It Works

1. Open VS Code
2. Open any `.lpc` file from your mudlib
3. You should see syntax highlighting for LPC constructs
4. Open a `.c` file from your driver - it should still use C highlighting

---

## Method 2: Development/Testing Mode (F5 Method)

Use this if you want to test or modify the extension before installing.

### Step 1: Open Extension in VS Code

```bash
cd /path/to/lpc-extension
code .
```

### Step 2: Launch Extension Development Host

1. Press `F5` (or go to Run → Start Debugging)
2. This opens a new "Extension Development Host" window

### Step 3: Open Your MUD Project

In the Extension Development Host window:
1. File → Open Folder
2. Navigate to your MUD project
3. Open any `.lpc` file to test syntax highlighting

### Step 4: Make Changes (Optional)

Back in the original window:
1. Edit `syntaxes/lpc.tmLanguage.json` to add custom keywords
2. Edit `package.json` to change extension settings
3. Press `Ctrl+R` (or `Cmd+R` on Mac) in the Extension Development Host to reload

---

## Method 3: Package as VSIX (For Distribution)

If you want to share your extension or install it more formally:

### Step 1: Install vsce

```bash
npm install -g @vscode/vsce
```

### Step 2: Package the Extension

```bash
cd /path/to/lpc-extension
vsce package
```

This creates `lpc-language-support-0.0.1.vsix`

### Step 3: Install the VSIX

In VS Code:
1. Press `Ctrl+Shift+P` (Cmd+Shift+P on Mac)
2. Type "Extensions: Install from VSIX"
3. Select your `.vsix` file

---

## Troubleshooting

### Extension Not Working

1. **Check file location:** Ensure the extension folder is directly in `.vscode/extensions/`
2. **Restart VS Code:** Fully quit and reopen (don't just reload)
3. **Check file extension:** Make sure your files have `.lpc` extension
4. **View installed extensions:** `Ctrl+Shift+X` and search for "LPC"

### Wrong Language Detected

1. Click the language indicator in bottom-right of VS Code
2. Select "Configure File Association for '.lpc'"
3. Choose "LPC" from the list

### Adding Custom Keywords

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

After editing, reload VS Code.

---

## Verifying Installation

### Check 1: Extensions List
1. Press `Ctrl+Shift+X`
2. Search for "LPC"
3. You should see "LPC Language Support"

### Check 2: Language Mode
1. Open a `.lpc` file
2. Look at bottom-right corner of VS Code
3. It should say "LPC" (not "C" or "Plain Text")

### Check 3: Syntax Highlighting
1. Open a `.lpc` file
2. Keywords like `inherit`, `mapping`, `mixed` should be colored
3. Function names should be highlighted
4. Comments should be colored differently

---

## Project Structure After Setup

```
your-mud-project/
├── driver/           # C source files
│   ├── main.c       # Regular C syntax
│   ├── compiler.c   # Regular C syntax
│   └── ...
└── mudlib/          # LPC library files
    ├── std/
    │   ├── room.lpc    # LPC syntax
    │   ├── object.lpc  # LPC syntax
    │   └── ...
    └── ...
```

Your `.c` files in `driver/` will use C language support.
Your `.lpc` files in `mudlib/` will use LPC language support.
