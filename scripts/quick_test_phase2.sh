#!/bin/bash
# Quick Start Test Script for Phase 2 Features
# Usage: ./quick_test_phase2.sh

echo "================================================="
echo "🎮 AMLP Phase 2 Feature Testing"
echo "================================================="
echo ""
echo "Starting driver on ports 3000 (telnet) and 3001 (websocket)..."
echo ""

cd /home/thurtea/amlp-driver

# Check if driver exists
if [ ! -f "./build/driver" ]; then
    echo "❌ Driver not found! Compiling..."
    make clean && make
    if [ $? -ne 0 ]; then
        echo "❌ Compilation failed!"
        exit 1
    fi
fi

# Kill any existing driver instances
pkill -f "./build/driver" 2>/dev/null
sleep 1

# Start driver in background
./build/driver 3000 3001 &
DRIVER_PID=$!

echo "✅ Driver started (PID: $DRIVER_PID)"
echo ""
echo "================================================="
echo "📋 TESTING FEATURES:"
echo "================================================="
echo ""
echo "🏊 Swimming System:"
echo "   - Go to: /domains/start/village_center"
echo "   - Then: south (to lake shore)"
echo "   - Try: swim lake"
echo ""
echo "⚔️ Techno-Wizard Hilt:"
echo "   - Swim across lake to far shore"
echo "   - Command: get hilt"
echo "   - Command: charge hilt with 10 ppe"
echo "   - Command: activate hilt"
echo ""
echo "🦅 Hawk Companion:"
echo "   - Go to: /domains/castle/entry"
echo "   - Command: say hawk follow"
echo "   - Command: say hawk fly"
echo "   - Command: say hawk hunt"
echo ""
echo "🐕 Spike the Dog Boy:"
echo "   - At castle entry"
echo "   - Command: talk to spike"
echo "   - Command: attack spike (combat test)"
echo ""
echo "✨ Magic Spells:"
echo "   - cast swim as a fish"
echo "   - cast read aura spike"
echo "   - cast armor of ithan"
echo "   - cast fireball spike"
echo ""
echo "🧠 Psionic Powers:"
echo "   - manifest see aura spike"
echo "   - manifest psi sword"
echo ""
echo "🐉 Dragon Metamorph:"
echo "   - Command: setocc great_horned_dragon"
echo "   - Command: metamorph human"
echo "   - Command: metamorph revert"
echo "   - Test: cast read aura <name> (while disguised)"
echo ""
echo "================================================="
echo "🔧 WIZARD SETUP COMMANDS:"
echo "================================================="
echo ""
echo "@goto /domains/start/village_center"
echo "@setocc ley_line_walker"
echo "@grantskill swimming 75"
echo "@grantskill falconry 80"
echo "@me->set_max_ppe(100)"
echo "@me->set_ppe(100)"
echo "@me->set_max_isp(50)"
echo "@me->set_isp(50)"
echo ""
echo "================================================="
echo "🌐 CONNECTION:"
echo "================================================="
echo ""
echo "Telnet: telnet localhost 3000"
echo "Websocket: ws://localhost:3001"
echo ""
echo "📖 Full testing guide: docs/PHASE2_TESTING_GUIDE.md"
echo "📊 Complete summary: docs/PHASE2_COMPLETE_SUMMARY.md"
echo ""
echo "================================================="
echo "⚡ Press Ctrl+C to stop the driver"
echo "================================================="
echo ""

# Wait for driver to exit
wait $DRIVER_PID
