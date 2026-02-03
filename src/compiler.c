#include "compiler.h"
#include "lexer.h"
#include "parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>

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
        ASTNode *ast_node;  /* Pointer to function declaration AST */
    } *functions;
    size_t function_count;
    size_t function_capacity;
    int current_function_idx;  /* Index of function currently being compiled */
    
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
    /* CRITICAL: Use calloc to zero all memory - prevents uninitialized state */
    compiler_state_t *state = calloc(1, sizeof(compiler_state_t));
    if (!state) return NULL;

    /* Explicitly initialize all fields */
    state->error_count = 0;

    state->bytecode_capacity = INITIAL_BYTECODE_SIZE;
    state->bytecode = malloc(state->bytecode_capacity);
    state->bytecode_len = 0;

    state->function_capacity = INITIAL_FUNCTION_COUNT;
    state->functions = malloc(sizeof(state->functions[0]) * state->function_capacity);
    state->function_count = 0;
    state->current_function_idx = -1;  /* No function being compiled yet */

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
 */
static void compiler_add_function(compiler_state_t *state, const char *name,
                                 uint16_t offset, uint8_t arg_count,
                                 uint8_t local_count, ASTNode *ast_node) {
    if (!state || !name) return;

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
    state->functions[index].ast_node = ast_node;
}

/**
 * Add global variable to program
 */
static void compiler_add_global(compiler_state_t *state, const char *name) {
    if (!state || !name) return;

    if (state->global_count >= state->global_capacity) {
        state->global_capacity *= 2;
        state->globals = realloc(state->globals,
                                sizeof(state->globals[0]) * state->global_capacity);
    }
    
    int index = state->global_count++;
    state->globals[index].name = malloc(strlen(name) + 1);
    strcpy(state->globals[index].name, name);
    state->globals[index].value.type = VALUE_INT;
    state->globals[index].value.data.int_value = 0;
}

/**
 * Add constant to program
 */
static void compiler_add_constant(compiler_state_t *state, VMValue value) __attribute__((unused));
static void compiler_add_constant(compiler_state_t *state, VMValue value) {
    if (!state) return;

    if (state->constant_count >= state->constant_capacity) {
        state->constant_capacity *= 2;
        state->constants = realloc(state->constants,
                                  sizeof(VMValue) * state->constant_capacity);
    }
    
    state->constants[state->constant_count++] = value;
}

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
 * Forward declarations for recursive bytecode generation
 */
static void compiler_codegen_statement(compiler_state_t *state, ASTNode *node);
static void compiler_codegen_expression(compiler_state_t *state, ASTNode *node);

/**
 * Generate bytecode for an expression
 * Leaves the value on the stack
 */
static void compiler_codegen_expression(compiler_state_t *state, ASTNode *node) {
    if (!state || !node) {
        return;
    }

    switch (node->type) {
        case NODE_LITERAL_NUMBER: {
            LiteralNumberNode *num = (LiteralNumberNode *)node->data;
            if (num->is_float) {
                compiler_emit(state, OP_PUSH_FLOAT, node->line);
                // Encode float value as bytes (simplified: store as bits)
                double val = num->float_value;
                uint64_t bits;
                memcpy(&bits, &val, sizeof(double));
                for (int i = 0; i < 8; i++) {
                    compiler_emit(state, (bits >> (i * 8)) & 0xFF, node->line);
                }
            } else {
                compiler_emit(state, OP_PUSH_INT, node->line);
                // Encode int value as 8 bytes (64-bit)
                long val = num->int_value;
                for (int i = 0; i < 8; i++) {
                    compiler_emit(state, (val >> (i * 8)) & 0xFF, node->line);
                }
            }
            break;
        }

        case NODE_LITERAL_STRING: {
            LiteralStringNode *str = (LiteralStringNode *)node->data;
            if (str && str->value) {
                compiler_emit(state, OP_PUSH_STRING, node->line);
                // Store string length (2 bytes)
                size_t len = strlen(str->value);
                compiler_emit(state, len & 0xFF, node->line);
                compiler_emit(state, (len >> 8) & 0xFF, node->line);
                // Store string bytes
                for (size_t i = 0; i < len; i++) {
                    compiler_emit(state, (unsigned char)str->value[i], node->line);
                }
            }
            break;
        }

        case NODE_IDENTIFIER: {
            IdentifierNode *id = (IdentifierNode *)node->data;
            if (id && id->name) {
                // Check if we're in a function and if this identifier is a local/param
                int local_idx = -1;
                if (state->current_function_idx >= 0) {
                    ASTNode *fn_node = state->functions[state->current_function_idx].ast_node;
                    if (fn_node && fn_node->data) {
                        FunctionDeclNode *fn = (FunctionDeclNode *)fn_node->data;
                        // Check parameters
                        for (int i = 0; i < fn->parameter_count; i++) {
                            if (fn->parameters[i].name && strcmp(fn->parameters[i].name, id->name) == 0) {
                                local_idx = i;
                                break;
                            }
                        }
                    }
                }
                
                if (local_idx >= 0) {
                    // It's a local variable/parameter - use LOAD_LOCAL
                    compiler_emit(state, OP_LOAD_LOCAL, node->line);
                    compiler_emit(state, local_idx, node->line);
                } else {
                    // It's a global variable - use LOAD_GLOBAL
                    compiler_emit(state, OP_LOAD_GLOBAL, node->line);
                    // Emit global index (simplified: use first 2 bytes for index)
                    compiler_emit(state, 0, node->line);
                    compiler_emit(state, 0, node->line);
                }
            }
            break;
        }

        case NODE_FUNCTION_CALL: {
            FunctionCallNode *call = (FunctionCallNode *)node->data;
            if (call && call->function_name) {
                // Emit arguments first
                for (int i = 0; i < call->argument_count; i++) {
                    compiler_codegen_expression(state, call->arguments[i]);
                }
                // Emit call instruction
                compiler_emit(state, OP_CALL, node->line);
                compiler_emit(state, call->argument_count & 0xFF, node->line);
                // Function name length and bytes
                size_t len = strlen(call->function_name);
                compiler_emit(state, len & 0xFF, node->line);
                for (size_t i = 0; i < len; i++) {
                    compiler_emit(state, (unsigned char)call->function_name[i], node->line);
                }
            }
            break;
        }

        case NODE_BINARY_OP: {
            BinaryOpNode *binop = (BinaryOpNode *)node->data;
            if (binop && binop->left && binop->right && binop->operator) {
                // Emit left operand
                compiler_codegen_expression(state, binop->left);
                // Emit right operand
                compiler_codegen_expression(state, binop->right);
                
                // Emit appropriate opcode based on operator
                if (strcmp(binop->operator, "+") == 0) {
                    compiler_emit(state, OP_ADD, node->line);
                } else if (strcmp(binop->operator, "-") == 0) {
                    compiler_emit(state, OP_SUB, node->line);
                } else if (strcmp(binop->operator, "*") == 0) {
                    compiler_emit(state, OP_MUL, node->line);
                } else if (strcmp(binop->operator, "/") == 0) {
                    compiler_emit(state, OP_DIV, node->line);
                } else if (strcmp(binop->operator, "%") == 0) {
                    compiler_emit(state, OP_MOD, node->line);
                } else if (strcmp(binop->operator, "==") == 0) {
                    compiler_emit(state, OP_EQ, node->line);
                } else if (strcmp(binop->operator, "!=") == 0) {
                    compiler_emit(state, OP_NE, node->line);
                } else if (strcmp(binop->operator, "<") == 0) {
                    compiler_emit(state, OP_LT, node->line);
                } else if (strcmp(binop->operator, ">") == 0) {
                    compiler_emit(state, OP_GT, node->line);
                } else if (strcmp(binop->operator, "<=") == 0) {
                    compiler_emit(state, OP_LE, node->line);
                } else if (strcmp(binop->operator, ">=") == 0) {
                    compiler_emit(state, OP_GE, node->line);
                } else if (strcmp(binop->operator, "&&") == 0) {
                    compiler_emit(state, OP_AND, node->line);
                } else if (strcmp(binop->operator, "||") == 0) {
                    compiler_emit(state, OP_OR, node->line);
                }
            }
            break;
        }

        case NODE_UNARY_OP: {
            UnaryOpNode *unop = (UnaryOpNode *)node->data;
            if (unop && unop->operand && unop->operator) {
                compiler_codegen_expression(state, unop->operand);
                if (strcmp(unop->operator, "-") == 0) {
                    compiler_emit(state, OP_NEG, node->line);
                } else if (strcmp(unop->operator, "!") == 0) {
                    compiler_emit(state, OP_NOT, node->line);
                }
            }
            break;
        }

        default:
            // Unsupported expression type - push null as placeholder
            compiler_emit(state, OP_PUSH_NULL, node->line);
            break;
    }
}

/**
 * Generate bytecode for a statement
 */
static void compiler_codegen_statement(compiler_state_t *state, ASTNode *node) {
    if (!state || !node) {
        return;
    }

    switch (node->type) {
        case NODE_BLOCK: {
            BlockNode *block = (BlockNode *)node->data;
            if (block) {
                for (int i = 0; i < block->statement_count; i++) {
                    compiler_codegen_statement(state, block->statements[i]);
                }
            }
            break;
        }

        case NODE_RETURN_STATEMENT: {
            ReturnStatementNode *ret = (ReturnStatementNode *)node->data;
            if (ret && ret->value) {
                compiler_codegen_expression(state, ret->value);
            } else {
                compiler_emit(state, OP_PUSH_NULL, node->line);
            }
            compiler_emit(state, OP_RETURN, node->line);
            break;
        }

        case NODE_EXPRESSION_STATEMENT: {
            // Just evaluate the expression and pop the result
            if (node->data) {
                ASTNode *expr = (ASTNode *)node->data;
                compiler_codegen_expression(state, expr);
                compiler_emit(state, OP_POP, node->line);
            }
            break;
        }

        case NODE_IF_STATEMENT: {
            IfStatementNode *ifstmt = (IfStatementNode *)node->data;
            if (ifstmt && ifstmt->condition && ifstmt->then_statement) {
                // Generate condition bytecode
                compiler_codegen_expression(state, ifstmt->condition);
                
                // Emit jump-if-false (placeholder offset, will need patching)
                int jump_false_addr = state->bytecode_len;
                compiler_emit(state, OP_JUMP_IF_FALSE, node->line);
                compiler_emit(state, 0, node->line);  // Offset placeholder
                compiler_emit(state, 0, node->line);
                
                // Generate then-branch bytecode
                compiler_codegen_statement(state, ifstmt->then_statement);
                
                // If there's an else branch, jump over it
                int jump_end_addr = -1;
                if (ifstmt->else_statement) {
                    jump_end_addr = state->bytecode_len;
                    compiler_emit(state, OP_JUMP, node->line);
                    compiler_emit(state, 0, node->line);  // Offset placeholder
                    compiler_emit(state, 0, node->line);
                }
                
                // Patch the jump-false offset
                int false_target = state->bytecode_len;
                state->bytecode[jump_false_addr + 1] = false_target & 0xFF;
                state->bytecode[jump_false_addr + 2] = (false_target >> 8) & 0xFF;
                
                // Generate else-branch if present
                if (ifstmt->else_statement) {
                    compiler_codegen_statement(state, ifstmt->else_statement);
                    
                    // Patch the jump-end offset
                    int end_target = state->bytecode_len;
                    state->bytecode[jump_end_addr + 1] = end_target & 0xFF;
                    state->bytecode[jump_end_addr + 2] = (end_target >> 8) & 0xFF;
                }
            }
            break;
        }

        default:
            // Unhandled statement type - silently skip
            break;
    }
}

/**
 * Compile bytecode from AST
 * Generates real bytecode for all functions in the program
 */
static compile_error_t compiler_generate_bytecode(compiler_state_t *state,
                                                  ASTNode *ast) {
    if (!state || !ast || ast->type != NODE_PROGRAM) {
        return COMPILE_SUCCESS;
    }

    // Generate bytecode for each registered function
    for (size_t i = 0; i < state->function_count; i++) {
        if (!state->functions[i].ast_node) {
            continue;
        }

        ASTNode *decl = state->functions[i].ast_node;
        FunctionDeclNode *fn = (FunctionDeclNode *)decl->data;
        if (!fn || !fn->body) {
            continue;
        }

        // Record offset for this function
        state->functions[i].offset = (uint16_t)state->bytecode_len;
        
        // Set current function context for variable resolution
        state->current_function_idx = (int)i;

        // Generate bytecode for function body
        if (fn->body->type == NODE_BLOCK) {
            compiler_codegen_statement(state, fn->body);
        } else {
            compiler_codegen_statement(state, fn->body);
        }

        // Ensure function ends with return (if not already present)
        if (state->bytecode_len == 0 || state->bytecode[state->bytecode_len - 1] != OP_RETURN) {
            compiler_emit(state, OP_PUSH_NULL, fn->body->line);
            compiler_emit(state, OP_RETURN, fn->body->line);
        }
        
        // Clear current function context
        state->current_function_idx = -1;
    }

    // If no bytecode was generated, emit a minimal program (just return)
    if (state->bytecode_len == 0) {
        compiler_emit(state, OP_PUSH_NULL, 1);
        compiler_emit(state, OP_RETURN, 1);
    }

    return state->error_count > 0 ? COMPILE_ERROR_SEMANTIC : COMPILE_SUCCESS;
}

/**
 * Extract top-level metadata (functions, globals) from AST
 */
static void compiler_extract_metadata(compiler_state_t *state, ASTNode *ast) {
    if (!state || !ast) return;

    if (ast->type != NODE_PROGRAM) {
        return;
    }

    ProgramNode *prog = (ProgramNode *)ast->data;
    if (!prog) return;

    for (int i = 0; i < prog->declaration_count; i++) {
        ASTNode *decl = prog->declarations[i];
        if (!decl) continue;

        if (decl->type == NODE_FUNCTION_DECL) {
            FunctionDeclNode *fn = (FunctionDeclNode *)decl->data;
            if (!fn || !fn->name) continue;
            uint8_t arg_count = (fn->parameter_count < 0) ? 0 : (fn->parameter_count > 255 ? 255 : (uint8_t)fn->parameter_count);
            compiler_add_function(state, fn->name, /* offset */ 0, arg_count, /* local_count */ 0, decl);
        } else if (decl->type == NODE_VARIABLE_DECL) {
            VariableDeclNode *var = (VariableDeclNode *)decl->data;
            if (!var || !var->name) continue;
            compiler_add_global(state, var->name);
        }
    }
}

/**
 * Compile LPC source code
 */
static Program* compiler_compile_internal(const char *source, const char *filename) {
    if (!source || !filename) {
        return NULL;
    }
    /* Debug dump: write raw source to temporary file for inspection */
    {
        FILE *dbg = fopen("/tmp/amlp_last_loaded_source.lpc", "w");
        if (dbg) {
            fprintf(dbg, "/* source for: %s */\n", filename);
            fwrite(source, 1, strlen(source), dbg);
            fclose(dbg);
        }
    }
    
    // Initialize compiler state
    compiler_state_t *state = compiler_state_new();
    if (!state) return NULL;
    
    // Lexical analysis
    Lexer *lexer = lexer_init_from_string(source);
    /* Dump token stream for debugging parser issues */
    if (lexer) {
        char tokens_path[256];
        snprintf(tokens_path, sizeof(tokens_path), "/tmp/amlp_tokens_%d.log", (int)getpid());
        FILE *tf = fopen(tokens_path, "a");
        if (tf) {
            Lexer *tmp = lexer_init_from_string(source);
            if (tmp) {
                fprintf(tf, "--- tokens for: %s (pid=%d) ---\n", filename, (int)getpid());
                while (1) {
                    Token tok = lexer_get_next_token(tmp);
                    if (tok.value) {
                        fprintf(tf, "%s:%d:%d: %s\n", token_type_to_string(tok.type), tok.line_number, tok.column_number, tok.value);
                        free(tok.value);
                    } else {
                        fprintf(tf, "%s:%d:%d:\n", token_type_to_string(tok.type), tok.line_number, tok.column_number);
                    }
                    if (tok.type == TOKEN_EOF) break;
                }
                lexer_free(tmp);
                fprintf(tf, "--- end tokens ---\n\n");
            }
            fclose(tf);
        }
    }
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
    
    // Collect top-level metadata (functions/globals) before codegen
    compiler_extract_metadata(state, ast);
    
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
            .filename = prog->filename,
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
