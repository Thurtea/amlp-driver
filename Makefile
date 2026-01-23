# Makefile for LPC MUD Driver
# Standard C project structure with src/, tests/, build/ directories

CC = gcc
CFLAGS = -Wall -Wextra -Werror -g -O2 -std=c99 -Isrc
LDFLAGS = -lm

# Directories
SRC_DIR = src
TEST_DIR = tests
BUILD_DIR = build

# Source files (in src/)
DRIVER_SRCS = $(SRC_DIR)/driver.c $(SRC_DIR)/lexer.c $(SRC_DIR)/parser.c $(SRC_DIR)/vm.c $(SRC_DIR)/codegen.c $(SRC_DIR)/object.c $(SRC_DIR)/gc.c $(SRC_DIR)/efun.c $(SRC_DIR)/array.c $(SRC_DIR)/mapping.c $(SRC_DIR)/compiler.c $(SRC_DIR)/program.c $(SRC_DIR)/simul_efun.c
DRIVER_OBJS = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(DRIVER_SRCS))

TEST_LEXER_SRCS = $(TEST_DIR)/test_lexer.c $(SRC_DIR)/lexer.c
TEST_LEXER_OBJS = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(filter $(SRC_DIR)/%,$(TEST_LEXER_SRCS))) $(patsubst $(TEST_DIR)/%.c,$(BUILD_DIR)/%.o,$(filter $(TEST_DIR)/%,$(TEST_LEXER_SRCS)))

TEST_PARSER_SRCS = $(TEST_DIR)/test_parser.c $(SRC_DIR)/lexer.c $(SRC_DIR)/parser.c
TEST_PARSER_OBJS = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(filter $(SRC_DIR)/%,$(TEST_PARSER_SRCS))) $(patsubst $(TEST_DIR)/%.c,$(BUILD_DIR)/%.o,$(filter $(TEST_DIR)/%,$(TEST_PARSER_SRCS)))

TEST_VM_SRCS = $(TEST_DIR)/test_vm.c $(SRC_DIR)/vm.c $(SRC_DIR)/object.c $(SRC_DIR)/array.c $(SRC_DIR)/mapping.c $(SRC_DIR)/gc.c
TEST_VM_OBJS = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(filter $(SRC_DIR)/%,$(TEST_VM_SRCS))) $(patsubst $(TEST_DIR)/%.c,$(BUILD_DIR)/%.o,$(filter $(TEST_DIR)/%,$(TEST_VM_SRCS)))

TEST_OBJECT_SRCS = $(TEST_DIR)/test_object.c $(SRC_DIR)/object.c $(SRC_DIR)/vm.c $(SRC_DIR)/array.c $(SRC_DIR)/mapping.c $(SRC_DIR)/gc.c
TEST_OBJECT_OBJS = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(filter $(SRC_DIR)/%,$(TEST_OBJECT_SRCS))) $(patsubst $(TEST_DIR)/%.c,$(BUILD_DIR)/%.o,$(filter $(TEST_DIR)/%,$(TEST_OBJECT_SRCS)))

TEST_GC_SRCS = $(TEST_DIR)/test_gc.c $(SRC_DIR)/gc.c
TEST_GC_OBJS = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(filter $(SRC_DIR)/%,$(TEST_GC_SRCS))) $(patsubst $(TEST_DIR)/%.c,$(BUILD_DIR)/%.o,$(filter $(TEST_DIR)/%,$(TEST_GC_SRCS)))

TEST_EFUN_SRCS = $(TEST_DIR)/test_efun.c $(SRC_DIR)/efun.c $(SRC_DIR)/vm.c $(SRC_DIR)/object.c $(SRC_DIR)/array.c $(SRC_DIR)/mapping.c $(SRC_DIR)/gc.c
TEST_EFUN_OBJS = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(filter $(SRC_DIR)/%,$(TEST_EFUN_SRCS))) $(patsubst $(TEST_DIR)/%.c,$(BUILD_DIR)/%.o,$(filter $(TEST_DIR)/%,$(TEST_EFUN_SRCS)))

TEST_ARRAY_SRCS = $(TEST_DIR)/test_array.c $(SRC_DIR)/array.c $(SRC_DIR)/mapping.c $(SRC_DIR)/vm.c $(SRC_DIR)/object.c $(SRC_DIR)/gc.c
TEST_ARRAY_OBJS = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(filter $(SRC_DIR)/%,$(TEST_ARRAY_SRCS))) $(patsubst $(TEST_DIR)/%.c,$(BUILD_DIR)/%.o,$(filter $(TEST_DIR)/%,$(TEST_ARRAY_SRCS)))

TEST_MAPPING_SRCS = $(TEST_DIR)/test_mapping.c $(SRC_DIR)/mapping.c $(SRC_DIR)/array.c $(SRC_DIR)/vm.c $(SRC_DIR)/object.c $(SRC_DIR)/gc.c
TEST_MAPPING_OBJS = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(filter $(SRC_DIR)/%,$(TEST_MAPPING_SRCS))) $(patsubst $(TEST_DIR)/%.c,$(BUILD_DIR)/%.o,$(filter $(TEST_DIR)/%,$(TEST_MAPPING_SRCS)))

TEST_COMPILER_SRCS = $(TEST_DIR)/test_compiler.c $(SRC_DIR)/compiler.c $(SRC_DIR)/lexer.c $(SRC_DIR)/parser.c $(SRC_DIR)/codegen.c $(SRC_DIR)/vm.c $(SRC_DIR)/object.c $(SRC_DIR)/gc.c $(SRC_DIR)/array.c $(SRC_DIR)/mapping.c
TEST_COMPILER_OBJS = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(filter $(SRC_DIR)/%,$(TEST_COMPILER_SRCS))) $(patsubst $(TEST_DIR)/%.c,$(BUILD_DIR)/%.o,$(filter $(TEST_DIR)/%,$(TEST_COMPILER_SRCS)))

TEST_PROGRAM_SRCS = $(TEST_DIR)/test_program.c $(SRC_DIR)/program.c $(SRC_DIR)/compiler.c $(SRC_DIR)/lexer.c $(SRC_DIR)/parser.c $(SRC_DIR)/codegen.c $(SRC_DIR)/vm.c $(SRC_DIR)/object.c $(SRC_DIR)/array.c $(SRC_DIR)/mapping.c $(SRC_DIR)/gc.c
TEST_PROGRAM_OBJS = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(filter $(SRC_DIR)/%,$(TEST_PROGRAM_SRCS))) $(patsubst $(TEST_DIR)/%.c,$(BUILD_DIR)/%.o,$(filter $(TEST_DIR)/%,$(TEST_PROGRAM_SRCS)))

TEST_SIMUL_EFUN_SRCS = $(TEST_DIR)/test_simul_efun.c $(SRC_DIR)/simul_efun.c $(SRC_DIR)/program.c $(SRC_DIR)/compiler.c $(SRC_DIR)/lexer.c $(SRC_DIR)/parser.c $(SRC_DIR)/codegen.c $(SRC_DIR)/vm.c $(SRC_DIR)/object.c $(SRC_DIR)/gc.c $(SRC_DIR)/array.c $(SRC_DIR)/mapping.c
TEST_SIMUL_EFUN_OBJS = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(filter $(SRC_DIR)/%,$(TEST_SIMUL_EFUN_SRCS))) $(patsubst $(TEST_DIR)/%.c,$(BUILD_DIR)/%.o,$(filter $(TEST_DIR)/%,$(TEST_SIMUL_EFUN_SRCS)))

# Build targets
all: $(BUILD_DIR)/driver $(BUILD_DIR)/test_lexer $(BUILD_DIR)/test_parser $(BUILD_DIR)/test_vm $(BUILD_DIR)/test_object $(BUILD_DIR)/test_gc $(BUILD_DIR)/test_efun $(BUILD_DIR)/test_array $(BUILD_DIR)/test_mapping $(BUILD_DIR)/test_compiler $(BUILD_DIR)/test_program $(BUILD_DIR)/test_simul_efun

$(BUILD_DIR)/driver: $(DRIVER_OBJS)
	@echo "[Linking] Building driver executable..."
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
	@echo "[Success] Driver built successfully!"

$(BUILD_DIR)/test_lexer: $(TEST_LEXER_OBJS)
	@echo "[Linking] Building lexer test..."
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
	@echo "[Success] Lexer test built successfully!"

$(BUILD_DIR)/test_parser: $(TEST_PARSER_OBJS)
	@echo "[Linking] Building parser test..."
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
	@echo "[Success] Parser test built successfully!"

$(BUILD_DIR)/test_vm: $(TEST_VM_OBJS)
	@echo "[Linking] Building VM test..."
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
	@echo "[Success] VM test built successfully!"

$(BUILD_DIR)/test_object: $(TEST_OBJECT_OBJS)
	@echo "[Linking] Building object test..."
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
	@echo "[Success] Object test built successfully!"

$(BUILD_DIR)/test_gc: $(TEST_GC_OBJS)
	@echo "[Linking] Building GC test..."
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
	@echo "[Success] GC test built successfully!"

$(BUILD_DIR)/test_efun: $(TEST_EFUN_OBJS)
	@echo "[Linking] Building efun test..."
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
	@echo "[Success] Efun test built successfully!"

$(BUILD_DIR)/test_array: $(TEST_ARRAY_OBJS)
	@echo "[Linking] Building array test..."
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
	@echo "[Success] Array test built successfully!"

$(BUILD_DIR)/test_mapping: $(TEST_MAPPING_OBJS)
	@echo "[Linking] Building mapping test..."
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
	@echo "[Success] Mapping test built successfully!"

$(BUILD_DIR)/test_compiler: $(TEST_COMPILER_OBJS)
	@echo "[Linking] Building compiler test..."
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
	@echo "[Success] Compiler test built successfully!"

$(BUILD_DIR)/test_program: $(TEST_PROGRAM_OBJS)
	@echo "[Linking] Building program test..."
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
	@echo "[Success] Program test built successfully!"

$(BUILD_DIR)/test_simul_efun: $(TEST_SIMUL_EFUN_OBJS)
	@echo "[Linking] Building simul efun test..."
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
	@echo "[Success] Simul efun test built successfully!"

# Object file compilation rules
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@echo "[Compiling] $<"
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: $(TEST_DIR)/%.c
	@echo "[Compiling] $<"
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Test targets
test: $(BUILD_DIR)/test_lexer $(BUILD_DIR)/test_parser $(BUILD_DIR)/test_vm $(BUILD_DIR)/test_object $(BUILD_DIR)/test_gc $(BUILD_DIR)/test_efun $(BUILD_DIR)/test_array $(BUILD_DIR)/test_mapping $(BUILD_DIR)/test_compiler $(BUILD_DIR)/test_program $(BUILD_DIR)/test_simul_efun
	@echo ""
	@echo "====== Running Lexer Tests ======"
	-$(BUILD_DIR)/test_lexer
	@echo ""
	@echo "====== Running Parser Tests ======"
	-$(BUILD_DIR)/test_parser
	@echo ""
	@echo "====== Running VM Tests ======"
	-$(BUILD_DIR)/test_vm
	@echo ""
	@echo "====== Running Object Tests ======"
	-$(BUILD_DIR)/test_object
	@echo ""
	@echo "====== Running GC Tests ======"
	-$(BUILD_DIR)/test_gc
	@echo ""
	@echo "====== Running Efun Tests ======"
	-$(BUILD_DIR)/test_efun
	@echo ""
	@echo "====== Running Array Tests ======"
	-$(BUILD_DIR)/test_array
	@echo ""
	@echo "====== Running Mapping Tests ======"
	-$(BUILD_DIR)/test_mapping
	@echo ""
	@echo "====== Running Compiler Tests ======"
	-$(BUILD_DIR)/test_compiler
	@echo ""
	@echo "====== Running Program Tests ======"
	-$(BUILD_DIR)/test_program
	@echo ""
	@echo "====== Running Simul Efun Tests ======"
	-$(BUILD_DIR)/test_simul_efun

# Clean build artifacts
clean:
	@echo "[Cleaning] Removing build artifacts..."
	rm -rf $(BUILD_DIR)
	@echo "[Success] Cleaned!"

# Clean everything including tests
distclean: clean
	@echo "[Cleaning] Removing all generated files..."

# Display help
help:
	@echo "LPC MUD Driver Build System"
	@echo ""
	@echo "Available targets:"
	@echo "  all           - Build driver and tests (default)"
	@echo "  driver        - Build the main driver executable"
	@echo "  test_lexer    - Build lexer test program"
	@echo "  test_parser   - Build parser test program"
	@echo "  test_vm       - Build VM test program"
	@echo "  test_object   - Build object system test program"
	@echo "  test_gc       - Build GC test program"
	@echo "  test_efun     - Build efun test program"
	@echo "  test_array    - Build array test program"
	@echo "  test_mapping  - Build mapping test program"
	@echo "  test_compiler - Build compiler test program"
	@echo "  test_program  - Build program test program"
	@echo "  test_simul_efun - Build simul efun test program"
	@echo "  test          - Run all tests"
	@echo "  clean         - Remove build artifacts"
	@echo "  distclean     - Remove all generated files"
	@echo "  help          - Display this help message"

# Styled build with terminal UI
build-ui:
	@./tools/build_ui.sh

.PHONY: all test clean distclean help build-ui
