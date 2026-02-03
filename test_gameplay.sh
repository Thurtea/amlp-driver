#!/bin/bash

# AMLP MUD Quick Test Guide
# Tests MUD functionality after parser implementation

echo "╔════════════════════════════════════════════════╗"
echo "║     AMLP MUD - Interactive Test Guide         ║"
echo "╚════════════════════════════════════════════════╝"
echo ""

# Check if MUD is running
if ! ./mud.ctl status > /dev/null 2>&1; then
    echo "❌ MUD is not running!"
    echo "   Start it with: ./mud.ctl start"
    exit 1
fi

echo "✅ MUD is running"
echo ""

echo "╔════════════════════════════════════════════════╗"
echo "║              CONNECTION TESTS                  ║"
echo "╚════════════════════════════════════════════════╝"
echo ""

echo "Test 1: Telnet Connection"
echo "-------------------------"
echo "Command: telnet localhost 3000"
echo "Expected: Welcome screen, character creation prompt"
echo ""

echo "Test 2: Netcat Connection"
echo "-------------------------"
echo "Command: nc localhost 3000"
echo "Expected: Welcome screen, character creation prompt"
echo ""

echo "╔════════════════════════════════════════════════╗"
echo "║           CHARACTER CREATION TESTS             ║"
echo "╚════════════════════════════════════════════════╝"
echo ""

echo "Test 3: Create Regular Player"
echo "-----------------------------"
echo "1. Connect to MUD"
echo "2. Enter name: TestPlayer"
echo "3. Follow race selection prompts"
echo "4. Follow class selection prompts"
echo "5. Allocate ability scores"
echo "6. Choose skills"
echo ""

echo "Test 4: Create Wizard Character"
echo "-------------------------------"
echo "1. Connect to MUD"
echo "2. Enter name: TestWizard"
echo "3. Complete character creation"
echo "4. Test wizard commands"
echo ""

echo "╔════════════════════════════════════════════════╗"
echo "║            BASIC COMMAND TESTS                 ║"
echo "╚════════════════════════════════════════════════╝"
echo ""

echo "Test 5: Navigation Commands"
echo "---------------------------"
cat << 'EOF'
look          - View room description
exits         - Show available exits
go <dir>      - Move in direction
north/south/
east/west     - Quick movement
EOF
echo ""

echo "Test 6: Character Commands"
echo "-------------------------"
cat << 'EOF'
inventory     - View inventory
score         - View character stats
skills        - List your skills
level         - View level info
EOF
echo ""

echo "Test 7: Communication"
echo "--------------------"
cat << 'EOF'
say <msg>     - Talk in room
tell <player> - Private message
who           - List online players
emote <text>  - Perform emote
EOF
echo ""

echo "Test 8: Skills System"
echo "--------------------"
cat << 'EOF'
skills            - List all skills
practice <skill>  - Practice skill
use <skill>       - Use skill ability
check <skill>     - Roll skill check
EOF
echo ""

echo "╔════════════════════════════════════════════════╗"
echo "║            WIZARD COMMAND TESTS                ║"
echo "╚════════════════════════════════════════════════╝"
echo ""

echo "Test 9: Wizard Commands (if wizard)"
echo "-----------------------------------"
cat << 'EOF'
goto <room>       - Teleport to room
summon <player>   - Summon player
force <player>    - Force command
shutdown          - Shutdown MUD
reload <object>   - Reload object
wizhelp           - Wizard help
EOF
echo ""

echo "╔════════════════════════════════════════════════╗"
echo "║             MONITORING COMMANDS                ║"
echo "╚════════════════════════════════════════════════╝"
echo ""

echo "Monitor Logs:"
echo "-------------"
echo "  ./mud.ctl log               # Watch live log"
echo "  tail -f lib/log/server.log  # Direct log tail"
echo "  grep ERROR lib/log/server.log  # Find errors"
echo ""

echo "Check Status:"
echo "------------"
echo "  ./mud.ctl status            # Check if running"
echo "  netstat -tuln | grep 3000   # Check port"
echo "  ps aux | grep driver        # Check process"
echo ""

echo "Control Commands:"
echo "----------------"
echo "  ./mud.ctl start             # Start MUD"
echo "  ./mud.ctl stop              # Stop MUD"
echo "  ./mud.ctl restart           # Restart MUD"
echo ""

echo "╔════════════════════════════════════════════════╗"
echo "║          WHAT TO REPORT BACK                   ║"
echo "╚════════════════════════════════════════════════╝"
echo ""

cat << 'EOF'
Please test and report:

✓ Connection Results:
  - Can you connect via telnet/netcat?
  - Does welcome screen display?
  - Any connection errors?

✓ Character Creation:
  - Can you create a character?
  - Do prompts work correctly?
  - Does character save?
  - Can you log back in?

✓ Basic Commands:
  - Does 'look' show room description?
  - Does 'inventory' work?
  - Does 'score' show stats?
  - Does movement work?

✓ Skills System:
  - Can you view skills?
  - Can you practice skills?
  - Do skill checks work?
  - Any error messages?

✓ Errors in Logs:
  - Any ERROR or CRITICAL messages?
  - Any segfaults or crashes?
  - Any missing efun warnings?

✓ Performance:
  - Response time acceptable?
  - Any lag or delays?
  - Memory growing over time?
EOF

echo ""
echo "╔════════════════════════════════════════════════╗"
echo "║              QUICK TEST NOW                    ║"
echo "╚════════════════════════════════════════════════╝"
echo ""

read -p "Do you want to connect to MUD now? (y/n) " -n 1 -r
echo ""

if [[ $REPLY =~ ^[Yy]$ ]]; then
    echo ""
    echo "Connecting to AMLP MUD on localhost:3000..."
    echo "Use Ctrl+] then 'quit' to exit telnet"
    echo ""
    sleep 2
    telnet localhost 3000
fi

echo ""
echo "Test guide complete. Happy testing!"
