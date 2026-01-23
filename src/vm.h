/*
 * vm.h - Virtual Machine Header
 * 
 * This header defines the Virtual Machine structure and API for executing
 * bytecode compiled from the Abstract Syntax Tree. The VM is a stack-based
 * architecture with support for:
 * - Local variables and function calls
 * - Arithmetic, logical, and comparison operations
 * - Control flow (jumps, conditionals, loops)
 * - LPC data types (int, float, string, object, array, mapping)
 */

#ifndef VM_H
#define VM_H

#include "parser.h"
#include "gc.h"
#include <stdint.h>

/* ========== Bytecode Opcodes ========== */

typedef enum {
    /* Stack Operations */
    OP_PUSH_INT,        /* Push integer constant onto stack */
    OP_PUSH_FLOAT,      /* Push float constant onto stack */
    OP_PUSH_STRING,     /* Push string constant onto stack */
    OP_PUSH_NULL,       /* Push null value onto stack */
    OP_POP,             /* Pop value from stack */
    OP_DUP,             /* Duplicate top stack value */
    
    /* Variable Operations */
    OP_LOAD_LOCAL,      /* Load local variable onto stack */
    OP_STORE_LOCAL,     /* Store top stack value to local variable */
    OP_LOAD_GLOBAL,     /* Load global variable onto stack */
    OP_STORE_GLOBAL,    /* Store top stack value to global variable */
    
    /* Arithmetic Operations */
    OP_ADD,             /* Addition: pop b, pop a, push a+b */
    OP_SUB,             /* Subtraction: pop b, pop a, push a-b */
    OP_MUL,             /* Multiplication: pop b, pop a, push a*b */
    OP_DIV,             /* Division: pop b, pop a, push a/b */
    OP_MOD,             /* Modulo: pop b, pop a, push a%b */
    OP_NEG,             /* Negation: pop a, push -a */
    
    /* Comparison Operations */
    OP_EQ,              /* Equal: pop b, pop a, push a==b */
    OP_NE,              /* Not equal: pop b, pop a, push a!=b */
    OP_LT,              /* Less than: pop b, pop a, push a<b */
    OP_LE,              /* Less or equal: pop b, pop a, push a<=b */
    OP_GT,              /* Greater than: pop b, pop a, push a>b */
    OP_GE,              /* Greater or equal: pop b, pop a, push a>=b */
    
    /* Logical Operations */
    OP_AND,             /* Logical AND: pop b, pop a, push a&&b */
    OP_OR,              /* Logical OR: pop b, pop a, push a||b */
    OP_NOT,             /* Logical NOT: pop a, push !a */
    
    /* Bitwise Operations */
    OP_BIT_AND,         /* Bitwise AND: pop b, pop a, push a&b */
    OP_BIT_OR,          /* Bitwise OR: pop b, pop a, push a|b */
    OP_BIT_XOR,         /* Bitwise XOR: pop b, pop a, push a^b */
    OP_BIT_NOT,         /* Bitwise NOT: pop a, push ~a */
    OP_LSHIFT,          /* Left shift: pop b, pop a, push a<<b */
    OP_RSHIFT,          /* Right shift: pop b, pop a, push a>>b */
    
    /* Control Flow */
    OP_JUMP,            /* Unconditional jump to address */
    OP_JUMP_IF_FALSE,   /* Jump if top of stack is false */
    OP_JUMP_IF_TRUE,    /* Jump if top of stack is true */
    
    /* Function Operations */
    OP_CALL,            /* Call function with n arguments */
    OP_RETURN,          /* Return from function */
    
    /* Array Operations */
    OP_MAKE_ARRAY,      /* Create array with n elements from stack */
    OP_INDEX_ARRAY,     /* Array indexing: pop index, pop array, push element */
    OP_STORE_ARRAY,     /* Array store: pop value, pop index, pop array */
    
    /* Mapping Operations */
    OP_MAKE_MAPPING,    /* Create mapping with n key-value pairs from stack */
    OP_INDEX_MAPPING,   /* Mapping indexing: pop key, pop mapping, push value */
    OP_STORE_MAPPING,   /* Mapping store: pop value, pop key, pop mapping */
    
    /* Object Operations */
    OP_CALL_METHOD,     /* Call object method with n arguments */
    
    /* Special */
    OP_HALT,            /* Stop execution */
    OP_PRINT,           /* Print top of stack (debugging) */
} OpCode;

/* ========== Value Types ========== */

struct array_t;
struct mapping_t;
typedef struct array_t array_t;
typedef struct mapping_t mapping_t;

typedef enum {
    VALUE_UNINITIALIZED,
    VALUE_INT,
    VALUE_FLOAT,
    VALUE_STRING,
    VALUE_OBJECT,
    VALUE_ARRAY,
    VALUE_MAPPING,
    VALUE_NULL,
    VALUE_FUNCTION,     /* Function reference */
} ValueType;

/* ========== Runtime Value Structure (forward declaration & definition) ========== */

typedef struct VMArray VMArray;
typedef struct VMMapping VMMapping;
typedef struct VMFunction VMFunction;

typedef struct {
    ValueType type;
    union {
        long int_value;
        double float_value;
        char *string_value;
        void *object_value;
        array_t *array_value;
        mapping_t *mapping_value;
        VMFunction *function_value;
    } data;
} VMValue;

/* ========== Bytecode Instruction ========== */

typedef struct {
    OpCode opcode;
    union {
        long int_operand;
        double float_operand;
        char *string_operand;
        int address_operand;    /* For jumps and calls */
        struct {
            int arg_count;      /* Number of arguments */
            int target;         /* Function or method index */
            char *name;         /* Function name for efun lookup */
        } call_operand;
    } operand;
} VMInstruction;

/* ========== Function Structure ========== */

typedef struct VMFunction {
    char *name;
    int param_count;
    int local_var_count;
    VMInstruction *instructions;
    int instruction_count;
    int instruction_capacity;
} VMFunction;

/* ========== Execution Stack ========== */

typedef struct {
    VMValue *values;
    int top;
    int capacity;
} VMStack;

/* ========== Call Frame ========== */

typedef struct CallFrame {
    VMFunction *function;       /* Function being executed */
    VMValue *local_variables;   /* Local variable storage */
    int instruction_pointer;    /* Current instruction in function */
    int stack_base;             /* Base of stack frame */
    struct CallFrame *prev;     /* Previous call frame */
} CallFrame;

/* ========== Virtual Machine Structure ========== */

typedef struct {
    /* Execution state */
    VMStack *stack;
    CallFrame *current_frame;
    int running;
    int error_count;

    /* Memory management */
    GC *gc;
    
    /* Efun registry */
    struct EfunRegistry *efun_registry;
    
    /* Program data */
    VMFunction **functions;     /* Array of all functions */
    int function_count;
    int function_capacity;
    
    VMValue *global_variables;  /* Global variable storage */
    int global_count;
    int global_capacity;
    
    /* String pool for constant strings */
    char **string_pool;
    int string_pool_count;
    int string_pool_capacity;
    
    /* Instruction pointer for top-level code */
    VMInstruction *instructions;
    int instruction_pointer;
    int instruction_count;
    int instruction_capacity;
} VirtualMachine;

/* ========== VM API Functions ========== */

/**
 * vm_init - Initialize the virtual machine
 * 
 * Creates and initializes a VirtualMachine structure with default
 * stack size, empty function table, and global variable storage.
 * 
 * Returns: Pointer to initialized VM, or NULL on error
 */
VirtualMachine* vm_init(void);

/**
 * vm_load_bytecode - Load bytecode instructions into the VM
 * @vm: Pointer to the VirtualMachine
 * @instructions: Array of bytecode instructions
 * @count: Number of instructions
 * 
 * Loads compiled bytecode into the VM for execution.
 * 
 * Returns: 0 on success, -1 on error
 */
int vm_load_bytecode(VirtualMachine *vm, VMInstruction *instructions, int count);

/**
 * vm_add_function - Add a function to the VM
 * @vm: Pointer to the VirtualMachine
 * @function: The function to add
 * 
 * Registers a function in the VM's function table.
 * 
 * Returns: Function index on success, -1 on error
 */
int vm_add_function(VirtualMachine *vm, VMFunction *function);

/**
 * vm_execute - Execute the loaded bytecode
 * @vm: Pointer to the VirtualMachine
 * 
 * Executes the loaded bytecode on the VM.
 * 
 * Returns: 0 on success, non-zero on error
 */
int vm_execute(VirtualMachine *vm);

/**
 * vm_call_function - Call a function by index
 * @vm: Pointer to the VirtualMachine
 * @function_index: Index of function to call
 * @arg_count: Number of arguments on stack
 * 
 * Calls a function with the specified number of arguments
 * already pushed onto the stack.
 * 
 * Returns: 0 on success, -1 on error
 */
int vm_call_function(VirtualMachine *vm, int function_index, int arg_count);

/**
 * vm_free - Free all VM resources
 * @vm: Pointer to the VirtualMachine
 * 
 * Deallocates the VM structure and all associated memory.
 */
void vm_free(VirtualMachine *vm);

/* ========== Stack Operations ========== */

/**
 * vm_push_value - Push a value onto the execution stack
 * @vm: Pointer to the VirtualMachine
 * @value: The value to push
 * 
 * Returns: 0 on success, -1 on stack overflow
 */
int vm_push_value(VirtualMachine *vm, VMValue value);

/**
 * vm_pop_value - Pop a value from the execution stack
 * @vm: Pointer to the VirtualMachine
 * 
 * Returns: The popped value, or VALUE_UNINITIALIZED if stack is empty
 */
VMValue vm_pop_value(VirtualMachine *vm);

/**
 * vm_peek_value - Peek at the top value without removing it
 * @vm: Pointer to the VirtualMachine
 * 
 * Returns: The top value, or VALUE_UNINITIALIZED if stack is empty
 */
VMValue vm_peek_value(VirtualMachine *vm);

/* ========== Value Operations ========== */

/**
 * vm_value_create_int - Create an integer value
 * @value: Integer value
 * 
 * Returns: VMValue containing the integer
 */
VMValue vm_value_create_int(long value);

/**
 * vm_value_create_float - Create a float value
 * @value: Float value
 * 
 * Returns: VMValue containing the float
 */
VMValue vm_value_create_float(double value);

/**
 * vm_value_create_string - Create a string value
 * @value: String value (will be copied)
 * 
 * Returns: VMValue containing the string
 */
VMValue vm_value_create_string(const char *value);

/**
 * vm_value_create_null - Create a null value
 * 
 * Returns: VMValue representing null
 */
VMValue vm_value_create_null(void);

/**
 * vm_value_free - Free a value's resources
 * @value: The value to free
 * 
 * Frees any dynamically allocated memory associated with the value.
 */
void vm_value_free(VMValue *value);

/**
 * vm_value_to_string - Convert a value to string representation
 * @value: The value to convert
 * 
 * Returns: Newly allocated string, caller must free
 */
char* vm_value_to_string(VMValue value);

/**
 * vm_value_is_truthy - Check if a value is truthy
 * @value: The value to check
 * 
 * Returns: 1 if truthy, 0 if falsy
 */
int vm_value_is_truthy(VMValue value);

/* ========== Value Clone ========== */

/**
 * vm_value_clone - Deep clone a VMValue
 * @value: Value to clone
 *
 * Returns: Deep copy of the value
 */
VMValue vm_value_clone(VMValue value);

/* ========== Function Operations ========== */

/**
 * vm_function_create - Create a new function
 * @name: Function name
 * @param_count: Number of parameters
 * @local_var_count: Number of local variables
 * 
 * Returns: Pointer to new function, or NULL on error
 */
VMFunction* vm_function_create(const char *name, int param_count, int local_var_count);

/**
 * vm_function_add_instruction - Add an instruction to a function
 * @function: The function
 * @instruction: Instruction to add
 * 
 * Returns: 0 on success, -1 on error
 */
int vm_function_add_instruction(VMFunction *function, VMInstruction instruction);

/**
 * vm_function_free - Free a function and its instructions
 * @function: The function to free
 */
void vm_function_free(VMFunction *function);

/* ========== Debugging ========== */

/**
 * vm_print_stack - Print the current stack contents
 * @vm: Pointer to the VirtualMachine
 */
void vm_print_stack(VirtualMachine *vm);

/**
 * vm_disassemble_instruction - Print a human-readable instruction
 * @instruction: The instruction to disassemble
 * @index: Instruction index (for display)
 */
void vm_disassemble_instruction(VMInstruction instruction, int index);

/**
 * vm_disassemble_function - Print all instructions in a function
 * @function: The function to disassemble
 */
void vm_disassemble_function(VMFunction *function);

#endif /* VM_H */
