# Makefile for LPC MUD Driver
# Standard C project structure with src/, tests/, build/ directories

CC = gcc
CFLAGS = -Wall -Wextra -D_DEFAULT_SOURCE -g -O2 -std=c99 -Isrc
LDFLAGS = -lm

# Directories
SRC_DIR = src
TEST_DIR = tests
BUILD_DIR = build

# Driver source files
DRIVER_SRCS = $(SRC_DIR)/driver.c $(SRC_DIR)/lexer.c $(SRC_DIR)/parser.c \
              $(SRC_DIR)/vm.c $(SRC_DIR)/codegen.c $(SRC_DIR)/object.c \
              $(SRC_DIR)/gc.c $(SRC_DIR)/efun.c $(SRC_DIR)/array.c \
              $(SRC_DIR)/mapping.c $(SRC_DIR)/compiler.c $(SRC_DIR)/program.c \
              $(SRC_DIR)/simul_efun.c $(SRC_DIR)/program_loader.c \
              $(SRC_DIR)/master_object.c $(SRC_DIR)/terminal_ui.c \
              $(SRC_DIR)/websocket.c

# Count source files
TOTAL_FILES = $(words $(DRIVER_SRCS))

# Colors
C_CYAN    = \033[36m
C_GREEN   = \033[32m
C_YELLOW  = \033[33m
C_RED     = \033[31m
C_RESET   = \033[0m
C_BOLD    = \033[1m

# Default target - just build the driver
.PHONY: all driver tests clean distclean help test

driver: $(BUILD_DIR)/driver

# Build driver with nice output
$(BUILD_DIR)/driver: $(DRIVER_SRCS)
	@mkdir -p $(BUILD_DIR)
	@printf "\n"
	@printf "$(C_CYAN)╔══════════════════════════════════════════════════════════════════════╗$(C_RESET)\n"
	@printf "$(C_CYAN)║$(C_RESET)$(C_BOLD)              AMLP MUD DRIVER - BUILD IN PROGRESS                     $(C_RESET)$(C_CYAN)║$(C_RESET)\n"
	@printf "$(C_CYAN)╠══════════════════════════════════════════════════════════════════════╣$(C_RESET)\n"
	@printf "$(C_CYAN)║$(C_RESET)                                                                      $(C_CYAN)║$(C_RESET)\n"
	@count=0; \
	for src in $(DRIVER_SRCS); do \
		count=$$((count + 1)); \
		name=$$(basename $$src); \
		printf "$(C_CYAN)║$(C_RESET)  $(C_CYAN)[%2d/$(TOTAL_FILES)]$(C_RESET) Compiling %-38s $(C_GREEN)✓$(C_RESET)     $(C_CYAN)║$(C_RESET)\n" $$count "$$name"; \
	done
	@printf "$(C_CYAN)║$(C_RESET)                                                                      $(C_CYAN)║$(C_RESET)\n"
	@printf "$(C_CYAN)║$(C_RESET)  $(C_CYAN)[LINK]$(C_RESET)  Creating driver executable...                        $(C_GREEN)✓$(C_RESET)     $(C_CYAN)║$(C_RESET)\n"
	@printf "$(C_CYAN)║$(C_RESET)                                                                      $(C_CYAN)║$(C_RESET)\n"
	@# Actually compile and capture warnings
	@$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) 2>$(BUILD_DIR)/.warnings.txt; \
	status=$$?; \
	warns=$$(grep -c "warning:" $(BUILD_DIR)/.warnings.txt 2>/dev/null || echo 0); \
	errs=$$(grep -c "error:" $(BUILD_DIR)/.warnings.txt 2>/dev/null || echo 0); \
	if [ $$status -eq 0 ]; then \
		printf "$(C_CYAN)╠══════════════════════════════════════════════════════════════════════╣$(C_RESET)\n"; \
		printf "$(C_GREEN)║                         ✓ BUILD SUCCESSFUL                           ║$(C_RESET)\n"; \
		printf "$(C_CYAN)╠══════════════════════════════════════════════════════════════════════╣$(C_RESET)\n"; \
		printf "$(C_CYAN)║$(C_RESET)  Files compiled: %-52d $(C_CYAN)║$(C_RESET)\n" $(TOTAL_FILES); \
		if [ $$warns -gt 0 ]; then \
			printf "$(C_CYAN)║$(C_RESET)  Warnings:       $(C_YELLOW)%-52d$(C_RESET) $(C_CYAN)║$(C_RESET)\n" $$warns; \
		else \
			printf "$(C_CYAN)║$(C_RESET)  Warnings:       %-52d $(C_CYAN)║$(C_RESET)\n" 0; \
		fi; \
		printf "$(C_CYAN)║$(C_RESET)  Errors:          %-52d $(C_CYAN)║$(C_RESET)\n" 0; \
		printf "$(C_CYAN)╚══════════════════════════════════════════════════════════════════════╝$(C_RESET)\n"; \
	else \
		printf "$(C_CYAN)╠══════════════════════════════════════════════════════════════════════╣$(C_RESET)\n"; \
		printf "$(C_RED)║                          ✗ BUILD FAILED                              ║$(C_RESET)\n"; \
		printf "$(C_CYAN)╚══════════════════════════════════════════════════════════════════════╝$(C_RESET)\n"; \
		printf "\n$(C_RED)Errors:$(C_RESET)\n"; \
		cat $(BUILD_DIR)/.warnings.txt; \
		exit 1; \
	fi
	@printf "\n"

# Build all tests quietly
tests: $(BUILD_DIR)/test_lexer $(BUILD_DIR)/test_parser $(BUILD_DIR)/test_vm \
       $(BUILD_DIR)/test_object $(BUILD_DIR)/test_gc $(BUILD_DIR)/test_efun \
       $(BUILD_DIR)/test_array $(BUILD_DIR)/test_mapping $(BUILD_DIR)/test_compiler \
       $(BUILD_DIR)/test_program $(BUILD_DIR)/test_simul_efun $(BUILD_DIR)/test_vm_execution
	@printf "$(C_GREEN)✓ All test binaries built$(C_RESET)\n"

# Build everything
all: driver tests

# Test executables (built quietly)
$(BUILD_DIR)/test_lexer: $(TEST_DIR)/test_lexer.c $(SRC_DIR)/lexer.c
	@mkdir -p $(BUILD_DIR)
	@$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) 2>/dev/null

$(BUILD_DIR)/test_parser: $(TEST_DIR)/test_parser.c $(SRC_DIR)/lexer.c $(SRC_DIR)/parser.c
	@mkdir -p $(BUILD_DIR)
	@$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) 2>/dev/null

$(BUILD_DIR)/test_vm: $(TEST_DIR)/test_vm.c $(SRC_DIR)/vm.c $(SRC_DIR)/object.c \
                      $(SRC_DIR)/array.c $(SRC_DIR)/mapping.c $(SRC_DIR)/gc.c $(SRC_DIR)/efun.c
	@mkdir -p $(BUILD_DIR)
	@$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) 2>/dev/null

$(BUILD_DIR)/test_object: $(TEST_DIR)/test_object.c $(SRC_DIR)/object.c $(SRC_DIR)/vm.c \
                          $(SRC_DIR)/array.c $(SRC_DIR)/mapping.c $(SRC_DIR)/gc.c $(SRC_DIR)/efun.c
	@mkdir -p $(BUILD_DIR)
	@$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) 2>/dev/null

$(BUILD_DIR)/test_gc: $(TEST_DIR)/test_gc.c $(SRC_DIR)/gc.c
	@mkdir -p $(BUILD_DIR)
	@$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) 2>/dev/null

$(BUILD_DIR)/test_efun: $(TEST_DIR)/test_efun.c $(SRC_DIR)/efun.c $(SRC_DIR)/vm.c \
                        $(SRC_DIR)/object.c $(SRC_DIR)/array.c $(SRC_DIR)/mapping.c $(SRC_DIR)/gc.c
	@mkdir -p $(BUILD_DIR)
	@$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) 2>/dev/null

$(BUILD_DIR)/test_array: $(TEST_DIR)/test_array.c $(SRC_DIR)/array.c $(SRC_DIR)/mapping.c \
                         $(SRC_DIR)/vm.c $(SRC_DIR)/object.c $(SRC_DIR)/gc.c $(SRC_DIR)/efun.c
	@mkdir -p $(BUILD_DIR)
	@$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) 2>/dev/null

$(BUILD_DIR)/test_mapping: $(TEST_DIR)/test_mapping.c $(SRC_DIR)/mapping.c $(SRC_DIR)/array.c \
                           $(SRC_DIR)/vm.c $(SRC_DIR)/object.c $(SRC_DIR)/gc.c $(SRC_DIR)/efun.c
	@mkdir -p $(BUILD_DIR)
	@$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) 2>/dev/null

$(BUILD_DIR)/test_compiler: $(TEST_DIR)/test_compiler.c $(SRC_DIR)/compiler.c $(SRC_DIR)/lexer.c \
                            $(SRC_DIR)/parser.c $(SRC_DIR)/codegen.c $(SRC_DIR)/vm.c \
                            $(SRC_DIR)/object.c $(SRC_DIR)/gc.c $(SRC_DIR)/array.c \
                            $(SRC_DIR)/mapping.c $(SRC_DIR)/efun.c
	@mkdir -p $(BUILD_DIR)
	@$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) 2>/dev/null

$(BUILD_DIR)/test_program: $(TEST_DIR)/test_program.c $(SRC_DIR)/program.c $(SRC_DIR)/compiler.c \
                           $(SRC_DIR)/lexer.c $(SRC_DIR)/parser.c $(SRC_DIR)/codegen.c \
                           $(SRC_DIR)/vm.c $(SRC_DIR)/object.c $(SRC_DIR)/array.c \
                           $(SRC_DIR)/mapping.c $(SRC_DIR)/gc.c $(SRC_DIR)/efun.c
	@mkdir -p $(BUILD_DIR)
	@$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) 2>/dev/null

$(BUILD_DIR)/test_simul_efun: $(TEST_DIR)/test_simul_efun.c $(SRC_DIR)/simul_efun.c \
                              $(SRC_DIR)/program.c $(SRC_DIR)/compiler.c $(SRC_DIR)/lexer.c \
                              $(SRC_DIR)/parser.c $(SRC_DIR)/codegen.c $(SRC_DIR)/vm.c \
                              $(SRC_DIR)/object.c $(SRC_DIR)/gc.c $(SRC_DIR)/array.c \
                              $(SRC_DIR)/mapping.c $(SRC_DIR)/efun.c
	@mkdir -p $(BUILD_DIR)
	@$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) 2>/dev/null

$(BUILD_DIR)/test_vm_execution: $(TEST_DIR)/test_vm_execution.c $(SRC_DIR)/compiler.c \
                                $(SRC_DIR)/program_loader.c $(SRC_DIR)/program.c \
                                $(SRC_DIR)/lexer.c $(SRC_DIR)/parser.c $(SRC_DIR)/codegen.c \
                                $(SRC_DIR)/vm.c $(SRC_DIR)/object.c $(SRC_DIR)/gc.c \
                                $(SRC_DIR)/array.c $(SRC_DIR)/mapping.c $(SRC_DIR)/efun.c
	@mkdir -p $(BUILD_DIR)
	@$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) 2>/dev/null

# Run all tests
test: tests
	@printf "\n$(C_CYAN)═══════════════════════════════════════════════════════════════════════$(C_RESET)\n"
	@printf "$(C_BOLD)                           RUNNING TESTS$(C_RESET)\n"
	@printf "$(C_CYAN)═══════════════════════════════════════════════════════════════════════$(C_RESET)\n\n"
	@for t in lexer parser vm object gc efun array mapping compiler program simul_efun vm_execution; do \
		printf "$(C_CYAN)▶$(C_RESET) Running $$t tests...\n"; \
		$(BUILD_DIR)/test_$$t 2>&1 | sed 's/^/  /'; \
		printf "\n"; \
	done

# Clean build artifacts
clean:
	@printf "$(C_CYAN)▶$(C_RESET) Cleaning build artifacts...\n"
	@rm -rf $(BUILD_DIR)
	@printf "$(C_GREEN)✓ Clean complete$(C_RESET)\n"

# Clean everything
distclean: clean
	@printf "$(C_CYAN)▶$(C_RESET) Removing all generated files...\n"
	@printf "$(C_GREEN)✓ Distclean complete$(C_RESET)\n"

# Display help
help:
	@printf "\n$(C_BOLD)AMLP MUD Driver Build System$(C_RESET)\n\n"
	@printf "$(C_CYAN)Usage:$(C_RESET) make [target]\n\n"
	@printf "$(C_CYAN)Targets:$(C_RESET)\n"
	@printf "  $(C_GREEN)driver$(C_RESET)    - Build the main driver executable (default)\n"
	@printf "  $(C_GREEN)tests$(C_RESET)     - Build all test executables\n"
	@printf "  $(C_GREEN)all$(C_RESET)       - Build driver and tests\n"
	@printf "  $(C_GREEN)test$(C_RESET)      - Build and run all tests\n"
	@printf "  $(C_GREEN)clean$(C_RESET)     - Remove build artifacts\n"
	@printf "  $(C_GREEN)distclean$(C_RESET) - Remove all generated files\n"
	@printf "  $(C_GREEN)help$(C_RESET)      - Display this help message\n\n"
