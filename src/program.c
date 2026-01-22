#include "program.h"
#include "vm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * Load and compile program from file
 */
Program* program_load_file(const char *filename) {
    if (!filename) return NULL;
    return compiler_compile_file(filename);
}

/**
 * Load and compile program from string
 */
Program* program_load_string(const char *source, const char *name) {
    if (!source || !name) return NULL;
    return compiler_compile_string(source, name);
}

/**
 * Execute a function in a program by name
 * Returns result of function execution
 */
VMValue program_execute_function(Program *prog, const char *function_name,
                                 VMValue *args, int arg_count) {
    if (!prog || !function_name) {
        return (VMValue){.type = VALUE_NULL};
    }
    
    int func_idx = program_find_function(prog, function_name);
    if (func_idx < 0) {
        fprintf(stderr, "Error: Function '%s' not found in program\n", function_name);
        return (VMValue){.type = VALUE_NULL};
    }
    
    return program_execute_by_index(prog, func_idx, args, arg_count);
}

/**
 * Execute a function by index
 * Phase 7 Note: This is a stub that will integrate with vm.c in iteration 2
 */
VMValue program_execute_by_index(Program *prog, int function_index,
                                 VMValue *args, int arg_count) {
    (void)args;        // Unused in Phase 7 iteration 1
    (void)arg_count;   // Unused in Phase 7 iteration 1
    
    if (!prog || function_index < 0 || function_index >= (int)prog->function_count) {
        return (VMValue){.type = VALUE_NULL};
    }
    
    // Phase 7 Iteration 2: Initialize VM, load program, execute function
    // For now, return a NULL value as placeholder
    
    return (VMValue){.type = VALUE_NULL};
}

/**
 * Get number of functions in program
 */
int program_function_count(Program *prog) {
    if (!prog) return 0;
    return prog->function_count;
}

/**
 * Get function name by index
 */
const char* program_function_name(Program *prog, int index) {
    if (!prog || index < 0 || index >= (int)prog->function_count) {
        return NULL;
    }
    return prog->functions[index].name;
}

/**
 * Get function argument count by index
 */
int program_function_args(Program *prog, int index) {
    if (!prog || index < 0 || index >= (int)prog->function_count) {
        return -1;
    }
    return prog->functions[index].arg_count;
}

/**
 * Get number of globals in program
 */
int program_global_count(Program *prog) {
    if (!prog) return 0;
    return prog->global_count;
}

/**
 * Get global variable name by index
 */
const char* program_global_name(Program *prog, int index) {
    if (!prog || index < 0 || index >= (int)prog->global_count) {
        return NULL;
    }
    return prog->globals[index].name;
}

/**
 * Get error string
 */
const char* program_error_string(exec_error_t error) {
    switch (error) {
        case EXEC_SUCCESS:
            return "Success";
        case EXEC_ERROR_FUNCTION_NOT_FOUND:
            return "Function not found";
        case EXEC_ERROR_INVALID_ARGS:
            return "Invalid arguments";
        case EXEC_ERROR_RUNTIME:
            return "Runtime error";
        case EXEC_ERROR_STACK_OVERFLOW:
            return "Stack overflow";
        default:
            return "Unknown error";
    }
}
