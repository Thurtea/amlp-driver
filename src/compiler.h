#ifndef COMPILER_H
#define COMPILER_H

#include "vm.h"
#include "codegen.h"

typedef enum {
    COMPILE_SUCCESS = 0,
    COMPILE_ERROR_IO = 1,
    COMPILE_ERROR_SYNTAX = 2,
    COMPILE_ERROR_SEMANTIC = 3,
    COMPILE_ERROR_UNKNOWN = 4
} compile_error_t;

typedef struct {
    char *filename;
    int line;
    int column;
    char *message;
} compile_error_info_t;

typedef struct Program {
    char *filename;
    char *source;
    
    // Bytecode and constants
    uint8_t *bytecode;
    size_t bytecode_len;
    
    // Function table
    struct {
        char *name;
        uint16_t offset;
        uint8_t arg_count;
        uint8_t local_count;
    } *functions;
    size_t function_count;
    
    // Global variables
    struct {
        char *name;
        VMValue value;
    } *globals;
    size_t global_count;
    
    // Constants pool
    VMValue *constants;
    size_t constant_count;
    
    // Debug info
    struct {
        uint16_t bytecode_offset;
        uint16_t source_line;
    } *line_map;
    size_t line_map_count;
    
    // Error tracking
    compile_error_t last_error;
    compile_error_info_t error_info;
    
    // Reference counting for GC
    int ref_count;
} Program;

// Compilation functions
Program* compiler_compile_file(const char *filename);
Program* compiler_compile_string(const char *source, const char *name);

// Program management
void program_incref(Program *prog);
void program_decref(Program *prog);
void program_free(Program *prog);

// Program execution support
int program_find_function(Program *prog, const char *name);
int program_find_global(Program *prog, const char *name);
VMValue program_get_global(Program *prog, const char *name);
void program_set_global(Program *prog, const char *name, VMValue value);

// Error information
const char* compiler_error_string(compile_error_t error);
void compiler_print_error(Program *prog);

#endif
