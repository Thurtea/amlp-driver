# LPC Command Loading Debug Report
**Date:** February 2, 2026  
**Issue:** New LPC commands not loading (tell, chat, whisper, etc.)  
**Status:** ✅ ROOT CAUSE IDENTIFIED

---

## Executive Summary

**The LPC command system is NOT ACTIVE.**

The driver is running in **BYPASS MODE** where all player objects are stub pointers that never execute LPC code. Commands are processed entirely in C through fallback handlers.

---

## Investigation Results

### Phase 1: File Comparison ✅

**Working Commands:**
- `say.lpc` (2.6K, 88 lines, dated Jan 28)
- `stats.lpc` (843 bytes, 32 lines, dated Feb 2)  
- `who.lpc` (867 bytes, 34 lines, dated Jan 24)

**Non-Working Commands:**
- `tell.lpc` (1.5K, 57 lines, dated Feb 2)
- `chat.lpc` (1.0K, 44 lines, dated Feb 2)
- `exits.lpc` (1.3K, 55 lines, dated Feb 2)

**Finding:** All files have identical structure and permissions. No syntax differences detected.

### Phase 2: Compilation Testing ✅

**Server Log Analysis:**
- No compilation errors for new commands
- No `load_object` attempts logged
- No "Calling player command" debug output

**Conclusion:** Commands never reach the LPC compilation stage.

### Phase 3: Command Daemon ✅

**Verified:**
- `/lib/daemon/command.lpc` exists and has `init_commands()`
- All new commands properly registered:
  ```lpc
  commands["tell"] = "/cmds/tell";
  commands["chat"] = "/cmds/chat";
  commands["whisper"] = "/cmds/whisper";
  commands["exits"] = "/cmds/exits";
  ```
- Daemon structure is correct

**Conclusion:** Daemon is properly configured but never gets called.

### Phase 4: Driver LPC Support ✅

**Found in `src/efun.c`:**
- ✅ `efun_load_object()` is implemented (line 1120)
- ✅ `load_object()` is registered as an efun (line 1349)
- ✅ Has debug output: "[Efun] load_object: requested '%s'"

**Found in `src/driver.c`:**
- ✅ `call_player_command()` exists (line 182)
- ✅ Has debug output: "[Server] Calling player command: %s"

**But:** NO debug output appears in logs = functions never called.

### Phase 5: THE SMOKING GUN 🎯

**File:** `src/driver.c` lines 142-179

```c
void* create_player_object(const char *username, const char *password_hash) {
    if (!global_vm) return NULL;
    
    fprintf(stderr, "[Server] Creating player object for: %s\n", username);
    
    /* TODO: Implement when object system is ready */
    
    /* TEMPORARY: Bypass LPC object system until parser is fixed
     * Return a non-NULL pointer as a placeholder */
    fprintf(stderr, "[Server] BYPASS MODE: Creating stub player object\n");
    
    /* Return a placeholder pointer (just needs to be non-NULL) */
    return (void *)0x1;  // ← SENTINEL VALUE
}
```

**File:** `src/driver.c` lines 197-199

```c
VMValue call_player_command(void *player_obj, const char *command) {
    // ...
    fprintf(stderr, "[Server] Calling player command: %s\n", command);
    
    // Guard: return early if player is sentinel
    if (player_obj == (void*)1) {  // ← BYPASS CHECK
        return result;  // ← RETURNS IMMEDIATELY
    }
    
    // This code NEVER EXECUTES:
    obj_t *obj = (obj_t *)player_obj;
    result = obj_call_method(global_vm, obj, "process_command", &cmd_arg, 1);
    // ...
}
```

---

## Root Cause

### The Problem

1. **Player objects are stubs**: `create_player_object()` returns `(void*)0x1` instead of real LPC objects
2. **Commands bypass LPC**: `call_player_command()` detects the sentinel and returns immediately
3. **Fallback C handlers**: Commands fall through to C-level built-in handlers in `execute_command()`

### Why Some Commands Work

Commands like `say`, `emote`, `look`, `stats` work because they are **implemented in C** as fallback handlers in `execute_command()` (lines 442-950).

Example from driver.c:
```c
VMValue execute_command(PlayerSession *session, const char *command) {
    // ...
    
    // LPC attempt (ALWAYS FAILS due to sentinel)
    if (session->player_object) {
        result = call_player_command(session->player_object, command);
        if (result.type == VALUE_STRING) {
            return result;  // Never happens
        }
    }
    
    // FALLBACK: Built-in C commands
    if (strcmp(cmd, "say") == 0) {
        cmd_say(session, args);  // ← This is why say works!
        return result;
    }
    
    if (strcmp(cmd, "look") == 0) {
        cmd_look(session);  // ← This is why look works!
        return result;
    }
    
    // ... more C commands ...
    
    // Unknown command (new LPC commands end up here)
    send_to_char(session, "Unknown command: ");
    send_to_char(session, cmd);
    send_to_char(session, "\n");
}
```

### Why New Commands Don't Work

New commands (`tell`, `chat`, `whisper`, `exits`, etc.) are:
1. ✅ Created as LPC files
2. ✅ Registered in command daemon
3. ❌ **Never loaded** because LPC system is bypassed
4. ❌ **Not implemented in C** as fallback handlers
5. ❌ Fall through to "Unknown command" message

---

## The Architecture

```
User enters command
    ↓
execute_command() in driver.c
    ↓
Try: call_player_command()
    ↓
  Check: player_obj == (void*)0x1?
    ↓
  YES (ALWAYS) → Return NULL immediately
    ↓
Fall back to C built-in commands
    ↓
┌─ say → cmd_say() → ✅ WORKS
├─ look → cmd_look() → ✅ WORKS
├─ stats → cmd_stats() → ✅ WORKS
├─ tell → (not in C) → ❌ "Unknown command"
├─ chat → (not in C) → ❌ "Unknown command"
└─ exits → (not in C) → ❌ "Unknown command"
```

---

## Solutions

### Option 1: Implement Real Player Objects (Enables LPC) ✨

**Pros:**
- Enables full LPC scripting
- All 16 new LPC commands will work
- Future commands can be added as LPC files
- More flexible/moddable

**Cons:**
- Requires implementing the player object system
- Need to fix "parser" issues mentioned in code
- More complex, longer implementation time

**Implementation:**
1. Create `/std/player.lpc` with full implementation
2. Fix LPC parser issues
3. Implement `clone_object()` master function
4. Change `create_player_object()` to return real objects
5. Remove sentinel check from `call_player_command()`

### Option 2: Implement Commands in C (Quick Fix) 🔧

**Pros:**
- Immediate solution
- Guaranteed to work
- Fast execution
- Follows pattern of existing commands

**Cons:**
- Less flexible for modding
- Requires C programming for new commands
- Harder for non-C programmers to extend

**Implementation:**
Add to `src/driver.c` in `execute_command()`:
```c
    // After existing fallback commands
    
    if (strcmp(cmd, "tell") == 0) {
        cmd_tell(session, args);
        return result;
    }
    
    if (strcmp(cmd, "chat") == 0) {
        cmd_chat(session, args);
        return result;
    }
    
    // ... etc for all 16 commands
```

Then implement each `cmd_*()` function in C.

### Option 3: Hybrid Approach (Recommended) 🎯

**Phase 1 (Immediate):**
- Implement critical commands in C: `tell`, `chat`, `exits`, `inventory`
- Get gameplay functional quickly

**Phase 2 (Later):**
- Fix player object system
- Enable LPC for future extension
- Add LPC-based commands for advanced features

---

## Recommendation

### ✅ **Go with Hybrid Approach**

**Week 1: Implement 10 Priority Commands in C**
- Communication: `tell`, `chat`, `whisper`, `shout`
- Observation: `exits`, `examine`  
- Inventory: `inventory`, `get`, `drop`, `give`

**Week 2: Enable LPC System**
- Fix player object creation
- Enable command daemon
- Test LPC command loading

**Week 3: Add Equipment in LPC**
- `wear`, `wield`, `remove`, `unwield`, `equipment`
- More complex, benefit from LPC flexibility

---

## Next Steps

1. **Document C command implementation pattern** (see `cmd_say()`, `cmd_look()`)
2. **Implement 10 critical commands in C** (copy from LPC, convert to C)
3. **Test each command** as implemented
4. **Plan LPC player object system** for Phase 2

---

## Files Referenced

- `src/driver.c` - lines 142-179 (create_player_object), 182-230 (call_player_command), 377-950 (execute_command)
- `src/efun.c` - line 1120 (efun_load_object)
- `lib/daemon/command.lpc` - command registration
- `lib/cmds/*.lpc` - 16 new command files (ready but inactive)

---

**Status:** Investigation complete. Ready to implement solution.

**Estimated Time:**
- Option 1 (LPC): 2-3 days
- Option 2 (C only): 1 day  
- Option 3 (Hybrid): Phase 1 = 1 day, Phase 2 = 2 days

