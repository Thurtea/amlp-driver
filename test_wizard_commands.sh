#!/bin/bash

# Test wizard commands
# This script tests the newly registered wizard/admin commands

echo "╔════════════════════════════════════════════════╗"
echo "║     AMLP MUD - Wizard Command Test            ║"
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

echo "Testing wizard commands via telnet..."
echo ""

# Create test script
cat > /tmp/test_wizard_cmds.txt << 'EOF'
help
wiztool
wiz help
wiz ls /
wiz pwd
wiz cd /domains
wiz pwd
goto /domains/start
look
clone /std/object
inventory
quit
EOF

echo "Test commands to send:"
echo "---"
cat /tmp/test_wizard_cmds.txt
echo "---"
echo ""

echo "Connecting and testing (this will take ~10 seconds)..."
echo ""

# Run test with timeout
timeout 15 bash -c '(
    sleep 1
    echo ""  # Empty for name prompt  
    sleep 1
    cat /tmp/test_wizard_cmds.txt
    sleep 10
) | telnet localhost 3000' 2>&1 | tee /tmp/wizard_test_output.txt

echo ""
echo "╔════════════════════════════════════════════════╗"
echo "║            Test Results Analysis               ║"
echo "╚════════════════════════════════════════════════╝"
echo ""

# Check for key success indicators
if grep -q "wiztool" /tmp/wizard_test_output.txt; then
    echo "✅ wiztool command recognized"
else
    echo "❌ wiztool command NOT recognized"
fi

if grep -q "unknown command.*wiz" -i /tmp/wizard_test_output.txt; then
    echo "❌ wiz command NOT recognized"
else
    echo "✅ wiz command appears to be working"
fi

if grep -q "Wiztool" /tmp/wizard_test_output.txt; then
    echo "✅ Wiztool functionality appears active"
else
    echo "⚠️  Wiztool may not be loading"
fi

if grep -q "goto" /tmp/wizard_test_output.txt; then
    echo "✅ goto command recognized"
else
    echo "⚠️  goto command may not be working"
fi

if grep -q "clone" /tmp/wizard_test_output.txt; then
    echo "✅ clone command recognized"
else
    echo "⚠️  clone command may not be working"
fi

echo ""
echo "Full output saved to: /tmp/wizard_test_output.txt"
echo "Review with: less /tmp/wizard_test_output.txt"
echo ""

# Cleanup
rm -f /tmp/test_wizard_cmds.txt

echo "Test complete!"
