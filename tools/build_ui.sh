#!/bin/bash
# Build UI wrapper for AMLP Driver
# Shows styled progress bars and summary for compilation

# Colors
CYAN='\033[36m'
GREEN='\033[32m'
YELLOW='\033[33m'
RED='\033[31m'
BOLD='\033[1m'
RESET='\033[0m'

# Get list of source files from src/ directory
SOURCE_FILES=$(find src -name "*.c" 2>/dev/null | sort)
TOTAL_FILES=$(echo "$SOURCE_FILES" | wc -l)

# Build directory
BUILD_DIR="build"
mkdir -p "$BUILD_DIR"

# Tracking
CURRENT=0
WARNINGS=0
ERRORS=0
WARNING_MSGS=()
ERROR_MSGS=()

# Start timer
START_TIME=$(date +%s)

# Clear screen and show header
clear
echo -e "${CYAN}${BOLD}"
echo "╔════════════════════════════════════════════════════════════════╗"
echo "║           AMLP DRIVER - COMPILATION IN PROGRESS                ║"
echo "╚════════════════════════════════════════════════════════════════╝"
echo -e "${RESET}\n"

# Compilation flags
CFLAGS="-Wall -Wextra -Werror -std=c99 -g -I./src"

# Compile each file
for SRC in $SOURCE_FILES; do
    CURRENT=$((CURRENT + 1))
    OBJ="${BUILD_DIR}/$(basename ${SRC%.c}.o)"
    
    # Show current file with progress counter
    echo -ne "${CYAN}[${CURRENT}/${TOTAL_FILES}]${RESET} Compiling $(basename $SRC)..."
    
    # Compile and capture output
    if OUTPUT=$(gcc $CFLAGS -c "$SRC" -o "$OBJ" 2>&1); then
        echo -e " ${GREEN}✓${RESET}"
    else
        if echo "$OUTPUT" | grep -q "error:"; then
            ERRORS=$((ERRORS + 1))
            echo -e " ${RED}✗${RESET}"
            ERROR_MSGS+=("$(basename $SRC): $OUTPUT")
        else
            WARNINGS=$((WARNINGS + 1))
            echo -e " ${YELLOW}⚠${RESET}"
            WARNING_MSGS+=("$(basename $SRC): $OUTPUT")
        fi
    fi
done

echo ""

# Link all object files
echo -ne "${CYAN}${BOLD}[LINK]${RESET} Creating driver executable..."
OBJ_FILES=$(ls build/*.o 2>/dev/null | tr '\n' ' ')

if [ -n "$OBJ_FILES" ]; then
    if gcc $OBJ_FILES -o build/driver -lm 2>/dev/null; then
        echo -e " ${GREEN}✓${RESET}\n"
    else
        echo -e " ${RED}✗${RESET}\n"
        ERRORS=$((ERRORS + 1))
    fi
else
    echo -e " ${RED}✗${RESET} (no object files)\n"
    ERRORS=$((ERRORS + 1))
fi

# Calculate elapsed time
END_TIME=$(date +%s)
ELAPSED=$((END_TIME - START_TIME))

# Show warnings
if [ $WARNINGS -gt 0 ]; then
    echo -e "${YELLOW}${BOLD}"
    echo "╔════════════════════════════════════════════════════════════════╗"
    echo "║ ⚠  WARNINGS ($WARNINGS)                                               ║"
    echo "╚════════════════════════════════════════════════════════════════╝"
    echo -e "${RESET}"
    for msg in "${WARNING_MSGS[@]}"; do
        echo -e "${YELLOW}  • ${msg}${RESET}" | head -n 1
    done
    echo ""
fi

# Show errors
if [ $ERRORS -gt 0 ]; then
    echo -e "${RED}${BOLD}"
    echo "╔════════════════════════════════════════════════════════════════╗"
    echo "║ ✗  ERRORS ($ERRORS)                                                  ║"
    echo "╚════════════════════════════════════════════════════════════════╝"
    echo -e "${RESET}"
    for msg in "${ERROR_MSGS[@]}"; do
        echo -e "${RED}  • ${msg}${RESET}" | head -n 1
    done
    echo ""
fi

# Show summary
echo ""
if [ $ERRORS -eq 0 ]; then
    echo -e "${GREEN}${BOLD}"
    echo "╔════════════════════════════════════════════════════════════════╗"
    echo "║ ✓  BUILD SUCCESSFUL                                            ║"
    echo "╚════════════════════════════════════════════════════════════════╝"
    echo -e "${RESET}"
else
    echo -e "${RED}${BOLD}"
    echo "╔════════════════════════════════════════════════════════════════╗"
    echo "║ ✗  BUILD FAILED                                                ║"
    echo "╚════════════════════════════════════════════════════════════════╝"
    echo -e "${RESET}"
fi

echo ""
echo -e "${CYAN}  Files compiled: ${RESET}${TOTAL_FILES}"
echo -e "${YELLOW}  Warnings:       ${RESET}${WARNINGS}"
if [ $ERRORS -gt 0 ]; then
    echo -e "${RED}  Errors:         ${ERRORS}${RESET}"
else
    echo -e "${GREEN}  Errors:         ${ERRORS}${RESET}"
fi
echo -e "${CYAN}  Time:           ${RESET}${ELAPSED}s"
echo ""

# Exit with appropriate code
if [ $ERRORS -gt 0 ]; then
    exit 1
fi

exit 0
