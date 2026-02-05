# AMLP Driver - Build Diagnostics System

**Click any file path below to open it in your editor**

This directory provides comprehensive documentation for all compiler warnings and errors that occur during the AMLP MUD driver build process.

##  What You'll Find Here

- **Explanations** of what each warning/error means
- **Why it matters** (or doesn't) for your MUD
- **How to fix it** with multiple solution options
- **Code examples** showing before/after fixes
- **Testing impact** - does it affect gameplay?

##  Quick Start

### When Your Build Shows Warnings

After running `make`, you'll see:

```
╠════════════════════════════════════════════════════════════════════════════╣
║  BUILD SUCCESSFUL                                                          ║
╠════════════════════════════════════════════════════════════════════════════╣
║  Files compiled: 28                                                        ║
║  Warnings:       50                                                        ║
║  Errors:         0                                                         ║
║                                                                            ║
║  Warning details: diagnostics/README.md                                    ║
║  Quick reference: diagnostics/quick-reference.md                           ║
╚════════════════════════════════════════════════════════════════════════════╝
```

**Click the file paths** above to open them directly in VS Code!

### When Your Build Fails

```
╠════════════════════════════════════════════════════════════════════════════╣
║                         X BUILD FAILED                                     ║
╠════════════════════════════════════════════════════════════════════════════╣
║  See diagnostics/undeclared-identifier.md for common compilation errors    ║
╚════════════════════════════════════════════════════════════════════════════╝
```

**Click the diagnostics path** to see solutions for common errors.

##  File Structure

```
diagnostics/
├── README.md                          ← Overview and navigation
├── quick-reference.md                 ← One-line solutions table
├── unused-parameter.md                ← -Wunused-parameter guide
├── unused-variable.md                 ← -Wunused-but-set-variable guide
├── sign-compare.md                    ← -Wsign-compare guide
├── format-truncation.md               ← -Wformat-truncation guide
├── missing-field-initializers.md      ← -Wmissing-field-initializers guide
├── address-check.md                   ← -Waddress guide
├── zero-length-format.md              ← -Wformat-zero-length guide
└── undeclared-identifier.md           ← Compilation error guide
```

##  Recommended Reading Order

### If You're New to AMLP

1. Start here: [README.md](diagnostics/README.md) - Overview of all warnings
2. Then check: [quick-reference.md](diagnostics/quick-reference.md) - Table of priorities

### If You See Specific Warnings

Just click the file path from the compilation output, or:

- **"unused parameter"** → [unused-parameter.md](diagnostics/unused-parameter.md)
- **"comparison of integer expressions"** → [sign-compare.md](diagnostics/sign-compare.md)
- **"may be truncated"** → [format-truncation.md](diagnostics/format-truncation.md)
- **"will always evaluate as 'true'"** → [address-check.md](diagnostics/address-check.md)
- **"missing initializer"** → [missing-field-initializers.md](diagnostics/missing-field-initializers.md)
- **"set but not used"** → [unused-variable.md](diagnostics/unused-variable.md)
- **"zero-length"** → [zero-length-format.md](diagnostics/zero-length-format.md)

### If Your Build Failed

Start with: [undeclared-identifier.md](diagnostics/undeclared-identifier.md)

##  Warning Priority System

Each guide has a priority indicator:

- 🟡 **Low** - Safe to ignore, cosmetic issues
- 🟠 **Medium** - Should fix eventually, potential bugs
- 🔴 **High** - Fix soon, likely bugs or inefficiencies
- ⛔ **Critical** - Must fix, blocks compilation

##  How to Use

### From Terminal Output

When you run `make`, file paths in the output are **clickable** in most terminals:

```bash
$ make
# Click any file path in the output to open it
```

VS Code, iTerm2, and most modern terminals make file paths clickable.

### From This Document

**All file paths in this documentation are markdown links** - click them to navigate:

- [diagnostics/README.md](diagnostics/README.md)
- [diagnostics/quick-reference.md](diagnostics/quick-reference.md)
- [src/vm.c](../src/vm.c)

### Search This Directory

```bash
# Find documentation for specific warning
cd diagnostics
grep -l "Wunused-parameter" *.md

# See all priority ratings
grep -h "Priority:" *.md
```

##  Current Build Status

**Last Analysis**: February 4, 2026  
**Total Warnings**: 50+  
**Warnings Documented**: All  
**Critical Errors**: 0 (all fixed)

### Breakdown by Priority

| Priority | Count | Should Fix? |
|----------|-------|-------------|
| 🟡 Low | ~30 | Optional |
| 🟠 Medium | ~15 | Recommended |
| 🔴 High | ~5 | Yes |
| ⛔ Critical | 0 | N/A (no errors) |

##  Common Fixes

### Fix All Unused Parameter Warnings

```bash
cd /home/thurtea/amlp-driver/src

# Add (void)args; to all affected functions
# See diagnostics/unused-parameter.md for details
```

### Fix All Sign Comparison Warnings

```bash
# Change int to size_t in loops
# See diagnostics/sign-compare.md for details
```

### Fix Critical Issues First

Priority order:
1. [address-check.md](diagnostics/address-check.md) - Remove redundant NULL checks
2. [format-truncation.md](diagnostics/format-truncation.md) - Increase buffer sizes
3. [missing-field-initializers.md](diagnostics/missing-field-initializers.md) - Initialize all struct fields

## 🧪 Does This Affect My MUD?

**TL;DR: Current warnings don't prevent the MUD from working.**

But fixing them:
- Prevents future bugs
- Improves code quality
- Makes real issues easier to spot
- Helps onboard new developers

See each guide's "Testing Impact" section for specifics.

##  Contributing

When adding new code:

1. **Run build** and check for warnings
2. **Check if warning is documented** in this directory
3. **Follow the guide** to fix or suppress appropriately
4. **Update documentation** if you encounter new warning types

##  Related Documentation

- [../docs/DEBUGGING_GUIDE.md](../docs/DEBUGGING_GUIDE.md) - Runtime debugging
- [../docs/COMBAT_TESTING_GUIDE.md](../docs/COMBAT_TESTING_GUIDE.md) - Feature testing
- [../README.md](../README.md) - Main project documentation

##  Tips

### Suppress Warnings Temporarily

During active development, you can reduce warning noise:

```bash
# Build with fewer warnings
make CFLAGS="-Wall -g -O2"

# Or suppress specific warnings
make CFLAGS="-Wall -Wno-unused-parameter -g -O2"
```

### Track Warning Trends

```bash
# Count warnings over time
make 2>&1 | grep -c "warning:"

# Save for comparison
make 2>&1 | grep "warning:" > warnings-$(date +%Y%m%d).txt
```

### Clickable Paths in Output

Ensure your terminal supports clickable paths:
- **VS Code Terminal**: Enabled by default
- **iTerm2**: Cmd+Click
- **GNOME Terminal**: Ctrl+Click
- **tmux**: May need configuration

## 📞 Getting Help

If you encounter warnings not covered here:

1. Check the [header files](../src/) for correct API usage
2. Search the [test files](../tests/) for examples
3. Check git history: `git log --all -- path/to/file`
4. Add new documentation to help future developers!

---

**Quick Links**: [README](diagnostics/README.md) | [Quick Reference](diagnostics/quick-reference.md) | [Main Docs](../docs/)

**Navigation**: Click any `.md` file path in this document to open it instantly!
