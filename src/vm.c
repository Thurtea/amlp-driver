/*
 * vm.c - Virtual Machine Implementation
 * 
 * This file implements the bytecode execution engine for the LPC MUD driver.
 * It provides a stack-based virtual machine that executes compiled bytecode,
 * manages memory for LPC values, and handles function calls.
 */

#define _POSIX_C_SOURCE 200809L

#include "vm.h"
#include "efun.h"
#include "object.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "array.h"
#include "mapping.h"

/* ========== Constants ========== */

#define VM_STACK_SIZE       1024
#define VM_FUNCTIONS_INIT   64
#define VM_GLOBALS_INIT     256
#define VM_STRING_POOL_INIT 128
#define VM_MAPPING_BUCKETS  16

typedef struct {
    int refcount;
    size_t length;
    char data[];
} VMStringHeader;

static VMStringHeader *vm_string_header(const char *data) {
    return (VMStringHeader *)((char *)data - offsetof(VMStringHeader, data));
}

static char *vm_string_create(const char *value, size_t len) {
    VMStringHeader *hdr = (VMStringHeader *)malloc(sizeof(VMStringHeader) + len + 1);
    if (!hdr) return NULL;
    hdr->refcount = 1;
    hdr->length = len;
    memcpy(hdr->data, value, len);
    hdr->data[len] = '\0';
    return hdr->data;
}

/**
 * Initialize the virtual machine
 */
VirtualMachine* vm_init(void) {
    VirtualMachine *vm = (VirtualMachine *)malloc(sizeof(VirtualMachine));
    if (!vm) {
        fprintf(stderr, "Fatal: Memory allocation failed for VM\n");
        return NULL;
    }

    vm->stack = (VMStack *)malloc(sizeof(VMStack));
    if (!vm->stack) {
        free(vm);
        return NULL;
    }

    vm->stack->capacity = VM_STACK_SIZE;
    vm->stack->values = (VMValue *)malloc(sizeof(VMValue) * vm->stack->capacity);
    vm->stack->top = 0;
    
    /* Initialize all stack values */
    for (int i = 0; i < vm->stack->capacity; i++) {
        vm->stack->values[i].type = VALUE_UNINITIALIZED;
    }

    vm->current_frame = NULL;
    vm->running = 0;
    vm->error_count = 0;
    
    vm->efun_registry = efun_init();
    if (!vm->efun_registry) {
        fprintf(stderr, "Warning: Efun registry initialization failed\n");
    } else {
        /* Register all built-in efuns */
        if (efun_register_all(vm->efun_registry) != 0) {
            fprintf(stderr, "Warning: Some efuns failed to register\n");
        }
    }
    
    vm->function_capacity = VM_FUNCTIONS_INIT;
    vm->function_count = 0;
    vm->functions = (VMFunction **)malloc(sizeof(VMFunction *) * vm->function_capacity);
    
    vm->global_capacity = VM_GLOBALS_INIT;
    vm->global_count = 0;
    vm->global_variables = (VMValue *)malloc(sizeof(VMValue) * vm->global_capacity);
    for (int i = 0; i < vm->global_capacity; i++) {
        vm->global_variables[i].type = VALUE_UNINITIALIZED;
    }
    
    vm->string_pool_capacity = VM_STRING_POOL_INIT;
    vm->string_pool_count = 0;
    vm->string_pool = (char **)malloc(sizeof(char *) * vm->string_pool_capacity);
    
    vm->instructions = NULL;
    vm->instruction_count = 0;
    vm->instruction_capacity = 0;
    vm->instruction_pointer = 0;

    vm->debug_flags = 0;
    vm->trace_output = NULL;
    memset(&vm->profile, 0, sizeof(vm->profile));

    vm->gc = gc_init();
    if (!vm->gc) {
        fprintf(stderr, "Fatal: GC initialization failed for VM\n");
        vm_free(vm);
        return NULL;
    }

    vm_debug_init(vm);

    printf("[VM] Virtual machine initialized\n");
    return vm;
}

/**
 * Load a program into the VM
 */
int vm_load_program(VirtualMachine *vm, ASTNode *program) {
    if (!vm || !program) {
        fprintf(stderr, "Error: Invalid VM or program\n");
        return -1;
    }

    printf("[VM] Program loaded successfully\n");
    return 0;
}

/**
 * Load bytecode instructions into the VM
 */
int vm_load_bytecode(VirtualMachine *vm, VMInstruction *instructions, int count) {
    if (!vm || !instructions || count <= 0) return -1;
    
    vm->instruction_capacity = count;
    vm->instructions = (VMInstruction *)malloc(sizeof(VMInstruction) * count);
    if (!vm->instructions) return -1;
    
    memcpy(vm->instructions, instructions, sizeof(VMInstruction) * count);
    vm->instruction_count = count;
    
    return 0;
}

/**
 * Add a function to the VM
 */
int vm_add_function(VirtualMachine *vm, VMFunction *function) {
    if (!vm || !function) return -1;
    
    if (vm->function_count >= vm->function_capacity) {
        vm->function_capacity *= 2;
        vm->functions = (VMFunction **)realloc(vm->functions,
                                                sizeof(VMFunction *) * vm->function_capacity);
    }
    
    vm->functions[vm->function_count] = function;
    return vm->function_count++;
}

/**
 * Value creation functions
 */
VMValue vm_value_create_int(long value) {
    VMValue v;
    v.type = VALUE_INT;
    v.data.int_value = value;
    vm_profile_note_create(v, 0);
    return v;
}

VMValue vm_value_create_float(double value) {
    VMValue v;
    v.type = VALUE_FLOAT;
    v.data.float_value = value;
    vm_profile_note_create(v, 0);
    return v;
}

VMValue vm_value_create_string(const char *value) {
    VMValue v;
    v.type = VALUE_STRING;
    if (value) {
        size_t len = strlen(value);
        v.data.string_value = vm_string_create(value, len);
        vm_profile_note_create(v, v.data.string_value ? len + 1 : 0);
    } else {
        v.data.string_value = NULL;
        vm_profile_note_create(v, 0);
    }
    return v;
}

VMValue vm_value_create_null(void) {
    VMValue v;
    v.type = VALUE_NULL;
    vm_profile_note_create(v, 0);
    return v;
}

void vm_value_addref(VMValue *value) {
    if (!value) return;
    if (value->type != VALUE_STRING || !value->data.string_value) return;

    VMStringHeader *hdr = vm_string_header(value->data.string_value);
    hdr->refcount++;
}

void vm_value_release(VMValue *value) {
    if (!value) return;
    if (value->type != VALUE_STRING || !value->data.string_value) return;

    VMStringHeader *hdr = vm_string_header(value->data.string_value);
    hdr->refcount--;
    if (hdr->refcount <= 0) {
        vm_profile_note_free(*value, hdr->length + 1);
        free(hdr);
    }

    value->data.string_value = NULL;
    value->type = VALUE_UNINITIALIZED;
}

void vm_value_free(VMValue *value) {
    if (!value) return;
    
    switch (value->type) {
        case VALUE_STRING:
            vm_value_release(value);
            return;
        case VALUE_ARRAY:
            if (value->data.array_value) {
                array_free((array_t *)value->data.array_value);
                value->data.array_value = NULL;
            }
            vm_profile_note_free(*value, 0);
            break;
        case VALUE_MAPPING:
            if (value->data.mapping_value) {
                mapping_free((mapping_t *)value->data.mapping_value);
                value->data.mapping_value = NULL;
            }
            vm_profile_note_free(*value, 0);
            break;
        default:
            vm_profile_note_free(*value, 0);
            break;
    }
    
    value->type = VALUE_UNINITIALIZED;
}

char* vm_value_to_string(VMValue value) {
    char *str = (char *)malloc(256);
    
    switch (value.type) {
        case VALUE_INT:
            snprintf(str, 256, "%ld", value.data.int_value);
            break;
        case VALUE_FLOAT:
            snprintf(str, 256, "%g", value.data.float_value);
            break;
        case VALUE_STRING:
            strncpy(str, value.data.string_value ? value.data.string_value : "(null)", 255);
            str[255] = '\0';
            break;
        case VALUE_NULL:
            strcpy(str, "null");
            break;
        case VALUE_ARRAY:
            strcpy(str, "array");
            break;
        case VALUE_MAPPING:
            strcpy(str, "mapping");
            break;
        case VALUE_OBJECT:
            strcpy(str, "object");
            break;
        default:
            strcpy(str, "uninitialized");
            break;
    }
    
    return str;
}

int vm_value_is_truthy(VMValue value) {
    switch (value.type) {
        case VALUE_NULL:
        case VALUE_UNINITIALIZED:
            return 0;
        case VALUE_INT:
            return value.data.int_value != 0;
        case VALUE_FLOAT:
            return value.data.float_value != 0.0;
        case VALUE_STRING:
            return value.data.string_value != NULL && strlen(value.data.string_value) > 0;
        case VALUE_ARRAY:
        case VALUE_MAPPING:
        case VALUE_OBJECT:
            return 1;
        default:
            return 0;
    }
}

VMValue vm_value_clone(VMValue value) {
    VMValue copy = value;

    switch (value.type) {
        case VALUE_STRING:
            if (value.data.string_value) {
                copy = vm_value_create_string(value.data.string_value);
            }
            break;
        case VALUE_ARRAY:
            if (value.data.array_value) {
                copy.type = VALUE_ARRAY;
                copy.data.array_value = array_clone((array_t *)value.data.array_value,
                                                    value.data.array_value->gc);
            }
            break;
        case VALUE_MAPPING:
            if (value.data.mapping_value) {
                copy.type = VALUE_MAPPING;
                copy.data.mapping_value = mapping_clone((mapping_t *)value.data.mapping_value,
                                                        value.data.mapping_value->gc);
            }
            break;
        default:
            break;
    }

    return copy;
}

/**
 * Function operations
 */
VMFunction* vm_function_create(const char *name, int param_count, int local_var_count) {
    VMFunction *func = (VMFunction *)malloc(sizeof(VMFunction));
    func->name = (char *)malloc(strlen(name) + 1);
    strcpy(func->name, name);
    func->param_count = param_count;
    func->local_var_count = local_var_count;
    func->instruction_count = 0;
    func->instruction_capacity = 32;
    func->instructions = (VMInstruction *)malloc(sizeof(VMInstruction) * func->instruction_capacity);
    func->source_file = NULL;
    func->line_map = NULL;
    func->line_map_count = 0;
    
    return func;
}

int vm_function_add_instruction(VMFunction *function, VMInstruction instruction) {
    if (!function) return -1;
    
    if (function->instruction_count >= function->instruction_capacity) {
        function->instruction_capacity *= 2;
        function->instructions = (VMInstruction *)realloc(function->instructions,
                                                          sizeof(VMInstruction) * function->instruction_capacity);
    }
    
    function->instructions[function->instruction_count++] = instruction;
    return function->instruction_count - 1;
}

void vm_function_free(VMFunction *function) {
    if (!function) return;
    
    if (function->name) free(function->name);
    if (function->instructions) free(function->instructions);
    if (function->source_file) free(function->source_file);
    if (function->line_map) free(function->line_map);
    free(function);
}

/**
 * Stack operations
 */
int vm_push_value(VirtualMachine *vm, VMValue value) {
    if (!vm || !vm->stack) return -1;
    
    if (vm->stack->top >= vm->stack->capacity) {
        fprintf(stderr, "Stack overflow\n");
        return -1;
    }
    
    vm_value_addref(&value);
    vm->stack->values[vm->stack->top++] = value;
    return 0;
}

VMValue vm_pop_value(VirtualMachine *vm) {
    if (!vm || !vm->stack || vm->stack->top <= 0) {
        return vm_value_create_null();
    }
    
    return vm->stack->values[--vm->stack->top];
}

VMValue vm_peek_value(VirtualMachine *vm) {
    if (!vm || !vm->stack || vm->stack->top <= 0) {
        return vm_value_create_null();
    }
    
    return vm->stack->values[vm->stack->top - 1];
}

/* ========== Arithmetic Operations ========== */

static void vm_arithmetic_op(VirtualMachine *vm, int op) {
    VMValue b = vm_pop_value(vm);
    VMValue a = vm_pop_value(vm);
    VMValue result;
    
    int a_is_float = (a.type == VALUE_FLOAT);
    int b_is_float = (b.type == VALUE_FLOAT);
    
    double a_val = a_is_float ? a.data.float_value : (double)a.data.int_value;
    double b_val = b_is_float ? b.data.float_value : (double)b.data.int_value;
    
    int result_is_float = a_is_float || b_is_float;
    
    double res_val = 0;
    switch (op) {
        case 0: res_val = a_val + b_val; break;
        case 1: res_val = a_val - b_val; break;
        case 2: res_val = a_val * b_val; break;
        case 3: 
            res_val = b_val != 0 ? a_val / b_val : 0; 
            result_is_float = 1;  // Division always returns float
            break;
        case 4: res_val = (long)a_val % (long)b_val; break;
    }
    
    if (result_is_float) {
        result = vm_value_create_float(res_val);
    } else {
        result = vm_value_create_int((long)res_val);
    }        
    
    vm_push_value(vm, result);
    vm_value_release(&a);
    vm_value_release(&b);
}

static void vm_negate(VirtualMachine *vm) {
    VMValue a = vm_pop_value(vm);
    VMValue result;
    
    if (a.type == VALUE_FLOAT) {
        result = vm_value_create_float(-a.data.float_value);
    } else {
        result = vm_value_create_int(-a.data.int_value);
    }
    
    vm_push_value(vm, result);
    vm_value_release(&a);
}

/* ========== Comparison Operations ========== */

static void vm_comparison_op(VirtualMachine *vm, int op) {
    VMValue b = vm_pop_value(vm);
    VMValue a = vm_pop_value(vm);
    
    double a_val = (a.type == VALUE_FLOAT) ? a.data.float_value : (double)a.data.int_value;
    double b_val = (b.type == VALUE_FLOAT) ? b.data.float_value : (double)b.data.int_value;
    
    long result = 0;
    switch (op) {
        case 0: result = (a_val == b_val); break;
        case 1: result = (a_val != b_val); break;
        case 2: result = (a_val < b_val); break;
        case 3: result = (a_val <= b_val); break;
        case 4: result = (a_val > b_val); break;
        case 5: result = (a_val >= b_val); break;
    }
    
    vm_push_value(vm, vm_value_create_int(result));
    vm_value_release(&a);
    vm_value_release(&b);
}

/* ========== Logical Operations ========== */

static void vm_logical_and(VirtualMachine *vm) {
    VMValue b = vm_pop_value(vm);
    VMValue a = vm_pop_value(vm);
    
    long result = vm_value_is_truthy(a) && vm_value_is_truthy(b);
    vm_push_value(vm, vm_value_create_int(result));
    vm_value_release(&a);
    vm_value_release(&b);
}

static void vm_logical_or(VirtualMachine *vm) {
    VMValue b = vm_pop_value(vm);
    VMValue a = vm_pop_value(vm);
    
    long result = vm_value_is_truthy(a) || vm_value_is_truthy(b);
    vm_push_value(vm, vm_value_create_int(result));
    vm_value_release(&a);
    vm_value_release(&b);
}

static void vm_logical_not(VirtualMachine *vm) {
    VMValue a = vm_pop_value(vm);
    long result = !vm_value_is_truthy(a);
    vm_push_value(vm, vm_value_create_int(result));
    vm_value_release(&a);
}

/* ========== Bitwise Operations ========== */

static void vm_bitwise_op(VirtualMachine *vm, int op) {
    VMValue b, a;
    long a_val, b_val;
    long result = 0;
    
    // NOT is unary, others are binary
    if (op == 3) {
        a = vm_pop_value(vm);
        a_val = (a.type == VALUE_INT) ? a.data.int_value : (long)a.data.float_value;
        result = ~a_val;
    } else {
        b = vm_pop_value(vm);
        a = vm_pop_value(vm);
        a_val = (a.type == VALUE_INT) ? a.data.int_value : (long)a.data.float_value;
        b_val = (b.type == VALUE_INT) ? b.data.int_value : (long)b.data.float_value;
        
        switch (op) {
            case 0: result = a_val & b_val; break;
            case 1: result = a_val | b_val; break;
            case 2: result = a_val ^ b_val; break;
            case 4: result = a_val << b_val; break;
            case 5: result = a_val >> b_val; break;
        }
    }
    
    vm_push_value(vm, vm_value_create_int(result));
    vm_value_release(&a);
    if (op != 3) {
        vm_value_release(&b);
    }
}

/* ========== VM Cleanup ========== */

void vm_free(VirtualMachine *vm) {
    if (!vm) return;
    
    if (vm->stack) {
        for (int i = 0; i < vm->stack->top; i++) {
            vm_value_free(&vm->stack->values[i]);
        }
        if (vm->stack->values) free(vm->stack->values);
        free(vm->stack);
    }
    
    if (vm->functions) {
        for (int i = 0; i < vm->function_count; i++) {
            if (vm->functions[i]) {
                vm_function_free(vm->functions[i]);
            }
        }
        free(vm->functions);
    }
    
    if (vm->global_variables) {
        for (int i = 0; i < vm->global_count; i++) {
            vm_value_free(&vm->global_variables[i]);
        }
        free(vm->global_variables);
    }
    
    if (vm->string_pool) {
        for (int i = 0; i < vm->string_pool_count; i++) {
            if (vm->string_pool[i]) {
                free(vm->string_pool[i]);
            }
        }
        free(vm->string_pool);
    }
    
    if (vm->instructions) {
        free(vm->instructions);
    }

    if (vm->gc) {
        gc_free(vm->gc);
    }

    if (vm->trace_output && vm->trace_output != stderr) {
        fclose(vm->trace_output);
        vm->trace_output = NULL;
    }

    free(vm);
    printf("[VM] Virtual machine freed\n");
}

/* ========== Instruction Dispatch ========== */

static int vm_execute_instruction(VirtualMachine *vm, VMInstruction *instr) {
    if (!vm || !instr) return -1;
    
    switch (instr->opcode) {
        case OP_PUSH_INT:
            return vm_push_value(vm, vm_value_create_int(instr->operand.int_operand));
        
        case OP_PUSH_FLOAT:
            return vm_push_value(vm, vm_value_create_float(instr->operand.float_operand));
        
        case OP_PUSH_STRING:
            return vm_push_value(vm, vm_value_create_string(instr->operand.string_operand));
        
        case OP_PUSH_NULL:
            return vm_push_value(vm, vm_value_create_null());
        
        case OP_POP: {
            VMValue v = vm_pop_value(vm);
            vm_value_release(&v);
            return 0;
        }
        
        case OP_DUP: {
            VMValue v = vm_peek_value(vm);
            return vm_push_value(vm, v);
        }
        
        case OP_LOAD_LOCAL: {
            if (!vm->current_frame) {
                fprintf(stderr, "[VM] OP_LOAD_LOCAL ERROR: No current frame!\n");
                return -1;
            }
            
            fprintf(stderr, "[VM] OP_LOAD_LOCAL called in function: %s\n", 
                    vm->current_frame->function ? vm->current_frame->function->name : "(null)");
            
            int idx = instr->operand.int_operand;
            /* CRITICAL FIX: Bounds check must include BOTH parameters and locals
             * Parameters are at indices [0..param_count-1]
             * Locals are at indices [param_count..param_count+local_var_count-1]
             * Total valid indices: [0..param_count+local_var_count-1] */
            int total_vars = vm->current_frame->function->param_count + vm->current_frame->function->local_var_count;
            if (idx < 0 || idx >= total_vars) {
                fprintf(stderr, "[VM] OP_LOAD_LOCAL ERROR: idx=%d out of bounds (total_vars=%d, params=%d, locals=%d)\n",
                        idx, total_vars, vm->current_frame->function->param_count, vm->current_frame->function->local_var_count);
                return -1;
            }
            
            /* Validate frame state */
            if (!vm->current_frame->local_variables) {
                fprintf(stderr, "[VM] OP_LOAD_LOCAL ERROR: No local_variables array in frame!\n");
                return -1;
            }
            
            VMValue v = vm->current_frame->local_variables[idx];
            
            /* Debug ALL OP_LOAD_LOCAL in process_command */
            if (strcmp(vm->current_frame->function->name, "process_command") == 0) {
                fprintf(stderr, "[VM] OP_LOAD_LOCAL idx=%d in %s: type=%d", 
                        idx, vm->current_frame->function->name, v.type);
                if (idx < vm->current_frame->function->param_count) {
                    fprintf(stderr, " (PARAM %d/%d)", idx+1, vm->current_frame->function->param_count);
                } else {
                    fprintf(stderr, " (LOCAL %d)", idx - vm->current_frame->function->param_count);
                }
                if (v.type == VALUE_STRING) {
                    fprintf(stderr, " ptr=%p", (void*)v.data.string_value);
                    if (v.data.string_value) {
                        fprintf(stderr, " value='%s'", v.data.string_value);
                    } else {
                        fprintf(stderr, " (NULL STRING POINTER!)");
                    }
                }
                fprintf(stderr, " frame=%p local_vars=%p\n", 
                        (void*)vm->current_frame, (void*)vm->current_frame->local_variables);
                
                /* Warn if parameter is NULL */
                if (v.type == VALUE_NULL && idx < vm->current_frame->function->param_count) {
                    fprintf(stderr, "[VM] ERROR: Parameter %d is NULL! Frame corruption?\n", idx);
                }
            }
            
            return vm_push_value(vm, v);
        }
        
        case OP_STORE_LOCAL: {
            if (!vm->current_frame) return -1;
            int idx = instr->operand.int_operand;
            /* CRITICAL FIX: Bounds check must include BOTH parameters and locals
             * Parameters are at indices [0..param_count-1]
             * Locals are at indices [param_count..param_count+local_var_count-1]
             * Total valid indices: [0..param_count+local_var_count-1] */
            int total_vars = vm->current_frame->function->param_count + vm->current_frame->function->local_var_count;
            if (idx < 0 || idx >= total_vars) {
                fprintf(stderr, "[VM] OP_STORE_LOCAL ERROR: idx=%d out of bounds (total_vars=%d, params=%d, locals=%d)\n",
                        idx, total_vars, vm->current_frame->function->param_count, vm->current_frame->function->local_var_count);
                return -1;
            }
            VMValue v = vm_pop_value(vm);
            vm_value_release(&vm->current_frame->local_variables[idx]);
            vm->current_frame->local_variables[idx] = v;
            return 0;
        }
        
        case OP_LOAD_GLOBAL: {
            int idx = instr->operand.int_operand;
            if (idx < 0 || idx >= vm->global_count) return -1;
            VMValue v = vm->global_variables[idx];
            return vm_push_value(vm, v);
        }
        
        case OP_STORE_GLOBAL: {
            int idx = instr->operand.int_operand;
            if (idx < 0) {
                if (vm->global_count >= vm->global_capacity) {
                    vm->global_capacity *= 2;
                    vm->global_variables = (VMValue *)realloc(vm->global_variables,
                                                               sizeof(VMValue) * vm->global_capacity);
                }
                idx = vm->global_count++;
            }
            VMValue v = vm_pop_value(vm);
            if (idx >= 0 && idx < vm->global_count) {
                vm_value_release(&vm->global_variables[idx]);
            }
            vm->global_variables[idx] = v;
            return 0;
        }
        
        case OP_ADD: vm_arithmetic_op(vm, 0); return 0;
        case OP_SUB: vm_arithmetic_op(vm, 1); return 0;
        case OP_MUL: vm_arithmetic_op(vm, 2); return 0;
        case OP_DIV: vm_arithmetic_op(vm, 3); return 0;
        case OP_MOD: vm_arithmetic_op(vm, 4); return 0;
        case OP_NEG: vm_negate(vm); return 0;
        
        case OP_EQ: vm_comparison_op(vm, 0); return 0;
        case OP_NE: vm_comparison_op(vm, 1); return 0;
        case OP_LT: vm_comparison_op(vm, 2); return 0;
        case OP_LE: vm_comparison_op(vm, 3); return 0;
        case OP_GT: vm_comparison_op(vm, 4); return 0;
        case OP_GE: vm_comparison_op(vm, 5); return 0;
        
        case OP_AND: vm_logical_and(vm); return 0;
        case OP_OR: vm_logical_or(vm); return 0;
        case OP_NOT: vm_logical_not(vm); return 0;
        
        case OP_BIT_AND: vm_bitwise_op(vm, 0); return 0;
        case OP_BIT_OR: vm_bitwise_op(vm, 1); return 0;
        case OP_BIT_XOR: vm_bitwise_op(vm, 2); return 0;
        case OP_BIT_NOT: vm_bitwise_op(vm, 3); return 0;
        case OP_LSHIFT: vm_bitwise_op(vm, 4); return 0;
        case OP_RSHIFT: vm_bitwise_op(vm, 5); return 0;
        
        case OP_JUMP:
            vm->instruction_pointer = instr->operand.address_operand;
            return 0;
        
        case OP_JUMP_IF_FALSE: {
            VMValue cond = vm_pop_value(vm);
            if (!vm_value_is_truthy(cond)) {
                vm->instruction_pointer = instr->operand.address_operand;
            }
            vm_value_release(&cond);
            return 0;
        }
        
        case OP_JUMP_IF_TRUE: {
            VMValue cond = vm_pop_value(vm);
            if (vm_value_is_truthy(cond)) {
                vm->instruction_pointer = instr->operand.address_operand;
            }
            vm_value_release(&cond);
            return 0;
        }
        
        case OP_CALL: {
            int arg_count = instr->operand.call_operand.arg_count;
            char *func_name = instr->operand.call_operand.name;
            
            if (func_name) {
                /* Try to call as an efun first */
                if (vm->efun_registry) {
                    EfunEntry *efun_entry = efun_find(vm->efun_registry, func_name);
                    if (efun_entry) {
                        VMValue *args = NULL;
                        if (arg_count > 0) {
                            args = (VMValue *)malloc(sizeof(VMValue) * arg_count);
                            if (!args) return -1;
                            /* Pop arguments from stack in reverse order */
                            for (int i = arg_count - 1; i >= 0; i--) {
                                args[i] = vm_pop_value(vm);
                            }
                        }
                        VMValue result = efun_entry->callback(vm, args, arg_count);
                        if (args) {
                            for (int i = 0; i < arg_count; i++) {
                                vm_value_release(&args[i]);
                            }
                            free(args);
                        }
                        return vm_push_value(vm, result);
                    }
                }
                
                /* Otherwise, try to call as a user-defined function */
                for (int i = 0; i < vm->function_count; i++) {
                    if (vm->functions[i] && strcmp(vm->functions[i]->name, func_name) == 0) {
                        return vm_call_function(vm, i, arg_count);
                    }
                }
                
                fprintf(stderr, "[VM] OP_CALL: Unknown function: %s\n", func_name);
                return -1;
            } else {
                int func_idx = instr->operand.call_operand.target;
                return vm_call_function(vm, func_idx, arg_count);
            }
        }
        
        case OP_RETURN:
            vm->running = 0;
            return 0;
        
        case OP_MAKE_ARRAY: {
            int size = instr->operand.int_operand;
            array_t *arr = array_new(vm->gc, size);
            for (int i = 0; i < size; i++) {
                VMValue v = vm_pop_value(vm);
                array_push(arr, v);
            }
            VMValue arr_val;
            arr_val.type = VALUE_ARRAY;
            arr_val.data.array_value = arr;
            return vm_push_value(vm, arr_val);
        }
        
        case OP_INDEX_ARRAY: {
            VMValue idx_val = vm_pop_value(vm);
            VMValue arr_val = vm_pop_value(vm);
            if (arr_val.type != VALUE_ARRAY) return -1;
            
            int idx = (idx_val.type == VALUE_INT) ? idx_val.data.int_value : (int)idx_val.data.float_value;
            VMValue result = array_get((array_t *)arr_val.data.array_value, idx);
            return vm_push_value(vm, result);
        }
        
        case OP_STORE_ARRAY: {
            VMValue val = vm_pop_value(vm);
            VMValue idx_val = vm_pop_value(vm);
            VMValue arr_val = vm_pop_value(vm);
            if (arr_val.type != VALUE_ARRAY) return -1;
            
            int idx = (idx_val.type == VALUE_INT) ? idx_val.data.int_value : (int)idx_val.data.float_value;
            return array_set((array_t *)arr_val.data.array_value, idx, val);
        }
        
        case OP_MAKE_MAPPING: {
            int pair_count = instr->operand.int_operand;
            mapping_t *map = mapping_new(vm->gc, VM_MAPPING_BUCKETS);
            for (int i = 0; i < pair_count; i++) {
                VMValue val = vm_pop_value(vm);
                VMValue key_val = vm_pop_value(vm);
                if (key_val.type == VALUE_STRING) {
                    mapping_set(map, key_val.data.string_value, val);
                }
            }
            VMValue map_val;
            map_val.type = VALUE_MAPPING;
            map_val.data.mapping_value = map;
            return vm_push_value(vm, map_val);
        }
        
        case OP_INDEX_MAPPING: {
            VMValue key_val = vm_pop_value(vm);
            VMValue map_val = vm_pop_value(vm);
            if (map_val.type != VALUE_MAPPING || key_val.type != VALUE_STRING) return -1;
            
            VMValue result = mapping_get((mapping_t *)map_val.data.mapping_value, 
                                         key_val.data.string_value);
            return vm_push_value(vm, result);
        }
        
        case OP_STORE_MAPPING: {
            VMValue val = vm_pop_value(vm);
            VMValue key_val = vm_pop_value(vm);
            VMValue map_val = vm_pop_value(vm);
            if (map_val.type != VALUE_MAPPING || key_val.type != VALUE_STRING) return -1;

            mapping_entry_t *entry = mapping_set((mapping_t *)map_val.data.mapping_value,
                                                 key_val.data.string_value, val);
            return entry ? 0 : -1;
        }
        
        case OP_HALT:
            vm->running = 0;
            return 0;
        
        case OP_PRINT: {
            VMValue v = vm_pop_value(vm);
            char *str = vm_value_to_string(v);
            printf("%s\n", str);
            free(str);
            vm_value_release(&v);
            return 0;
        }
        
        case OP_CALL_METHOD: {
            int arg_count = instr->operand.int_operand;
            if (arg_count < 0 || arg_count > vm->stack->top - 2) {
                fprintf(stderr, "[VM] OP_CALL_METHOD: invalid arg_count=%d (stack=%d)\n",
                        arg_count, vm->stack ? vm->stack->top : -1);
                vm_push_value(vm, vm_value_create_null());
                return -1;
            }

            /* Collect arguments from stack (preserve call order) */
            VMValue args_buffer[32];
            VMValue *args = args_buffer;
            if (arg_count > (int)(sizeof(args_buffer) / sizeof(args_buffer[0]))) {
                args = (VMValue *)malloc(sizeof(VMValue) * arg_count);
                if (!args) {
                    fprintf(stderr, "[VM] OP_CALL_METHOD: out of memory for %d args\n", arg_count);
                    vm_push_value(vm, vm_value_create_null());
                    return -1;
                }
            }

            for (int i = arg_count - 1; i >= 0; i--) {
                args[i] = vm_pop_value(vm);
            }

            VMValue method_val = vm_pop_value(vm);
            VMValue obj_val = vm_pop_value(vm);

            if (method_val.type != VALUE_STRING || !method_val.data.string_value) {
                fprintf(stderr, "[VM] OP_CALL_METHOD: method name must be string\n");
                vm_value_free(&method_val);
                for (int i = 0; i < arg_count; i++) {
                    vm_value_free(&args[i]);
                }
                if (args != args_buffer) free(args);
                vm_push_value(vm, vm_value_create_null());
                return -1;
            }

            if (obj_val.type != VALUE_OBJECT || !obj_val.data.object_value) {
                fprintf(stderr, "[VM] OP_CALL_METHOD: invalid object reference\n");
                vm_value_free(&method_val);
                for (int i = 0; i < arg_count; i++) {
                    vm_value_free(&args[i]);
                }
                if (args != args_buffer) free(args);
                vm_push_value(vm, vm_value_create_null());
                return -1;
            }

            obj_t *target = (obj_t *)obj_val.data.object_value;
            const char *method_name = method_val.data.string_value;

            VMValue result = obj_call_method(vm, target, method_name, args, arg_count);

            /* method_name no longer needed */
            vm_value_free(&method_val);

            /* Clean up temporary args allocation if used */
            if (args != args_buffer) {
                free(args);
            }

            return vm_push_value(vm, result);
        }
        
        default:
            fprintf(stderr, "Unknown opcode: %d\n", instr->opcode);
            return -1;
    }
}

int vm_execute(VirtualMachine *vm) {
    if (!vm || !vm->instructions) return -1;
    
    vm->running = 1;
    vm->instruction_pointer = 0;
    
    while (vm->running && vm->instruction_pointer < vm->instruction_count) {
        VMInstruction *instr = &vm->instructions[vm->instruction_pointer++];
        if (vm->debug_flags & VM_DEBUG_TRACE) {
            vm_trace_instruction(vm, vm->current_frame, instr, vm->instruction_pointer - 1);
        }
        if (vm_execute_instruction(vm, instr) < 0) {
            vm->error_count++;
            if (vm->debug_flags & VM_DEBUG_CALLSTACK) {
                vm_trace_dump_call_stack(vm, "vm_execute error");
            }
            return -1;
        }
    }
    
    return 0;
}

/* Forward declaration */
static const char* opcode_name(OpCode opcode);

int vm_call_function(VirtualMachine *vm, int function_index, int arg_count) {
    if (!vm || function_index < 0 || function_index >= vm->function_count) return -1;
    
    VMFunction *func = vm->functions[function_index];
    if (!func || arg_count != func->param_count) return -1;
    
    /* CRITICAL: Allocate space for BOTH parameters and local variables
     * Parameters are stored in local_variables[0..param_count-1]
     * Local variables are stored in local_variables[param_count..param_count+local_var_count-1]
     * Previous code allocated only local_var_count space, causing buffer overflow when params > 0 */
    int total_vars = func->param_count + func->local_var_count;
    fprintf(stderr, "[VM] ALLOCATION DEBUG: func->param_count=%d, func->local_var_count=%d, total_vars=%d, size=%zu bytes\n",
            func->param_count, func->local_var_count, total_vars, sizeof(VMValue) * total_vars);
    
    CallFrame *frame = (CallFrame *)malloc(sizeof(CallFrame));
    frame->function = func;
    frame->local_variables = (VMValue *)malloc(sizeof(VMValue) * total_vars);
    fprintf(stderr, "[VM] ALLOCATION DEBUG: allocated frame->local_variables=%p\n", (void*)frame->local_variables);
    frame->instruction_pointer = 0;
    frame->stack_base = vm->stack->top - arg_count;
    frame->prev = vm->current_frame;
    
    /* Initialize all local variables (parameters + locals) to uninitialized
     * Parameters will be overwritten in the next loop, locals need initialization */
    for (int i = 0; i < total_vars; i++) {
        frame->local_variables[i].type = VALUE_UNINITIALIZED;
    }
    
    /* Copy parameters from stack to local variables array */
    for (int i = 0; i < arg_count; i++) {
        frame->local_variables[i] = vm->stack->values[frame->stack_base + i];
        vm_value_addref(&frame->local_variables[i]);
        fprintf(stderr, "[VM] DEBUG: Copied param %d from stack[%d] to local[%d]: type=%d", 
                i, frame->stack_base + i, i, frame->local_variables[i].type);
        if (frame->local_variables[i].type == VALUE_STRING) {
            fprintf(stderr, " str_ptr=%p value='%s'", 
                    (void*)frame->local_variables[i].data.string_value,
                    frame->local_variables[i].data.string_value ? frame->local_variables[i].data.string_value : "(null)");
        }
        fprintf(stderr, "\n");
    }
    
    vm->current_frame = frame;
    
    /* Debug: Print frame state before execution */
    if (strcmp(func->name, "process_command") == 0) {
        fprintf(stderr, "[VM] FRAME SETUP for %s: frame=%p local_vars=%p param_count=%d local_count=%d\n",
                func->name, (void*)frame, (void*)frame->local_variables, 
                func->param_count, func->local_var_count);
        /* Enable instruction tracing for this function */
        vm->debug_flags |= VM_DEBUG_TRACE;
    }
    
    int saved_running = vm->running;
    vm->running = 1;

    while (frame->instruction_pointer < func->instruction_count && vm->running) {
        VMInstruction *instr = &func->instructions[frame->instruction_pointer++];
        
        /* Debug EVERY instruction in process_command */
        if (strcmp(func->name, "process_command") == 0) {
            fprintf(stderr, "[VM] [%s] IP=%d opcode=%d\n", 
                    func->name, frame->instruction_pointer - 1, instr->opcode);
        }
        
        if (vm->debug_flags & VM_DEBUG_TRACE) {
            vm_trace_instruction(vm, frame, instr, frame->instruction_pointer - 1);
        }
        if (vm_execute_instruction(vm, instr) < 0) {
            vm->error_count++;
            if (vm->debug_flags & VM_DEBUG_CALLSTACK) {
                vm_trace_dump_call_stack(vm, "vm_call_function error");
            }
            vm->running = saved_running;
            vm->current_frame = frame->prev;
            /* Release all variables (parameters and locals) */
            for (int i = 0; i < total_vars; i++) {
                vm_value_release(&frame->local_variables[i]);
            }
            if (frame->local_variables) free(frame->local_variables);
            free(frame);
            return -1;
        }
    }

    vm->running = saved_running;
    vm->current_frame = frame->prev;
    /* Release all variables (parameters and locals) */
    for (int i = 0; i < total_vars; i++) {
        vm_value_release(&frame->local_variables[i]);
    }
    if (frame->local_variables) free(frame->local_variables);
    free(frame);
    
    return 0;
}

/* ========== Debugging Functions ========== */

void vm_print_stack(VirtualMachine *vm) {
    if (!vm || !vm->stack) return;
    
    printf("Stack (%d items):\n", vm->stack->top);
    for (int i = 0; i < vm->stack->top; i++) {
        printf("  [%d] ", i);
        char *str = vm_value_to_string(vm->stack->values[i]);
        printf("%s\n", str);
        free(str);
    }
}

static const char* opcode_name(OpCode opcode) {
    switch (opcode) {
        case OP_PUSH_INT: return "PUSH_INT";
        case OP_PUSH_FLOAT: return "PUSH_FLOAT";
        case OP_PUSH_STRING: return "PUSH_STRING";
        case OP_PUSH_NULL: return "PUSH_NULL";
        case OP_POP: return "POP";
        case OP_DUP: return "DUP";
        case OP_LOAD_LOCAL: return "LOAD_LOCAL";
        case OP_STORE_LOCAL: return "STORE_LOCAL";
        case OP_LOAD_GLOBAL: return "LOAD_GLOBAL";
        case OP_STORE_GLOBAL: return "STORE_GLOBAL";
        case OP_ADD: return "ADD";
        case OP_SUB: return "SUB";
        case OP_MUL: return "MUL";
        case OP_DIV: return "DIV";
        case OP_MOD: return "MOD";
        case OP_NEG: return "NEG";
        case OP_EQ: return "EQ";
        case OP_NE: return "NE";
        case OP_LT: return "LT";
        case OP_LE: return "LE";
        case OP_GT: return "GT";
        case OP_GE: return "GE";
        case OP_AND: return "AND";
        case OP_OR: return "OR";
        case OP_NOT: return "NOT";
        case OP_BIT_AND: return "BIT_AND";
        case OP_BIT_OR: return "BIT_OR";
        case OP_BIT_XOR: return "BIT_XOR";
        case OP_BIT_NOT: return "BIT_NOT";
        case OP_LSHIFT: return "LSHIFT";
        case OP_RSHIFT: return "RSHIFT";
        case OP_JUMP: return "JUMP";
        case OP_JUMP_IF_FALSE: return "JUMP_IF_FALSE";
        case OP_JUMP_IF_TRUE: return "JUMP_IF_TRUE";
        case OP_CALL: return "CALL";
        case OP_RETURN: return "RETURN";
        case OP_MAKE_ARRAY: return "MAKE_ARRAY";
        case OP_INDEX_ARRAY: return "INDEX_ARRAY";
        case OP_STORE_ARRAY: return "STORE_ARRAY";
        case OP_MAKE_MAPPING: return "MAKE_MAPPING";
        case OP_INDEX_MAPPING: return "INDEX_MAPPING";
        case OP_STORE_MAPPING: return "STORE_MAPPING";
        case OP_CALL_METHOD: return "CALL_METHOD";
        case OP_HALT: return "HALT";
        case OP_PRINT: return "PRINT";
        default: return "UNKNOWN";
    }
}

void vm_disassemble_instruction(VMInstruction instruction, int index) {
    printf("[%3d] %-20s", index, opcode_name(instruction.opcode));
    
    switch (instruction.opcode) {
        case OP_PUSH_INT:
            printf(" %ld\n", instruction.operand.int_operand);
            break;
        case OP_PUSH_FLOAT:
            printf(" %g\n", instruction.operand.float_operand);
            break;
        case OP_PUSH_STRING:
            printf(" \"%s\"\n", instruction.operand.string_operand);
            break;
        case OP_JUMP:
        case OP_JUMP_IF_FALSE:
        case OP_JUMP_IF_TRUE:
            printf(" -> %d\n", instruction.operand.address_operand);
            break;
        case OP_CALL:
            printf(" func=%d, args=%d\n", instruction.operand.call_operand.target,
                   instruction.operand.call_operand.arg_count);
            break;
        case OP_LOAD_LOCAL:
        case OP_STORE_LOCAL:
        case OP_LOAD_GLOBAL:
        case OP_STORE_GLOBAL:
        case OP_MAKE_ARRAY:
        case OP_MAKE_MAPPING:
            printf(" %ld\n", instruction.operand.int_operand);
            break;
        default:
            printf("\n");
            break;
    }
}

void vm_disassemble_function(VMFunction *function) {
    if (!function) return;
    
    printf("Function: %s (params=%d, locals=%d)\n",
           function->name, function->param_count, function->local_var_count);
    
    for (int i = 0; i < function->instruction_count; i++) {
        vm_disassemble_instruction(function->instructions[i], i);
    }
    printf("\n");
}
