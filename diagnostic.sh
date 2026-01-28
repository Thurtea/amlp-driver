#!/bin/bash
# AMLP MUD - Quick Diagnostic Check

echo "╔════════════════════════════════════════════════════════════════╗"
echo "║  AMLP MUD - System Diagnostic                                  ║"
echo "╚════════════════════════════════════════════════════════════════╝"
echo ""

cd ~/amlp-driver

echo "═══════════════════════════════════════════════════════════════"
echo " 1. File Structure Check"
echo "═══════════════════════════════════════════════════════════════"

check_file() {
    if [ -f "$1" ]; then
        echo "  ✓ $1"
        return 0
    else
        echo "  ✗ MISSING: $1"
        return 1
    fi
}

# Critical files
check_file "lib/secure/master.c"
check_file "lib/secure/simul_efun.c"
check_file "lib/std/wiztool.c"
check_file "lib/std/player.c"
check_file "lib/daemon/command.c"
check_file "lib/include/globals.h"

# Check for leftover .lpc files
echo ""
lpc_count=$(find lib -name "*.lpc" 2>/dev/null | wc -l)
if [ $lpc_count -gt 0 ]; then
    echo "  ⚠️  WARNING: Found $lpc_count .lpc files (should be .c)"
    echo "     Run: find lib -name '*.lpc' -type f"
else
    echo "  ✓ No .lpc files found (good!)"
fi

echo ""
echo "═══════════════════════════════════════════════════════════════"
echo " 2. Directory Structure"
echo "═══════════════════════════════════════════════════════════════"

check_dir() {
    if [ -d "$1" ]; then
        echo "  ✓ $1"
    else
        echo "  ✗ MISSING: $1"
    fi
}

check_dir "lib/save/players"
check_dir "lib/log"
check_dir "lib/daemon"
check_dir "lib/cmds"
check_dir "lib/std"

echo ""
echo "═══════════════════════════════════════════════════════════════"
echo " 3. Content Checks"
echo "═══════════════════════════════════════════════════════════════"

# Check if player.c has wiztool integration
if grep -q "query_wiztool" lib/std/player.c 2>/dev/null; then
    echo "  ✓ player.c has wiztool integration"
else
    echo "  ✗ player.c missing wiztool integration"
    echo "    (needs query_wiztool() function)"
fi

# Check if player.c routes commands
if grep -q "COMMAND_D" lib/std/player.c 2>/dev/null; then
    echo "  ✓ player.c routes to command daemon"
else
    echo "  ✗ player.c missing COMMAND_D routing"
    echo "    (needs to call COMMAND_D in process_input)"
fi

# Check if master.c loads daemons
if grep -q "daemon/command" lib/secure/master.c 2>/dev/null; then
    echo "  ✓ master.c loads command daemon"
else
    echo "  ✗ master.c doesn't load command daemon"
fi

echo ""
echo "═══════════════════════════════════════════════════════════════"
echo " 4. Process Check"
echo "═══════════════════════════════════════════════════════════════"

if pgrep -f "driver 3000" > /dev/null; then
    echo "  ✓ Driver is running (port 3000)"
    echo ""
    echo "  Recent log entries:"
    if [ -f lib/log/server.log ]; then
        tail -n 5 lib/log/server.log | sed 's/^/    /'
    fi
else
    echo "  ✗ Driver is NOT running"
    echo "    Start with: ./build/driver 3000"
fi

echo ""
echo "═══════════════════════════════════════════════════════════════"
echo " 5. Quick Syntax Check"
echo "═══════════════════════════════════════════════════════════════"

# Check for common syntax issues
error_count=0

if grep -n "\.lpc" lib/secure/master.c 2>/dev/null | grep -v "^Binary" | head -3; then
    echo "  ⚠️  master.c references .lpc files"
    ((error_count++))
fi

if ! grep -q "#include.*globals" lib/std/player.c 2>/dev/null; then
    echo "  ⚠️  player.c missing #include <globals.h>"
    ((error_count++))
fi

if [ $error_count -eq 0 ]; then
    echo "  ✓ No obvious syntax issues detected"
fi

echo ""
echo "═══════════════════════════════════════════════════════════════"
echo " SUMMARY"
echo "═══════════════════════════════════════════════════════════════"
echo ""

if [ -f lib/secure/master.c ] && [ -f lib/std/wiztool.c ] && [ -f lib/daemon/command.c ]; then
    echo "✓ Core files present"
    echo ""
    echo "Next steps:"
    echo "  1. If driver not running: ./build/driver 3000"
    echo "  2. Run manual test: bash /home/claude/manual_test.sh"
    echo "  3. Or run automated: python3 /home/claude/robust_mud_test.py"
else
    echo "✗ Missing critical files"
    echo ""
    echo "Need to copy implementation files:"
    echo "  cp /home/claude/command_daemon.c lib/daemon/command.c"
    echo "  cp /home/claude/wiztool.c lib/std/wiztool.c"
    echo "  cp /home/claude/master.c lib/secure/master.c"
    echo "  cp /home/claude/globals.h lib/include/globals.h"
fi

echo ""
