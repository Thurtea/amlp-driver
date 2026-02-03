# Makefile for LPC MUD Driver
# Standard C project structure with src/, tests/, build/ directories

CC = gcc
CFLAGS = -Wall -Wextra -D_DEFAULT_SOURCE -g -O2 -std=c99 -Isrc
LDFLAGS = -lm

# Directories
SRC_DIR = src
TEST_DIR = tests
BUILD_DIR = build

# Common sources needed for all tests (MUST BE BEFORE RULES!)
TEST_COMMON_SOURCES = $(SRC_DIR)/vm.c \
                      $(SRC_DIR)/object.c \
                      $(SRC_DIR)/array.c \
                      $(SRC_DIR)/mapping.c \
                      $(SRC_DIR)/gc.c \
                      $(SRC_DIR)/efun.c \
                      $(SRC_DIR)/compiler.c \
                      $(SRC_DIR)/program_loader.c \
                      $(SRC_DIR)/program.c \
                      $(SRC_DIR)/master_object.c \
                      $(SRC_DIR)/session.c \
                      $(SRC_DIR)/lexer.c \
                      $(SRC_DIR)/parser.c \
                      $(SRC_DIR)/codegen.c

# Driver source files
DRIVER_SRCS = $(SRC_DIR)/driver.c $(SRC_DIR)/server.c $(SRC_DIR)/lexer.c $(SRC_DIR)/parser.c \
              $(SRC_DIR)/vm.c $(SRC_DIR)/codegen.c $(SRC_DIR)/object.c \
              $(SRC_DIR)/gc.c $(SRC_DIR)/efun.c $(SRC_DIR)/array.c \
              $(SRC_DIR)/mapping.c $(SRC_DIR)/compiler.c $(SRC_DIR)/program.c \
              $(SRC_DIR)/simul_efun.c $(SRC_DIR)/program_loader.c \
              $(SRC_DIR)/master_object.c $(SRC_DIR)/terminal_ui.c \
              $(SRC_DIR)/websocket.c $(SRC_DIR)/session.c \
              $(SRC_DIR)/room.c $(SRC_DIR)/chargen.c $(SRC_DIR)/skills.c \
              $(SRC_DIR)/combat.c $(SRC_DIR)/item.c $(SRC_DIR)/psionics.c \
              $(SRC_DIR)/magic.c

# Count source files
TOTAL_FILES = $(words $(DRIVER_SRCS))

# Colors
C_CYAN = \033[36m
C_GREEN = \033[32m
C_YELLOW = \033[33m
C_RED = \033[31m
C_RESET = \033[0m
C_BOLD = \033[1m

# Default target - just build the driver
.PHONY: all driver tests clean distclean help test

driver: $(BUILD_DIR)/driver


# Build driver with custom unicode frame and ASCII indicators
$(BUILD_DIR)/driver: $(DRIVER_SRCS)
	@mkdir -p $(BUILD_DIR)
	@printf "\n"
	@printf "$(C_CYAN)╔════════════════════════════════════════════════════════════════════════════╗$(C_RESET)\n"
	@printf "$(C_CYAN)║$(C_RESET)%-76s$(C_CYAN)║$(C_RESET)\n" "AMLP MUD DRIVER - BUILD IN PROGRESS"
	@printf "$(C_CYAN)╠════════════════════════════════════════════════════════════════════════════╣$(C_RESET)\n"
	@printf "$(C_CYAN)║$(C_RESET)%-76s$(C_CYAN)║$(C_RESET)\n" ""
	@count=0; for src in $(DRIVER_SRCS); do \
		count=$$((count + 1)); \
		name=$$(basename $$src); \
		line=$$(printf " [*] [%2d/$(TOTAL_FILES)] Compiling %s" $$count "$$name"); \
		printf "$(C_CYAN)║$(C_RESET)%-76s$(C_CYAN)║$(C_RESET)\n" "$$line"; \
	done
	@printf "║                                                                            ║\n"
	@line=$$(printf " [*] [LINK]  Creating driver executable..."); printf "$(C_CYAN)║$(C_RESET)%-76s$(C_CYAN)║$(C_RESET)\n" "$$line"
	@printf "$(C_CYAN)║$(C_RESET)%-76s$(C_CYAN)║$(C_RESET)\n" ""
	@$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) 2>$(BUILD_DIR)/.warnings.txt; \
	status=$$?; \
	warns=$$(grep -c "warning:" $(BUILD_DIR)/.warnings.txt 2>/dev/null | head -1 || echo 0); \
	warns=$${warns:-0}; \
	       if [ "$$status" -eq 0 ]; then \
		       printf "$(C_CYAN)╠════════════════════════════════════════════════════════════════════════════╣$(C_RESET)\n"; \
		       printf "$(C_CYAN)║$(C_RESET)%-76s$(C_CYAN)║$(C_RESET)\n" "  BUILD SUCCESSFUL"; \
		       printf "$(C_CYAN)╠════════════════════════════════════════════════════════════════════════════╣$(C_RESET)\n"; \
		       printf "$(C_CYAN)║$(C_RESET)%-76s$(C_CYAN)║$(C_RESET)\n" "  Files compiled: $(TOTAL_FILES)"; \
		       printf "$(C_CYAN)║$(C_RESET)%-76s$(C_CYAN)║$(C_RESET)\n" "  Warnings:       $$warns"; \
		       printf "$(C_CYAN)║$(C_RESET)%-76s$(C_CYAN)║$(C_RESET)\n" "  Errors:         0"; \
		       printf "$(C_CYAN)╚════════════════════════════════════════════════════════════════════════════╝$(C_RESET)\n"; \
	else \
		printf "╠════════════════════════════════════════════════════════════════════════════╣\n"; \
		printf "║                         X BUILD FAILED                                  ║\n"; \
		printf "╚════════════════════════════════════════════════════════════════════════════╝\n"; \
		printf "\nErrors:\n"; \
		cat $(BUILD_DIR)/.warnings.txt; \
		exit 1; \
	fi
	@printf "\n"

# Build all tests quietly
tests: $(BUILD_DIR)/test_lexer $(BUILD_DIR)/test_parser $(BUILD_DIR)/test_vm \
       $(BUILD_DIR)/test_object $(BUILD_DIR)/test_gc $(BUILD_DIR)/test_efun \
       $(BUILD_DIR)/test_array $(BUILD_DIR)/test_mapping $(BUILD_DIR)/test_compiler \
       $(BUILD_DIR)/test_program $(BUILD_DIR)/test_simul_efun $(BUILD_DIR)/test_vm_execution \
       $(BUILD_DIR)/test_parser_stability
	@printf "All test binaries built\n"

# Build everything
all: driver tests


# Pattern rule for all test targets (custom frame, ASCII indicators, no emojis)
$(BUILD_DIR)/test_%: $(TEST_DIR)/test_%.c $(TEST_COMMON_SOURCES)
	@mkdir -p $(BUILD_DIR)
	@printf "$(C_CYAN)╔════════════════════════════════════════════════════════════════════════════╗$(C_RESET)\n"
	@printf "$(C_CYAN)║$(C_RESET)%-76s$(C_CYAN)║$(C_RESET)\n" "BUILDING TEST: $@"
	@printf "$(C_CYAN)╠════════════════════════════════════════════════════════════════════════════╣$(C_RESET)\n"
	@printf "$(C_CYAN)║$(C_RESET)%-76s$(C_CYAN)║$(C_RESET)\n" " [*] Compiling test sources..."
	@$(CC) $(CFLAGS) -o $@ $^ -I$(SRC_DIR) -I$(TEST_DIR) $(LDFLAGS)
	@status=$$?; \
	       if [ "$$status" -eq 0 ]; then \
		       printf "$(C_CYAN)╠════════════════════════════════════════════════════════════════════════════╣$(C_RESET)\n"; \
		       printf "$(C_CYAN)║$(C_RESET)%-76s$(C_CYAN)║$(C_RESET)\n" "  TEST BUILD SUCCESSFUL"; \
		       printf "$(C_CYAN)╚════════════════════════════════════════════════════════════════════════════╝$(C_RESET)\n"; \
	else \
		printf "╠════════════════════════════════════════════════════════════════════════════╣\n"; \
		printf "║                         X TEST BUILD FAILED                             ║\n"; \
		printf "╚════════════════════════════════════════════════════════════════════════════╝\n"; \
		exit 1; \
	fi

# Specific override for test_simul_efun (needs simul_efun.c)
$(BUILD_DIR)/test_simul_efun: $(TEST_DIR)/test_simul_efun.c $(TEST_COMMON_SOURCES) $(SRC_DIR)/simul_efun.c
	@mkdir -p $(BUILD_DIR)
	@printf "$(C_CYAN)╔════════════════════════════════════════════════════════════════════════════╗$(C_RESET)\n"
	@printf "$(C_CYAN)║$(C_RESET)%-76s$(C_CYAN)║$(C_RESET)\n" "BUILDING TEST: $@"
	@printf "$(C_CYAN)╠════════════════════════════════════════════════════════════════════════════╣$(C_RESET)\n"
	@printf "$(C_CYAN)║$(C_RESET)%-76s$(C_CYAN)║$(C_RESET)\n" " [*] Compiling test sources..."
	@$(CC) $(CFLAGS) -o $@ $^ -I$(SRC_DIR) -I$(TEST_DIR) $(LDFLAGS)
	@status=$$?; \
	       if [ "$$status" -eq 0 ]; then \
		       printf "$(C_CYAN)╠════════════════════════════════════════════════════════════════════════════╣$(C_RESET)\n"; \
		       printf "$(C_CYAN)║$(C_RESET)%-76s$(C_CYAN)║$(C_RESET)\n" "  TEST BUILD SUCCESSFUL"; \
		       printf "$(C_CYAN)╚════════════════════════════════════════════════════════════════════════════╝$(C_RESET)\n"; \
	else \
		printf "╠════════════════════════════════════════════════════════════════════════════╣\n"; \
		printf "║                         X TEST BUILD FAILED                             ║\n"; \
		printf "╚════════════════════════════════════════════════════════════════════════════╝\n"; \
		exit 1; \
	fi

# Run all tests (custom frame, ASCII indicators, no emojis except checkmark)
test: tests
	@printf "\n$(C_CYAN)╔════════════════════════════════════════════════════════════════════════════╗$(C_RESET)\n"
	@printf "$(C_CYAN)║$(C_BOLD)%-76s$(C_CYAN)║$(C_RESET)\n" "RUNNING TESTS"
	@printf "$(C_CYAN)╠════════════════════════════════════════════════════════════════════════════╣$(C_RESET)\n"
	@for t in lexer parser vm object gc efun array mapping compiler program simul_efun vm_execution; do \
		printf "$(C_CYAN)║$(C_RESET) [*] Running %-62s$(C_CYAN)║$(C_RESET)\n" "$$t tests..."; \
		$(BUILD_DIR)/test_$$t 2>&1 | sed 's/^/  /'; \
		printf "$(C_CYAN)║%-76s$(C_CYAN)║\n" ""; \
	done
	@printf "$(C_CYAN)╚════════════════════════════════════════════════════════════════════════════╝$(C_RESET)\n"

# Clean build artifacts
clean:
	@printf "$(C_CYAN)▶$(C_RESET) Cleaning build artifacts...\n"
	@rm -rf $(BUILD_DIR)
	@printf "Clean complete\n"

# Clean everything
distclean: clean
	@printf "$(C_CYAN)▶$(C_RESET) Removing all generated files...\n"
	@printf "Distclean complete\n"

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
