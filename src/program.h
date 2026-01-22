#ifndef PROGRAM_H
#define PROGRAM_H

#include "compiler.h"

/**
 * Program management - handles loading, execution, and lifecycle of compiled LPC programs
 */

typedef enum {
    EXEC_SUCCESS = 0,
    EXEC_ERROR_FUNCTION_NOT_FOUND = 1,
    EXEC_ERROR_INVALID_ARGS = 2,
    EXEC_ERROR_RUNTIME = 3,
    EXEC_ERROR_STACK_OVERFLOW = 4
} exec_error_t;

typedef struct {
    Program *program;
    int function_index;
    VMValue *args;
    int arg_count;
    exec_error_t last_error;
} program_context_t;

// Program loading and execution
Program* program_load_file(const char *filename);
Program* program_load_string(const char *source, const char *name);

// Function execution
VMValue program_execute_function(Program *prog, const char *function_name,
                                 VMValue *args, int arg_count);
VMValue program_execute_by_index(Program *prog, int function_index,
                                 VMValue *args, int arg_count);

// Program information
int program_function_count(Program *prog);
const char* program_function_name(Program *prog, int index);
int program_function_args(Program *prog, int index);
int program_global_count(Program *prog);
const char* program_global_name(Program *prog, int index);

// Error handling
const char* program_error_string(exec_error_t error);

#endif
