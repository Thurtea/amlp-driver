#!/bin/bash
# Test Phase 1 C command implementations

echo "=== Testing Phase 1 Commands ==="
echo ""

PORT=3000
USERNAME="thurtea"
PASSWORD="password"

# Helper function to send commands
test_command() {
    local cmd="$1"
    local desc="$2"
    
    echo "Testing: $desc"
    echo "Command: $cmd"
    (
        echo "$USERNAME"
        sleep 0.5
        echo "$PASSWORD"
        sleep 0.5
        echo "$cmd"
        sleep 0.3
        echo "quit"
    ) | telnet localhost $PORT 2>&1 | grep -A 10 "$cmd" | head -20
    echo ""
    echo "---"
    echo ""
}

# Test communication commands
echo "=== Communication Commands ==="
test_command "chat Hello everyone!" "Global chat"
test_command "shout Can anyone hear me?" "Shout in room"
test_command "tell thurtea Hi there" "Tell self (should fail or work)"
test_command "whisper thurtea secret" "Whisper to self"

# Test observation commands
echo "=== Observation Commands ==="
test_command "exits" "Show room exits"
test_command "examine room" "Examine room"
test_command "examine thurtea" "Examine self"

# Test inventory commands
echo "=== Inventory Commands ==="
test_command "inventory" "Show inventory"
test_command "get sword" "Get item (stub)"
test_command "give sword to thurtea" "Give item (stub)"

echo ""
echo "=== Test Complete ==="
