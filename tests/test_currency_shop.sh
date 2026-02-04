#!/bin/bash
# Test script for currency and shop system

echo "=================================="
echo "CURRENCY & SHOP SYSTEM TEST"
echo "=================================="
echo ""

# Give the player some starting credits for testing
echo "Test 1: Give player 10000 UC and 5000 BMC for testing"
echo "Command: @me->set_universal_credits(10000)"
echo "Command: @me->set_black_market_credits(5000)"
echo ""

echo "Test 2: Check credits balance"
echo "Command: credits"
echo ""

echo "Test 3: Go to General Store"
echo "Command: east"
echo ""

echo "Test 4: Browse shop inventory"
echo "Command: list"
echo ""

echo "Test 5: Try to buy a vibro-blade (5000 UC)"
echo "Command: buy vibro-blade"
echo ""

echo "Test 6: Check credits after purchase"
echo "Command: credits"
echo ""

echo "Test 7: Check inventory"
echo "Command: i"
echo ""

echo "Test 8: Try to sell the vibro-blade back (2500 UC)"
echo "Command: sell vibro-blade"
echo ""

echo "Test 9: Check final credits"
echo "Command: credits"
echo ""

echo "Test 10: Try to buy expensive item without enough credits"
echo "Command: buy heavy armor"
echo ""

echo "=================================="
echo "Manual Testing Steps:"
echo "=================================="
echo "1. Start the driver: ./mud.ctl start"
echo "2. Connect: telnet localhost 3000"
echo "3. Give yourself credits: @me->set_universal_credits(10000)"
echo "4. Check balance: credits"
echo "5. Go to shop: east"
echo "6. Browse items: list"
echo "7. Buy something: buy vibro-blade"
echo "8. Sell it back: sell vibro-blade"
echo "=================================="
