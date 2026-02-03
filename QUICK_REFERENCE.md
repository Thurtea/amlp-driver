# AMLP Driver - Phase 1 & 2 Quick Reference

## 🎮 Gameplay Commands (20+ Available)

### Communication (4 commands)
- **chat** - Message all players in the game
- **tell** - Private message to a specific player  
- **whisper** - Message to players in your room
- **shout** - Loud message to room

### Observation (2 commands)
- **exits** - Show available exits
- **examine** - Inspect a player or object

### Inventory (4 commands)
- **inventory** - Show what you're carrying
- **get** - Pick up an item
- **drop** - Drop an item
- **give** - Give item to another player

### System Commands (5+ commands)
- **stats** - Show your character stats
- **who** - List players online
- **say** - Speak a message
- **look** - Describe current room
- **emote** - Perform an action

---

## 🏗️ System Architecture

**Command Flow:**
```
Input → LPC player_simple.lpc → Returns 0
     ↓
     → C Fallback Commands (Phase 1)
     ↓
     → Output to Player
```

**Login Flow:**
```
Player Login
  ↓
Load character data from disk
  ↓
Call efun_clone_object("/std/player_simple")
  ↓
Create real LPC player object
  ↓
Attach to session
  ↓
Ready for commands
```

---

## 📊 Implementation Status

| Phase | Feature | Status |
|-------|---------|--------|
| 1 | 10 Priority Commands | ✅ COMPLETE |
| 1 | C Command Implementation | ✅ COMPLETE |
| 2 | LPC Infrastructure | ✅ COMPLETE |
| 2 | Player Objects | ✅ COMPLETE |
| 2 | Hybrid Architecture | ✅ COMPLETE |
| 3 | Full LPC Commands | 🔄 READY (needs parser) |
| 3 | Advanced Parser Syntax | 🔄 FUTURE |

---

## 🚀 Running the Server

```bash
cd ~/amlp-driver

# Start server
./mud.ctl start

# Check status
./mud.ctl status

# Stop server
./mud.ctl stop

# View logs
tail -f lib/log/server.log
```

---

## 🧪 Testing

```bash
# Test single command
(echo "thurtea"; sleep 1; echo "password"; sleep 2; echo "chat Test!"; sleep 1; echo "quit") | nc localhost 3000

# Check LPC object creation
tail -30 lib/log/server.log | grep "player_simple\|clone_object"

# Verify no errors
tail -30 lib/log/server.log | grep -i "error\|fatal"
```

---

## 📁 Key Files

**Source Code:**
- `src/driver.c` - Main driver with LPC integration
- `lib/std/player_simple.lpc` - Player object (LPC)
- `src/efun.c` - efun_clone_object() implementation

**Documentation:**
- `PHASE1_COMMAND_IMPLEMENTATION_COMPLETE.md` - Phase 1 details
- `PHASE2_COMPLETION_REPORT.md` - Phase 2 architecture
- `lib/log/server.log` - Runtime logs

---

## 🎯 Production Readiness

✅ **Stable:** Multiple login cycles tested  
✅ **Functional:** All commands working  
✅ **Secure:** Character data saved/restored  
✅ **Tested:** No crashes or memory leaks  
✅ **Documented:** Everything recorded  
✅ **Versioned:** All commits tracked  

---

## 🔮 Future Enhancements

**When parser supports advanced syntax:**
1. Replace player_simple.lpc with full player.lpc
2. Enable 16 LPC commands via command daemon
3. Add complete LPC extensibility

**Estimated effort:** 1-2 days of parser work  
**Breaking changes:** Zero

---

## 📞 Support

**Issues:**
- Check `lib/log/server.log` for errors
- Verify player saved data: `ls lib/save/players/`
- Test login with existing character

**Common Commands:**
```
who              - See who's online
stats            - Check your stats
inventory        - View inventory
chat Hello!      - Send global message
tell foo Hello   - Private message to foo
exits            - Show exits
examine room     - Look at surroundings
```

---

**Last Updated:** February 2, 2026  
**Status:** PRODUCTION READY  
**Next Phase:** Parser Enhancement (Optional)
