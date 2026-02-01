#!/bin/bash
# Test the skills system integration

echo "=== Testing Skills System ==="
sleep 2

(
  # Character creation
  echo "skilltest_phase2"    # Username
  sleep 0.5
  echo "Pass123!"           # Password
  sleep 0.5
  echo "Pass123!"           # Confirm
  sleep 0.5
  echo "1"                  # Select Human race
  sleep 0.5
  echo "15"                 # Select OCC #15 (Juicer is a combat OCC)
  sleep 0.5
  echo "yes"                # Accept stats
  sleep 1
  echo "skills"             # Show skills
  sleep 2
  echo "quit"               # Quit
  sleep 1
) | nc localhost 3000 2>&1

echo ""
echo "=== Test Complete ==="
