# Creator/Builder Filesystem Commands

**Purpose:** This directory contains filesystem and development commands used by wizard/creator wiztools. These commands are NOT directly accessible to regular players.

**Access Model:** Commands in this directory are delegated to by wiztool objects:
- `/lib/std/wiztool_admin.lpc` - Full system access (all commands)
- `/lib/std/wiztool_coder.lpc` - Development tools only  
- `/lib/std/wiztool_roleplay.lpc` - Story/building tools
- `/lib/std/wiztool_domain.lpc` - Area-specific access

## Current Commands (16)

### Directory Navigation & Management
- **cd.lpc** - Change working directory (with history support)
- **pwd.lpc** - Print working directory path
- **ls.lpc** - List files and directories (with color-coded output)
- **mkdir.lpc** - Create directories (with recursive -p option)

### File Operations
- **cat.lpc** - Display file contents
- **ed.lpc** - Line-mode LPC code editor
- **head.lpc** - Show first N lines of file (default 10)
- **tail.lpc** - Show last N lines of file (default 10)

### File Search & Analysis
- **grep.lpc** - Search files for text patterns (supports -i, -n, -r flags)
- **find.lpc** - Find files by name pattern (supports wildcards)

### Object Management
- **clone.lpc** - Clone/instantiate objects from blueprints
- **destruct.lpc** - Destroy/remove objects from memory  
- **load.lpc** - Load/compile LPC files into memory
- **update.lpc** - Recompile and reload objects (hot-reload)

### Advanced Tools
- **eval.lpc** - Execute LPC code directly (powerful debugging)
- **force.lpc** - Force another player to execute a command (admin use)

## Wiztool Integration Pattern

Commands are NOT called directly. Wiztools delegate to them:

```lpc
// Example from wiztool_coder.lpc
void cmd_ls(string dir) {
    object me = query_owner();
    
    // Add role-specific logging
    log_wizard_action(me, "ls", dir);
    
    // Delegate to shared implementation
    return call_other("/lib/cmds/creator/ls", "main", me, dir);
}
```

This allows:
- Multiple wiztool types to share command implementations
- Each wiztool to add role-specific permissions/logging
- Centralized command logic for easy maintenance
- Consistent behavior across wizard types

## Future Additions (Planned)

### [x] Phase 2 - COMPLETED (February 4, 2026)
All 5 essential filesystem tools implemented:
- [x] **grep.lpc** - Search text patterns in files
- [x] **find.lpc** - Find files by name/pattern
- [x] **mkdir.lpc** - Create directories
- [x] **head.lpc** - Show first N lines of file
- [x] **tail.lpc** - Show last N lines of file

### Phase 3 - File Operations (Priority: MEDIUM)
- **cp.lpc** - Copy files/objects
- **mv.lpc** - Move/rename files
- **rm.lpc** - Remove files
- **rmdir.lpc** - Remove directories
- **diff.lpc** - Compare two files
- **du.lpc** - Show disk/object usage

### Phase 4 - Advanced Tools (Priority: LOW)
- **snoop.lpc** - Monitor user session
- **xargs.lpc** - Batch command execution
- **locate.lpc** - Find objects in memory
- **replace.lpc** - Text replacement in files

## Architecture Notes

### Why Separate from Player Commands?

1. **Security:** These commands reveal internal game structure and code
2. **Access Control:** Only wizards/creators should use these
3. **Discoverability:** Builders find related tools together
4. **Scalability:** Can grow to 50+ creator commands without cluttering player command space

### Why NOT /lib/cmds/wizard/?

The `/wizard/` directory was removed because:
- Creator commands deserve their own clear namespace
- "Creator" is more descriptive than "wizard" for builder tools  
- Aligns with Lima mudlib's `/trans/cmds/` pattern
- Separates creator infrastructure from game content

### Comparison with Player Commands

| Aspect | Player Commands (/lib/cmds/) | Creator Commands (/lib/cmds/creator/) |
|--------|------------------------------|---------------------------------------|
| **Purpose** | Gameplay actions | Development & building |
| **Users** | All players | Wizards/creators only |
| **Examples** | attack, look, get, say | ls, cd, clone, update |
| **Security** | Public game actions | Internal system access |
| **Count** | 39 commands | 11 (growing to 50+) |

## Development Guidelines

When adding new creator commands:

1. **Place here** if command is filesystem/development related
2. **Use consistent interface** - accept owner object as first parameter
3. **Return meaningful values** - objects created, files modified, etc.
4. **Log appropriately** - especially destructive operations
5. **Document usage** - clear help text in command file
6. **Test thoroughly** - creator commands can break game systems

## Related Documentation

- `/docs/COMMAND_ORGANIZATION_ANALYSIS.md` - Comparison with other mudlibs
- `/docs/FILESYSTEM_COMMANDS_REFERENCE.md` - Lima command catalog and priorities
- `/docs/COMMAND_REORGANIZATION_PROPOSAL.md` - Why Option B was chosen
- `/docs/LS_COMMAND_CONTEXT.md` - Understanding ls.lpc ecosystem

## Migration Log

**Date:** February 4, 2026  
**Action:** Implemented Option B reorganization

**Files Moved from /wizard/ to /creator/:**
- cat.lpc
- cd.lpc
- clone.lpc
- destruct.lpc
- ed.lpc
- eval.lpc
- force.lpc
- load.lpc
- pwd.lpc
- update.lpc

**Files Moved from root to /creator/:**
- ls.lpc

**Files Moved to /admin/:**
- shutdown.lpc (from root)

**Files Removed (duplicates):**
- /admin/clone.lpc (kept in /creator/)
- /wizard/goto.lpc (kept in /admin/)

**Result:** Clean separation of creator tools from player game commands.

---

## Phase 2 Implementation Log

**Date:** February 4, 2026  
**Action:** Implemented 5 essential filesystem commands (Tier 1 priority)

**Files Created:**
- **grep.lpc** (210 lines) - Text pattern search with flags (-i, -n, -r)
- **find.lpc** (167 lines) - File/directory finder with wildcard support
- **mkdir.lpc** (110 lines) - Directory creation with recursive -p flag
- **head.lpc** (99 lines) - First N lines display (default 10)
- **tail.lpc** (114 lines) - Last N lines display (default 10, -f stub)

**Total Implementation:**
- 5 new commands
- ~700 lines of code
- All commands include help() functions
- No color codes (AMLP standard compliance)
- Complete wildcard support in find
- Recursive search in grep and mkdir

**Commands Now Total:** 16 creator tools (was 11)
