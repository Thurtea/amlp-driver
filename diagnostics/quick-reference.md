# Build Warnings Quick Reference

**One-line solutions for every warning in the AMLP driver build**

## Quick Lookup Table

| Warning Flag | Count | Priority | One-Line Fix | Details |
|--------------|-------|----------|--------------|---------|
| `-Wunused-parameter` | 25+ | 🟡 Low | Add `(void)param;` or `__attribute__((unused))` | [→ Details](unused-parameter.md) |
| `-Wsign-compare` | 12+ | 🟠 Medium | Cast to same type or use unsigned loop vars | [→ Details](sign-compare.md) |
| `-Wformat-truncation` | 5+ | 🟠 Medium | Increase buffer size or check snprintf return | [→ Details](format-truncation.md) |
| `-Waddress` | 5+ | 🔴 High | Remove NULL checks on arrays | [→ Details](address-check.md) |
| `-Wmissing-field-initializers` | 2 | 🟠 Medium | Initialize all struct fields explicitly | [→ Details](missing-field-initializers.md) |
| `-Wunused-but-set-variable` | 2 | 🟡 Low | Remove variable or mark as `__attribute__((unused))` | [→ Details](unused-variable.md) |
| `-Wformat-zero-length` | 1 | 🔴 High | Replace empty string with NULL or use strcpy | [→ Details](zero-length-format.md) |

## By File

### src/chargen.c (Most warnings: 20+)
- **Unused parameters**: `args` in display commands (stats, skills, inventory, etc.)
- **Sign comparisons**: `NUM_RACES` comparisons in loops
- **Format truncation**: backup file path construction

**Quick fix pattern**:
```c
void cmd_stats(PlayerSession *sess, const char *args) {
    (void)args;  // Suppress unused warning
    // ... implementation
}
```

### src/skills.c (Sign comparisons: 5)
- **Issue**: `TOTAL_SKILLS` is `size_t`, loop vars are `int`
- **Quick fix**: Change loop variables to `size_t`

```c
// OLD: for (int i = 0; i < TOTAL_SKILLS; i++)
// NEW: for (size_t i = 0; i < TOTAL_SKILLS; i++)
```

### src/combat.c (Address checks: 2, Missing init: 2)
- **Issue**: `session->username` is array, not pointer
- **Quick fix**: Remove NULL check

```c
// OLD: p->name = strdup(sess && sess->username ? sess->username : "Unknown");
// NEW: p->name = strdup(sess ? sess->username : "Unknown");
```

### src/driver.c (Address checks: 4)
- **Issue**: Same as combat.c - username array checks
- **Quick fix**: Remove `&& session->username` from conditionals

### src/room.c (Address check: 1, Unused param: 1)
- **Address issue**: `room->players[i]->username` NULL check
- **Unused param**: `args` in `cmd_look`

### src/server.c (Format truncations: 2, Zero-length: 1)
- **Zero-length**: Line 119 - empty snprintf format
- **Quick fix**: Use `new_dir[0] = '\0';` instead

### src/vm.c (Fixed - 0 warnings now)
- **Was**: `GC_STRING` undeclared, `array_size` undefined
- **Fixed**: Changed to `GC_TYPE_STRING` and `array_length`

## Priority-Based Action Plan

### Phase 1: Critical Fixes (Do Now)
These prevent compilation or cause runtime bugs:
- [ ] [vm.c] Fix undeclared identifiers  **DONE**
- [ ] [server.c:119] Replace zero-length format string
- [ ] [driver.c, combat.c, room.c] Remove array NULL checks

### Phase 2: Medium Priority (Do Soon)
Improve code quality and prevent potential bugs:
- [ ] [skills.c] Fix sign comparisons with `TOTAL_SKILLS`
- [ ] [chargen.c] Fix `NUM_RACES` sign comparisons
- [ ] [combat.c] Initialize all DamageResult fields
- [ ] [chargen.c] Increase backup buffer size or validate

### Phase 3: Low Priority (Cleanup)
Code hygiene improvements:
- [ ] [chargen.c] Add `(void)args;` to 15+ display commands
- [ ] [wiz_tools.c] Mark unused params in stub functions
- [ ] [psionics.c, magic.c] Mark `target_name` unused

## Makefile Integration

The build system shows:
```
╠════════════════════════════════════════════════════════════════════════════╣
║  BUILD SUCCESSFUL                                                          ║
╠════════════════════════════════════════════════════════════════════════════╣
║  Files compiled: 28                                                        ║
║  Warnings:       50                                                        ║
║  Errors:         0                                                         ║
╚════════════════════════════════════════════════════════════════════════════╝
```

To see which warnings matter most: `make 2>&1 | grep "error:"` (0 results = good)

## Testing Impact

**Do warnings affect gameplay?**

| Warning Type | Affects MUD? | Severity |
|--------------|--------------|----------|
| Unused parameters |  No | Safe |
| Sign comparisons |  Rare edge cases | Loop may terminate early if negative |
| Format truncation |  Path too long | File operations might fail |
| Address checks |  No | Redundant but harmless |
| Missing initializers |  Yes | Fields may have garbage values |
| Zero-length format |  No | Just inefficient |

**Bottom line**: The current 50 warnings won't prevent the MUD from running, but fixing Phase 1 and Phase 2 items prevents future bugs.

## Search by Warning Message

**"unused parameter"** → [unused-parameter.md](unused-parameter.md)
**"comparison of integer expressions of different signedness"** → [sign-compare.md](sign-compare.md)
**"may be truncated"** → [format-truncation.md](format-truncation.md)
**"will always evaluate as 'true'"** → [address-check.md](address-check.md)
**"missing initializer for field"** → [missing-field-initializers.md](missing-field-initializers.md)
**"set but not used"** → [unused-variable.md](unused-variable.md)
**"zero-length"** → [zero-length-format.md](zero-length-format.md)

---

**Navigation**: [← README](README.md) | [↑ Top](#build-warnings-quick-reference)
