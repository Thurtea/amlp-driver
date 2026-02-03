#!/bin/bash

# Test ASCII Border Formatting
# Verifies that Unicode borders have been replaced with clean ASCII

echo "╔════════════════════════════════════════════════╗"
echo "║     ASCII Border Formatting Test              ║"
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

echo "Testing character stats formatting..."
echo "This will show the new ASCII border style."
echo ""

# Create a test command file
cat > /tmp/test_format.txt << 'EOF'
thurtea

stats
skills
languages
quit
EOF

echo "Commands to test:"
echo "  - stats (character sheet)"
echo "  - skills (skill list)"  
echo "  - languages (language list)"
echo ""

# Run the test
timeout 10 bash -c 'cat /tmp/test_format.txt | nc localhost 3000' 2>&1 | tee /tmp/format_test_output.txt

echo ""
echo "╔════════════════════════════════════════════════╗"
echo "║              Test Results                      ║"
echo "╚════════════════════════════════════════════════╝"
echo ""

# Check for old Unicode borders (should be gone)
if grep -q "╔\|║\|═\|╠\|╣\|╚\|╝" /tmp/format_test_output.txt; then
    echo "❌ FAIL: Unicode borders still present!"
    echo "   Files may not have reloaded properly."
else
    echo "✅ PASS: No Unicode borders found"
fi

# Check for new ASCII borders (should be present)
if grep -q "========================================" /tmp/format_test_output.txt; then
    echo "✅ PASS: ASCII headers found"
else
    echo "⚠️  WARNING: ASCII headers not found"
fi

if grep -q "----------------------------------------" /tmp/format_test_output.txt; then
    echo "✅ PASS: ASCII dividers found"
else
    echo "⚠️  WARNING: ASCII dividers not found"
fi

# Check alignment (should not have excessive whitespace)
if grep -q "║  " /tmp/format_test_output.txt; then
    echo "❌ FAIL: Old box padding still present (║  )"
else
    echo "✅ PASS: Clean left alignment (no box padding)"
fi

echo ""
echo "Full output saved to: /tmp/format_test_output.txt"
echo "Review with: cat /tmp/format_test_output.txt"
echo ""

# Show a sample of the stats output
echo "Sample stats output:"
echo "---"
grep -A 20 "========================================" /tmp/format_test_output.txt | head -25
echo "---"
echo ""

# Cleanup
rm -f /tmp/test_format.txt

echo "Test complete!"
