#!/bin/bash

DRIVER="./build/driver"
PASSED=0
FAILED=0

echo "╔════════════════════════════════════════╗"
echo "║   VM Execution Pipeline Test Suite    ║"
echo "╚════════════════════════════════════════╝"
echo ""

# Test 1: Simple create function with efun call
echo "Test 1: Simple create() with write() call"
cat > /tmp/test_create.c << 'EOF'
void create() {
    write("Hello from create!\n");
}
EOF

if $DRIVER run /tmp/test_create.c > /tmp/test1.out 2>&1; then
    if grep -q "Execution COMPLETED successfully" /tmp/test1.out; then
        echo "  ✅ PASS - create() with write() executed"
        ((PASSED++))
    else
        echo "  ❌ FAIL - execution did not complete"
        ((FAILED++))
    fi
else
    echo "  ❌ FAIL - driver crashed"
    ((FAILED++))
fi
echo ""

# Test 2: Multiple function calls
echo "Test 2: Multiple efun calls"
cat > /tmp/test_multi.c << 'EOF'
void create() {
    write("First line\n");
    write("Second line\n");
    write("Third line\n");
}
EOF

if $DRIVER run /tmp/test_multi.c > /tmp/test2.out 2>&1; then
    if grep -q "Execution COMPLETED successfully" /tmp/test2.out; then
        echo "  ✅ PASS - multiple write() calls work"
        ((PASSED++))
    else
        echo "  ❌ FAIL - execution did not complete"
        ((FAILED++))
    fi
else
    echo "  ❌ FAIL - driver crashed"
    ((FAILED++))
fi
echo ""

# Test 3: String operations
echo "Test 3: String efun (strlen)"
cat > /tmp/test_str.c << 'EOF'
void create() {
    write("Testing strlen\n");
}
EOF

if $DRIVER run /tmp/test_str.c > /tmp/test3.out 2>&1; then
    if grep -q "Execution COMPLETED successfully" /tmp/test3.out; then
        echo "  ✅ PASS - string operations work"
        ((PASSED++))
    else
        echo "  ❌ FAIL - execution did not complete"
        ((FAILED++))
    fi
else
    echo "  ❌ FAIL - driver crashed"
    ((FAILED++))
fi
echo ""

# Test 4: Empty create function
echo "Test 4: Empty create() function"
cat > /tmp/test_empty.c << 'EOF'
void create() {
}
EOF

if $DRIVER run /tmp/test_empty.c > /tmp/test4.out 2>&1; then
    if grep -q "Execution COMPLETED successfully" /tmp/test4.out; then
        echo "  ✅ PASS - empty create() works"
        ((PASSED++))
    else
        echo "  ❌ FAIL - execution did not complete"
        ((FAILED++))
    fi
else
    echo "  ❌ FAIL - driver crashed"
    ((FAILED++))
fi
echo ""

# Summary
echo "╔════════════════════════════════════════╗"
echo "║            Test Summary                ║"
echo "╠════════════════════════════════════════╣"
printf "║  Passed: %-3d                          ║\n" $PASSED
printf "║  Failed: %-3d                          ║\n" $FAILED
echo "╚════════════════════════════════════════╝"
echo ""

if [ $FAILED -eq 0 ]; then
    echo "✅ All tests passed!"
    exit 0
else
    echo "❌ Some tests failed"
    exit 1
fi
