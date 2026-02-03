#!/bin/bash

# Quick Test Guide for Wizard Commands
# Run this after logging in as an admin user

cat << 'EOF'
╔═══════════════════════════════════════════════════════════════╗
║       WIZARD COMMAND TEST GUIDE - Quick Reference             ║
╚═══════════════════════════════════════════════════════════════╝

== CONNECTION ==
nc localhost 3000
OR
telnet localhost 3000

== LOGIN ==
Name: thurtea (or your admin username)
Password: [your password]

== TEST COMMANDS (Copy/Paste These) ==

# 1. Load Wiztool
wiztool

# 2. Show Help
wiz help

# 3. List Root Directory
wiz ls /

# 4. List Commands Directory
wiz ls /cmds

# 5. List Admin Commands
wiz ls /cmds/admin

# 6. Change Directory
wiz cd /domains

# 7. Show Current Directory
wiz pwd

# 8. Go Back to Root
wiz cd /

# 9. Show Current Directory Again
wiz pwd

# 10. Teleport to Start Room
goto /domains/start

# 11. Look Around
look

# 12. Clone an Object
clone /std/object

# 13. Check Your Inventory
inventory

# 14. Evaluate LPC Code
wiz eval 1 + 1

# 15. Evaluate String Operation
wiz eval "Hello " + "World"

# 16. List All Loaded Objects
wiz objects

# 17. Show File Contents
wiz cat /master.lpc

# 18. Show Current Stats
stats

# 19. Show Who's Online
who

# 20. Test Help System
help

╔═══════════════════════════════════════════════════════════════╗
║                    EXPECTED RESULTS                           ║
╚═══════════════════════════════════════════════════════════════╝

✅ wiztool       → "You load your wiztool. Type 'wiz help'..."
✅ wiz help      → Shows full command list
✅ wiz ls /      → Lists directories (cmds, daemon, domains, etc.)
✅ wiz cd /dir   → "Changed directory to: /dir"
✅ wiz pwd       → "Current directory: [path]"
✅ goto /room    → "Teleporting to..."
✅ clone /obj    → "Cloned: /std/object#12345"
✅ wiz eval      → "Result: [value]"
✅ wiz objects   → Lists all loaded objects
✅ wiz cat       → Shows file contents

╔═══════════════════════════════════════════════════════════════╗
║                  TROUBLESHOOTING                              ║
╚═══════════════════════════════════════════════════════════════╝

❌ "Unknown command: wiztool"
   → MUD needs restart: ./mud.ctl restart

❌ "You don't have permission"
   → Check privilege level: stats
   → Should show "Admin" or privilege 1+

❌ "Wiztool not loaded"
   → First run: wiztool
   → Then use: wiz help

❌ "Command not found"
   → Check command syntax
   → Use: help
   → Use: wiz help

╔═══════════════════════════════════════════════════════════════╗
║                   QUICK REFERENCE                             ║
╚═══════════════════════════════════════════════════════════════╝

== FILESYSTEM ==
wiz ls [dir]         - List files
wiz cd <dir>         - Change directory
wiz pwd              - Show current directory
wiz cat <file>       - View file
wiz mkdir <dir>      - Create directory
wiz rm <file>        - Delete file

== OBJECTS ==
clone <file>         - Create object (shortcut)
wiz clone <file>     - Create object (via wiztool)
wiz dest <obj>       - Destroy object
wiz update <file>    - Reload object file
wiz load <file>      - Load object
wiz objects          - List all objects

== DEVELOPMENT ==
wiz eval <code>      - Run LPC code
wiz call <obj> <fn>  - Call function
wiz trace <n>        - Set debug level
wiz errors           - Show error log

== NAVIGATION ==
goto <room>          - Teleport (shortcut)
wiz goto <room>      - Teleport (via wiztool)

== INFO ==
stats                - Your stats
who                  - Online players
inventory            - Your items
look                 - Look around
help                 - Command help

╔═══════════════════════════════════════════════════════════════╗
║                  EXAMPLE WORKFLOW                             ║
╚═══════════════════════════════════════════════════════════════╝

# Build a new room
wiz cd /domains/test
wiz clone /std/room
wiz eval write("Testing!")
wiz ls
goto /domains/test/room1
look

# Create test object
wiz cd /domains/test/obj
clone /std/object
inventory
wiz stat /domains/test/obj/object1

# Update code
wiz update /std/object
wiz eval 2 + 2

# Explore
wiz cd /
wiz ls
wiz cat /master.lpc

EOF
