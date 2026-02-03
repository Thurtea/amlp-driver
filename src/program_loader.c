/*
 * program_loader.c - Program Loader Implementation
 * 
 * Bridges the compiler and VM by converting compiled Program
 * structures into VM-executable instructions and functions.
 */

/* Ensure strdup is available (POSIX) */
#define _POSIX_C_SOURCE 200809L

#include "program_loader.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* Helper: Read uint8 from bytecode */
static uint8_t read_u8(const uint8_t *bytecode, size_t *offset) {
    return bytecode[(*offset)++];
}

/* Helper: Read uint16 from bytecode (little-endian to match compiler) */
static uint16_t read_u16(const uint8_t *bytecode, size_t *offset) {
    uint16_t lo = bytecode[(*offset)++];
    uint16_t hi = bytecode[(*offset)++];
    return (hi << 8) | lo;
}

/* Helper: Read int64/long from bytecode (little-endian, 8 bytes) */
static long read_i64(const uint8_t *bytecode, size_t *offset) {
    uint64_t result = 0;
    for (int i = 0; i < 8; i++) {
        result |= ((uint64_t)bytecode[(*offset)++]) << (i * 8);
    }
    return (long)result;
}

/* Helper: Read float64 from bytecode (little-endian to match compiler) */
static double read_f64(const uint8_t *bytecode, size_t *offset) {
    union {
        uint64_t u;
        double d;
    } converter;
    
    converter.u = 0;
    for (int i = 0; i < 8; i++) {
        converter.u |= ((uint64_t)bytecode[(*offset)++]) << (i * 8);
    }
    return converter.d;
}

static int program_line_for_offset(Program *program, size_t offset) {
    if (!program || !program->line_map || program->line_map_count == 0) {
        return -1;
    }

    int line = -1;
    for (size_t i = 0; i < program->line_map_count; i++) {
        if (program->line_map[i].bytecode_offset > offset) {
            break;
        }
        line = program->line_map[i].source_line;
    }

    return line;
}

int program_loader_decode_instruction(const uint8_t *bytecode, size_t offset, VMInstruction *instr) {
    if (!bytecode || !instr) return -1;
    
    size_t start_offset = offset;
    OpCode opcode = (OpCode)read_u8(bytecode, &offset);
    instr->opcode = opcode;
    
    /* Decode operands based on opcode */
    switch (opcode) {
        case OP_PUSH_INT:
            instr->operand.int_operand = read_i64(bytecode, &offset);
            break;
            
        case OP_PUSH_FLOAT:
            instr->operand.float_operand = read_f64(bytecode, &offset);
            break;
            
        case OP_PUSH_STRING: {
            /* String operand: 2 bytes length (little-endian) + string data */
            uint8_t len_lo = read_u8(bytecode, &offset);
            uint8_t len_hi = read_u8(bytecode, &offset);
            uint16_t str_len = len_lo | (len_hi << 8);
            char *str = (char*)malloc(str_len + 1);
            if (!str) return -1;
            memcpy(str, &bytecode[offset], str_len);
            str[str_len] = '\0';
            offset += str_len;
            instr->operand.string_operand = str;
            break;
        }
            
        case OP_LOAD_LOCAL:
        case OP_STORE_LOCAL:
        case OP_LOAD_GLOBAL:
        case OP_STORE_GLOBAL:
        case OP_MAKE_ARRAY:
        case OP_MAKE_MAPPING:
        case OP_CALL_METHOD:
            instr->operand.int_operand = read_u16(bytecode, &offset);
            break;
            
        case OP_JUMP:
        case OP_JUMP_IF_FALSE:
        case OP_JUMP_IF_TRUE:
            instr->operand.address_operand = read_u16(bytecode, &offset);
            break;
            
        case OP_CALL: {
            /* Compiler format: arg_count(1) + name_length(1) + name_bytes */
            uint8_t arg_count = read_u8(bytecode, &offset);
            uint8_t name_len = read_u8(bytecode, &offset);
            char *name = (char*)malloc(name_len + 1);
            if (!name) return -1;
            memcpy(name, &bytecode[offset], name_len);
            name[name_len] = '\0';
            offset += name_len;
            instr->operand.call_operand.arg_count = arg_count;
            instr->operand.call_operand.target = 0;  /* Will be resolved at runtime */
            instr->operand.call_operand.name = name;  /* Store function name for lookup */
            break;
        }
            
        /* Opcodes with no operands */
        case OP_PUSH_NULL:
        case OP_POP:
        case OP_DUP:
        case OP_ADD:
        case OP_SUB:
        case OP_MUL:
        case OP_DIV:
        case OP_MOD:
        case OP_NEG:
        case OP_EQ:
        case OP_NE:
        case OP_LT:
        case OP_LE:
        case OP_GT:
        case OP_GE:
        case OP_AND:
        case OP_OR:
        case OP_NOT:
        case OP_BIT_AND:
        case OP_BIT_OR:
        case OP_BIT_XOR:
        case OP_BIT_NOT:
        case OP_LSHIFT:
        case OP_RSHIFT:
        case OP_RETURN:
        case OP_INDEX_ARRAY:
        case OP_STORE_ARRAY:
        case OP_INDEX_MAPPING:
        case OP_STORE_MAPPING:
        case OP_HALT:
        case OP_PRINT:
            /* No operands */
            break;
            
        default:
            fprintf(stderr, "[program_loader] Unknown opcode: %d\n", opcode);
            return -1;
    }
    
    return (int)(offset - start_offset);
}

int program_loader_load(VirtualMachine *vm, Program *program) {
    if (!vm || !program || !program->bytecode) {
        fprintf(stderr, "[program_loader] Invalid arguments\n");
        return -1;
    }

    /* Debug: dump the Program's source as seen by the loader */
    {
        char dump_path[256];
        snprintf(dump_path, sizeof(dump_path), "/tmp/amlp_program_load_%d.lpc", (int)getpid());
        FILE *d = fopen(dump_path, "a");
        if (d) {
            fprintf(d, "/* program filename: %s */\n", program->filename ? program->filename : "<null>");
            if (program->source) {
                fwrite(program->source, 1, strlen(program->source), d);
                fprintf(d, "\n/* EOF */\n\n");
            } else {
                fprintf(d, "/* NO SOURCE AVAILABLE */\n\n");
            }
            fclose(d);
        }
    }
    
    /* Step 1: Decode bytecode into VMInstructions */
    VMInstruction *instructions = NULL;
    int instruction_count = 0;
    int instruction_capacity = 256;
    
    instructions = (VMInstruction*)malloc(sizeof(VMInstruction) * instruction_capacity);
    if (!instructions) {
        fprintf(stderr, "[program_loader] Out of memory for instructions\n");
        return -1;
    }
    
    size_t offset = 0;
    while (offset < program->bytecode_len) {
        if (instruction_count >= instruction_capacity) {
            instruction_capacity *= 2;
            VMInstruction *new_instructions = (VMInstruction*)realloc(
                instructions,
                sizeof(VMInstruction) * instruction_capacity
            );
            if (!new_instructions) {
                free(instructions);
                fprintf(stderr, "[program_loader] Out of memory expanding instructions\n");
                return -1;
            }
            instructions = new_instructions;
        }
        
        VMInstruction instr;
        int bytes_read = program_loader_decode_instruction(
            program->bytecode,
            offset,
            &instr
        );
        
        if (bytes_read < 0) {
            free(instructions);
            fprintf(stderr, "[program_loader] Failed to decode instruction at offset %zu\n", offset);
            return -1;
        }
        
        instructions[instruction_count++] = instr;
        offset += bytes_read;
    }
    
    /* Step 2: Load top-level bytecode into VM */
    if (vm_load_bytecode(vm, instructions, instruction_count) != 0) {
        free(instructions);
        fprintf(stderr, "[program_loader] Failed to load bytecode into VM\n");
        return -1;
    }
    
    /* Step 3: Create VMFunctions from function table */
    for (size_t i = 0; i < program->function_count; i++) {
        VMFunction *func = (VMFunction*)malloc(sizeof(VMFunction));
        if (!func) {
            fprintf(stderr, "[program_loader] Out of memory creating function %zu\n", i);
            return -1;
        }
        
        func->name = strdup(program->functions[i].name);
        func->param_count = program->functions[i].arg_count;
        func->local_var_count = program->functions[i].local_count;
        func->source_file = program->filename ? strdup(program->filename) : NULL;
        func->line_map = NULL;
        func->line_map_count = 0;
        
        /* Extract function bytecode from main bytecode */
        uint16_t func_offset = program->functions[i].offset;
        if (func_offset >= program->bytecode_len) {
            fprintf(stderr, "[program_loader] Function %s offset %u beyond bytecode (%zu)\n",
                    program->functions[i].name, func_offset, program->bytecode_len);
            free(func->name);
            free(func);
            return -1;
        }
        
        /* Find function end (next function start or end of bytecode) */
        uint16_t func_end = (i + 1 < program->function_count)
            ? program->functions[i + 1].offset
            : program->bytecode_len;
        if (func_end > program->bytecode_len) {
            func_end = program->bytecode_len;
        }
        
        /* Decode function instructions */
        int func_instr_capacity = 64;
        func->instructions = (VMInstruction*)malloc(sizeof(VMInstruction) * func_instr_capacity);
        func->instruction_count = 0;
        func->instruction_capacity = func_instr_capacity;
        int *line_map = (int *)malloc(sizeof(int) * func_instr_capacity);
        
        if (!func->instructions) {
            free(func->name);
            if (func->source_file) free(func->source_file);
            free(func);
            fprintf(stderr, "[program_loader] Out of memory for function instructions\n");
            return -1;
        }
        
        size_t func_offset_current = func_offset;
        while (func_offset_current < func_end) {
            if (func->instruction_count >= func->instruction_capacity) {
                func->instruction_capacity *= 2;
                VMInstruction *new_instr = (VMInstruction*)realloc(
                    func->instructions,
                    sizeof(VMInstruction) * func->instruction_capacity
                );
                if (!new_instr) {
                    free(func->instructions);
                    if (line_map) free(line_map);
                    free(func->name);
                    if (func->source_file) free(func->source_file);
                    free(func);
                    fprintf(stderr, "[program_loader] Out of memory expanding function instructions\n");
                    return -1;
                }
                func->instructions = new_instr;
                if (line_map) {
                    int *new_map = (int *)realloc(line_map, sizeof(int) * func->instruction_capacity);
                    if (!new_map) {
                        free(func->instructions);
                        free(line_map);
                        free(func->name);
                        if (func->source_file) free(func->source_file);
                        free(func);
                        fprintf(stderr, "[program_loader] Out of memory expanding line map\n");
                        return -1;
                    }
                    line_map = new_map;
                }
            }
            
            VMInstruction instr;
            size_t instr_offset = func_offset_current;
            int bytes_read = program_loader_decode_instruction(
                program->bytecode,
                func_offset_current,
                &instr
            );
            
            if (bytes_read < 0) {
                free(func->instructions);
                if (line_map) free(line_map);
                free(func->name);
                if (func->source_file) free(func->source_file);
                free(func);
                fprintf(stderr, "[program_loader] Failed to decode function instruction at offset %zu\n",
                        func_offset_current);
                return -1;
            }
            
            func->instructions[func->instruction_count++] = instr;
            if (line_map) {
                line_map[func->instruction_count - 1] = program_line_for_offset(program, instr_offset);
            }
            func_offset_current += bytes_read;
        }

        func->line_map = line_map;
        func->line_map_count = line_map ? func->instruction_count : 0;
        
        /* Add function to VM */
        if (vm_add_function(vm, func) < 0) {
            free(func->instructions);
            if (func->line_map) free(func->line_map);
            free(func->name);
            if (func->source_file) free(func->source_file);
            free(func);
            fprintf(stderr, "[program_loader] Failed to add function to VM\n");
            return -1;
        }
    }
    
    /* Step 4: Load globals into VM */
    for (size_t i = 0; i < program->global_count; i++) {
        /* Initialize global variables in VM's global storage */
        if (i >= (size_t)vm->global_capacity) {
            /* Expand global storage if needed */
            int new_capacity = vm->global_capacity * 2;
            if (new_capacity < (int)(i + 1)) {
                new_capacity = i + 1;
            }
            
            VMValue *new_globals = (VMValue*)realloc(
                vm->global_variables,
                sizeof(VMValue) * new_capacity
            );
            if (!new_globals) {
                fprintf(stderr, "[program_loader] Out of memory expanding globals\n");
                return -1;
            }
            
            vm->global_variables = new_globals;
            vm->global_capacity = new_capacity;
        }
        
        /* Initialize global with default value */
        vm->global_variables[i] = program->globals[i].value;
        if (i >= (size_t)vm->global_count) {
            vm->global_count = i + 1;
        }
    }
    
    /* Step 5: Load constants into VM string pool */
    for (size_t i = 0; i < program->constant_count; i++) {
        if (program->constants[i].type == VALUE_STRING) {
            /* Add string constant to VM string pool */
            if (vm->string_pool_count >= vm->string_pool_capacity) {
                int new_capacity = vm->string_pool_capacity * 2;
                char **new_pool = (char**)realloc(
                    vm->string_pool,
                    sizeof(char*) * new_capacity
                );
                if (!new_pool) {
                    fprintf(stderr, "[program_loader] Out of memory expanding string pool\n");
                    return -1;
                }
                vm->string_pool = new_pool;
                vm->string_pool_capacity = new_capacity;
            }
            
            vm->string_pool[vm->string_pool_count++] = 
                strdup(program->constants[i].data.string_value);
        }
    }
    
    printf("[program_loader] Loaded program: %d instructions, %zu functions, %zu globals\n",
           instruction_count, program->function_count, program->global_count);
    
    if (vm->function_count > 0) {
        printf("[program_loader] Functions registered:\n");
        for (int i = 0; i < vm->function_count; i++) {
            if (vm->functions[i]) {
                printf("  [%d] %s (%d params, %d locals, %d instructions)\n",
                       i,
                       vm->functions[i]->name,
                       vm->functions[i]->param_count,
                       vm->functions[i]->local_var_count,
                       vm->functions[i]->instruction_count);
            }
        }
    }
    
    return 0;
}

int program_loader_find_entry(VirtualMachine *vm, Program *program, const char *entry_name) {
    if (!vm || !program || !entry_name) return -1;
    
    /* Default entry point is "create" */
    const char *search_name = entry_name ? entry_name : "create";
    
    /* Search function table in Program */
    int func_idx = program_find_function(program, search_name);
    if (func_idx < 0) {
        fprintf(stderr, "[program_loader] Entry function '%s' not found\n", search_name);
        return -1;
    }
    
    return func_idx;
}
