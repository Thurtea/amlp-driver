#include "compiler.h"
#include "lexer.h"
#include "parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_ERRORS 100
#define INITIAL_BYTECODE_SIZE 1024
#define INITIAL_FUNCTION_COUNT 16
#define INITIAL_GLOBALS_COUNT 16

typedef struct {
    compile_error_t error_type;
    int line;
    int column;
    char message[512];
} compiler_error_t;

typedef struct {
    compiler_error_t errors[MAX_ERRORS];
    int error_count;
    
    uint8_t *bytecode;
    size_t bytecode_len;
    size_t bytecode_capacity;
    
    struct {
        char *name;
        uint16_t offset;
        uint8_t arg_count;
        uint8_t local_count;
    } *functions;
    size_t function_count;
    size_t function_capacity;
    
    struct {
        char *name;
        VMValue value;
    } *globals;
    size_t global_count;
    size_t global_capacity;
    
    VMValue *constants;
    size_t constant_count;
    size_t constant_capacity;
    
    struct {
        uint16_t bytecode_offset;
        uint16_t source_line;
    } *line_map;
    size_t line_map_count;
    size_t line_map_capacity;
} compiler_state_t;

/**
 * Read file contents into memory
 */
static char* read_file(const char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f) {
        return NULL;
    }
    
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);
    
    char *content = malloc(size + 1);
    if (!content) {
        fclose(f);
        return NULL;
    }
    
    size_t read = fread(content, 1, size, f);
    fclose(f);
    
    content[read] = '\0';
    return content;
}

/**
 * Initialize compiler state
 */
static compiler_state_t* compiler_state_new(void) {
    compiler_state_t *state = malloc(sizeof(compiler_state_t));
    if (!state) return NULL;
    
    state->error_count = 0;
    
    state->bytecode_capacity = INITIAL_BYTECODE_SIZE;
    state->bytecode = malloc(state->bytecode_capacity);
    state->bytecode_len = 0;
    
    state->function_capacity = INITIAL_FUNCTION_COUNT;
    state->functions = malloc(sizeof(state->functions[0]) * state->function_capacity);
    state->function_count = 0;
    
    state->global_capacity = INITIAL_GLOBALS_COUNT;
    state->globals = malloc(sizeof(state->globals[0]) * state->global_capacity);
    state->global_count = 0;
    
    state->constant_capacity = 256;
    state->constants = malloc(sizeof(VMValue) * state->constant_capacity);
    state->constant_count = 0;
    
    state->line_map_capacity = 256;
    state->line_map = malloc(sizeof(state->line_map[0]) * state->line_map_capacity);
    state->line_map_count = 0;
    
    return state;
}

/**
 * Free compiler state
 * Note: bytecode, functions, globals, constants, line_map are transferred
 * to Program object and will be freed by program_free()
 */
static void compiler_state_free(compiler_state_t *state) {
    if (!state) return;
    
    // Don't free the arrays - they're owned by Program now
    // Only free the state structure itself
    
    free(state);
}

/**
 * Add error to compiler state
 */
static void compiler_add_error(compiler_state_t *state, compile_error_t type,
                              int line, int column, const char *message) {
    if (state->error_count >= MAX_ERRORS) return;
    
    compiler_error_t *err = &state->errors[state->error_count++];
    err->error_type = type;
    err->line = line;
    err->column = column;
    strncpy(err->message, message, sizeof(err->message) - 1);
    err->message[sizeof(err->message) - 1] = '\0';
}

/**
 * Add function to program
 * Phase 7 Iteration 2: Will be used when integrating codegen
 
static void compiler_add_function(compiler_state_t *state, const char *name,
                                 uint16_t offset, uint8_t arg_count,
                                 uint8_t local_count) {
    if (state->function_count >= state->function_capacity) {
        state->function_capacity *= 2;
        state->functions = realloc(state->functions,
                                  sizeof(state->functions[0]) * state->function_capacity);
    }
    
    int index = state->function_count++;
    state->functions[index].name = malloc(strlen(name) + 1);
    strcpy(state->functions[index].name, name);
    state->functions[index].offset = offset;
    state->functions[index].arg_count = arg_count;
    state->functions[index].local_count = local_count;
}
*/

/**
 * Add global variable to program
 * Phase 7 Iteration 2: Will be used when integrating codegen
 
static void compiler_add_global(compiler_state_t *state, const char *name) {
    if (state->global_count >= state->global_capacity) {
        state->global_capacity *= 2;
        state->globals = realloc(state->globals,
                                sizeof(state->globals[0]) * state->global_capacity);
    }
    
    int index = state->global_count++;
    state->globals[index].name = malloc(strlen(name) + 1);
    strcpy(state->globals[index].name, name);
    state->globals[index].value.type = VALUE_NULL;
}
*/

/**
 * Add constant to program
 * Phase 7 Iteration 2: Will be used when integrating codegen
 
static void compiler_add_constant(compiler_state_t *state, VMValue value) {
    if (state->constant_count >= state->constant_capacity) {
        state->constant_capacity *= 2;
        state->constants = realloc(state->constants,
                                  sizeof(VMValue) * state->constant_capacity);
    }
    
    state->constants[state->constant_count++] = value;
}
*/

/**
 * Emit bytecode
 */
static void compiler_emit(compiler_state_t *state, uint8_t byte, int line) {
    if (state->bytecode_len >= state->bytecode_capacity) {
        state->bytecode_capacity *= 2;
        state->bytecode = realloc(state->bytecode, state->bytecode_capacity);
    }
    
    state->bytecode[state->bytecode_len++] = byte;
    
    // Update line map for debugging
    if (state->line_map_count == 0 ||
        state->line_map[state->line_map_count - 1].source_line != line) {
        if (state->line_map_count >= state->line_map_capacity) {
            state->line_map_capacity *= 2;
            state->line_map = realloc(state->line_map,
                                     sizeof(state->line_map[0]) * state->line_map_capacity);
        }
        
        state->line_map[state->line_map_count].bytecode_offset = state->bytecode_len - 1;
        state->line_map[state->line_map_count].source_line = line;
        state->line_map_count++;
    }
}

/**
 * Compile lexical tokens to bytecode
 * This is a stub that will be expanded in Phase 7 iteration 2
 */
static compile_error_t compiler_generate_bytecode(compiler_state_t *state,
                                                  ASTNode *ast) {
    if (!ast) {
        return COMPILE_SUCCESS;
    }
    
    // For now, just generate a simple RETURN instruction
    compiler_emit(state, OP_RETURN, 1);
    
    return state->error_count > 0 ? COMPILE_ERROR_SEMANTIC : COMPILE_SUCCESS;
}

/**
 * Compile LPC source code
 */
static Program* compiler_compile_internal(const char *source, const char *filename) {
    if (!source || !filename) {
        return NULL;
    }
    
    // Initialize compiler state
    compiler_state_t *state = compiler_state_new();
    if (!state) return NULL;
    
    // Lexical analysis
    Lexer *lexer = lexer_init_from_string(source);
    if (!lexer) {
        compiler_state_free(state);
        return NULL;
    }
    
    // Syntax analysis
    Parser *parser = parser_init(lexer);
    if (!parser) {
        lexer_free(lexer);
        compiler_state_free(state);
        return NULL;
    }
    
    ASTNode *ast = parser_parse(parser);
    
    // Note: Parser errors are tracked via parser->error_count
    // Phase 7 Iteration 2: Expand parser to collect detailed error info
    if (parser->error_count > 0) {
        compiler_add_error(state, COMPILE_ERROR_SYNTAX, 1, 0,
                         "Syntax error in source code");
    }
    
    // Semantic analysis and code generation
    compile_error_t compile_result = COMPILE_SUCCESS;
    if (state->error_count == 0 && ast) {
        compile_result = compiler_generate_bytecode(state, ast);
    }
    
    // Clean up parser and lexer
    parser_free(parser);
    lexer_free(lexer);
    
    // Create Program object
    Program *prog = malloc(sizeof(Program));
    if (!prog) {
        compiler_state_free(state);
        return NULL;
    }
    
    prog->filename = malloc(strlen(filename) + 1);
    strcpy(prog->filename, filename);
    
    prog->source = malloc(strlen(source) + 1);
    strcpy(prog->source, source);
    
    prog->bytecode = state->bytecode;
    prog->bytecode_len = state->bytecode_len;
    
    // Allocate Program's function array
    prog->functions = malloc(sizeof(prog->functions[0]) * state->function_count);
    for (size_t i = 0; i < state->function_count; i++) {
        prog->functions[i].name = state->functions[i].name;
        prog->functions[i].offset = state->functions[i].offset;
        prog->functions[i].arg_count = state->functions[i].arg_count;
        prog->functions[i].local_count = state->functions[i].local_count;
    }
    prog->function_count = state->function_count;
    
    // Allocate Program's globals array
    prog->globals = malloc(sizeof(prog->globals[0]) * state->global_count);
    for (size_t i = 0; i < state->global_count; i++) {
        prog->globals[i].name = state->globals[i].name;
        prog->globals[i].value = state->globals[i].value;
    }
    prog->global_count = state->global_count;
    
    prog->constants = state->constants;
    prog->constant_count = state->constant_count;
    
    // Allocate Program's line_map array
    prog->line_map = malloc(sizeof(prog->line_map[0]) * state->line_map_count);
    for (size_t i = 0; i < state->line_map_count; i++) {
        prog->line_map[i].bytecode_offset = state->line_map[i].bytecode_offset;
        prog->line_map[i].source_line = state->line_map[i].source_line;
    }
    prog->line_map_count = state->line_map_count;
    
    prog->last_error = compile_result;
    if (state->error_count > 0) {
        prog->error_info = (compile_error_info_t){
            .filename = prog->filename,
            .line = state->errors[0].line,
            .column = state->errors[0].column,
            .message = malloc(strlen(state->errors[0].message) + 1)
        };
        strcpy(prog->error_info.message, state->errors[0].message);
    } else {
        prog->error_info = (compile_error_info_t){
            .filename = NULL,
            .line = 0,
            .column = 0,
            .message = NULL
        };
    }
    
    prog->ref_count = 1;
    
    free(state);
    return prog;
}

/**
 * Public API: Compile file
 */
Program* compiler_compile_file(const char *filename) {
    if (!filename) return NULL;
    
    char *source = read_file(filename);
    if (!source) {
        fprintf(stderr, "Error: Could not open file '%s'\n", filename);
        return NULL;
    }
    
    Program *prog = compiler_compile_internal(source, filename);
    free(source);
    
    return prog;
}

/**
 * Public API: Compile string
 */
Program* compiler_compile_string(const char *source, const char *name) {
    if (!source || !name) return NULL;
    return compiler_compile_internal(source, name);
}

/**
 * Reference counting
 */
void program_incref(Program *prog) {
    if (prog) {
        prog->ref_count++;
    }
}

void program_decref(Program *prog) {
    if (prog) {
        prog->ref_count--;
        if (prog->ref_count <= 0) {
            program_free(prog);
        }
    }
}

/**
 * Free program
 */
void program_free(Program *prog) {
    if (!prog) return;
    
    if (prog->filename) free(prog->filename);
    if (prog->source) free(prog->source);
    if (prog->bytecode) free(prog->bytecode);
    
    if (prog->functions) {
        for (size_t i = 0; i < prog->function_count; i++) {
            free(prog->functions[i].name);
        }
        free(prog->functions);
    }
    
    if (prog->globals) {
        for (size_t i = 0; i < prog->global_count; i++) {
            free(prog->globals[i].name);
        }
        free(prog->globals);
    }
    
    if (prog->constants) free(prog->constants);
    if (prog->line_map) free(prog->line_map);
    if (prog->error_info.message) free(prog->error_info.message);
    
    free(prog);
}

/**
 * Find function in program
 */
int program_find_function(Program *prog, const char *name) {
    if (!prog || !name) return -1;
    
    for (size_t i = 0; i < prog->function_count; i++) {
        if (strcmp(prog->functions[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

/**
 * Find global variable in program
 */
int program_find_global(Program *prog, const char *name) {
    if (!prog || !name) return -1;
    
    for (size_t i = 0; i < prog->global_count; i++) {
        if (strcmp(prog->globals[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

/**
 * Get global variable value
 */
VMValue program_get_global(Program *prog, const char *name) {
    int idx = program_find_global(prog, name);
    if (idx >= 0) {
        return prog->globals[idx].value;
    }
    return (VMValue){.type = VALUE_NULL};
}

/**
 * Set global variable value
 */
void program_set_global(Program *prog, const char *name, VMValue value) {
    int idx = program_find_global(prog, name);
    if (idx >= 0) {
        prog->globals[idx].value = value;
    }
}

/**
 * Get error string
 */
const char* compiler_error_string(compile_error_t error) {
    switch (error) {
        case COMPILE_SUCCESS:
            return "Success";
        case COMPILE_ERROR_IO:
            return "I/O Error";
        case COMPILE_ERROR_SYNTAX:
            return "Syntax Error";
        case COMPILE_ERROR_SEMANTIC:
            return "Semantic Error";
        case COMPILE_ERROR_UNKNOWN:
            return "Unknown Error";
        default:
            return "Invalid Error Code";
    }
}

/**
 * Print compilation error
 */
void compiler_print_error(Program *prog) {
    if (!prog || prog->last_error == COMPILE_SUCCESS) {
        return;
    }
    
    fprintf(stderr, "%s:%d:%d: %s: %s\n",
            prog->error_info.filename ?: "<unknown>",
            prog->error_info.line,
            prog->error_info.column,
            compiler_error_string(prog->last_error),
            prog->error_info.message ?: "");
}
