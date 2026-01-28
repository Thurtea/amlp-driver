#!/bin/bash
# AMLP MUD - Manual Interactive Test Script

echo "╔════════════════════════════════════════════════════════════════╗"
echo "║     AMLP MUD - Manual Testing Guide                            ║"
echo "╚════════════════════════════════════════════════════════════════╝"
echo ""

# Check if driver is running
if ! pgrep -f "driver 3000" > /dev/null; then
    echo "⚠️  Warning: Driver doesn't appear to be running"
    echo ""
    echo "Start it with:"
    echo "  cd ~/amlp-driver"
    echo "  ./build/driver 3000"
    echo ""
    read -p "Press Enter once driver is started..."
fi

echo "This script will guide you through testing the MUD manually."
echo ""
echo "═══════════════════════════════════════════════════════════════"
echo " TEST 1: Check Server Startup Logs"
echo "═══════════════════════════════════════════════════════════════"
echo ""
echo "Look at your driver terminal. You should see:"
echo "  ✓ [Server] Loading master object: lib/secure/master.c"
echo "  ✓ Master object initializing..."
echo "  ✓ Simul_efun loaded."
echo "  ✓ Loaded: /daemon/command"
echo "  ✓ Master object initialized successfully."
echo ""
echo "You should NOT see:"
echo "  ✗ [WARNING] Cannot open master"
echo "  ✗ [INFO] Server will run with limited functionality"
echo ""
read -p "Did the master object load successfully? (y/n): " answer
if [ "$answer" != "y" ]; then
    echo ""
    echo "❌ Master object didn't load. Check:"
    echo "   1. Does lib/secure/master.c exist?"
    echo "   2. Any syntax errors in startup logs?"
    echo "   3. File permissions correct?"
    exit 1
fi

echo ""
echo "✓ Master object loaded!"
echo ""

echo "═══════════════════════════════════════════════════════════════"
echo " TEST 2: Admin Login & Wiztool"
echo "═══════════════════════════════════════════════════════════════"
echo ""
echo "Now we'll test admin login. I'll open telnet for you."
echo "Follow these steps:"
echo ""
echo "1. When prompted 'Enter your name:', type: AdminTest"
echo "2. Set a password (it won't show as you type)"
echo "3. Confirm the password"
echo "4. Watch for these messages:"
echo "   - 'As the first player, you have been granted Admin privileges.'"
echo "   - 'Wiztool attached.'"
echo "   - 'Type wiz help for wizard commands.'"
echo ""
read -p "Press Enter to connect as admin..."

# Create test commands file
cat > /tmp/admin_test_commands.txt << 'EOF'
# Copy and paste these commands one by one:

# First, verify you're admin
stats

# Test wiztool help
wiz help

# Test filesystem commands
pwd
ls
cd /lib
pwd
ls
cd std
pwd
ls

# Test object commands  
eval 2 + 2
eval "hello" + " world"

# Try to clone something
clone /std/object
inventory
dest object

# Standard commands should also work
who
look
say Testing admin commands

# When done
quit
EOF

echo ""
echo "╔════════════════════════════════════════════════════════════════╗"
echo "║  Commands file created: /tmp/admin_test_commands.txt           ║"
echo "║  You can copy/paste from it or type manually                   ║"
echo "╚════════════════════════════════════════════════════════════════╝"
echo ""
echo "Opening telnet connection now..."
echo ""

telnet localhost 3000

echo ""
echo "═══════════════════════════════════════════════════════════════"
echo " TEST 2 CHECKLIST - Did you see:"
echo "═══════════════════════════════════════════════════════════════"
echo ""
read -p "✓ 'Wiztool attached' message on login? (y/n): " wiz_attached
read -p "✓ 'wiz help' showed wiztool commands? (y/n): " wiz_help
read -p "✓ 'pwd' showed current directory? (y/n): " pwd_works
read -p "✓ 'ls' listed directory contents? (y/n): " ls_works
read -p "✓ 'cd /lib' changed directory? (y/n): " cd_works
read -p "✓ 'eval 2 + 2' returned 'int: 4'? (y/n): " eval_works
read -p "✓ 'clone /std/object' created object? (y/n): " clone_works

echo ""
echo "═══════════════════════════════════════════════════════════════"
echo " TEST 3: Normal Player (Should NOT have wiztool)"
echo "═══════════════════════════════════════════════════════════════"
echo ""
echo "Now we'll test a normal player to verify they DON'T get wiztool."
echo ""
echo "1. When prompted 'Enter your name:', type: NormalPlayer"
echo "2. Set a password"
echo "3. You should NOT see 'Wiztool attached'"
echo ""
read -p "Press Enter to connect as normal player..."

cat > /tmp/player_test_commands.txt << 'EOF'
# Test these commands:

# Should work
who
look
say Hello
stats

# Should NOT work (no wiztool)
wiz help
cd /lib
ls
clone /std/object
eval 2 + 2

# When done
quit
EOF

echo ""
echo "Commands file: /tmp/player_test_commands.txt"
echo "Opening telnet connection now..."
echo ""

telnet localhost 3000

echo ""
echo "═══════════════════════════════════════════════════════════════"
echo " TEST 3 CHECKLIST - Did you see:"
echo "═══════════════════════════════════════════════════════════════"
echo ""
read -p "✓ NO 'Wiztool attached' message on login? (y/n): " no_wiz
read -p "✓ 'wiz help' returned 'Unknown command'? (y/n): " wiz_denied
read -p "✓ 'cd /lib' returned 'Unknown command'? (y/n): " cd_denied
read -p "✓ Standard commands (who, look, say) worked? (y/n): " std_works

echo ""
echo "═══════════════════════════════════════════════════════════════"
echo " FINAL SUMMARY"
echo "═══════════════════════════════════════════════════════════════"
echo ""

# Count successes
success=0
total=11

[ "$wiz_attached" = "y" ] && ((success++))
[ "$wiz_help" = "y" ] && ((success++))
[ "$pwd_works" = "y" ] && ((success++))
[ "$ls_works" = "y" ] && ((success++))
[ "$cd_works" = "y" ] && ((success++))
[ "$eval_works" = "y" ] && ((success++))
[ "$clone_works" = "y" ] && ((success++))
[ "$no_wiz" = "y" ] && ((success++))
[ "$wiz_denied" = "y" ] && ((success++))
[ "$cd_denied" = "y" ] && ((success++))
[ "$std_works" = "y" ] && ((success++))

echo "Test Results: $success/$total passed"
echo ""

if [ $success -eq $total ]; then
    echo "╔════════════════════════════════════════════════════════════════╗"
    echo "║  ✅ SUCCESS - ALL TESTS PASSED!                                ║"
    echo "║                                                                ║"
    echo "║  Your MUD is working correctly:                                ║"
    echo "║  • Master object loads                                         ║"
    echo "║  • Command system works                                        ║"
    echo "║  • Wiztool attaches to admins                                  ║"
    echo "║  • All wiztool commands functional                             ║"
    echo "║  • Normal players correctly restricted                         ║"
    echo "╚════════════════════════════════════════════════════════════════╝"
elif [ $success -ge 8 ]; then
    echo "╔════════════════════════════════════════════════════════════════╗"
    echo "║  ⚠️  MOSTLY WORKING - Minor issues                             ║"
    echo "║                                                                ║"
    echo "║  Most features work but some commands may need fixes.          ║"
    echo "╚════════════════════════════════════════════════════════════════╝"
else
    echo "╔════════════════════════════════════════════════════════════════╗"
    echo "║  ❌ ISSUES DETECTED                                            ║"
    echo "║                                                                ║"
    echo "║  Several features aren't working. Check:                       ║"
    echo "║  1. Master object loaded correctly?                            ║"
    echo "║  2. player.c updated with wiztool integration?                 ║"
    echo "║  3. Command daemon loaded?                                     ║"
    echo "║  4. All .lpc files renamed to .c?                              ║"
    echo "╚════════════════════════════════════════════════════════════════╝"
fi

echo ""
echo "Logs saved to:"
echo "  /tmp/admin_test_commands.txt"
echo "  /tmp/player_test_commands.txt"
echo ""
