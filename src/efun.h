/**
 * efun.h - External Functions (Efuns) Header
 * 
 * Built-in functions available to LPC code.
 * Efuns provide core functionality for string manipulation, array operations,
 * mathematical functions, and I/O.
 * 
 * Phase 5 Implementation - January 22, 2026
 */

#ifndef EFUN_H
#define EFUN_H

#include "vm.h"
#include "array.h"
#include "mapping.h"
#include <stddef.h>

/* ========== Forward Declarations ========== */

typedef struct EfunRegistry EfunRegistry;
typedef VMValue (*EfunCallback)(VirtualMachine *vm, VMValue *args, int arg_count);

/* ========== Efun Function Entry ========== */

typedef struct {
    char *name;              /* Efun name */
    EfunCallback callback;    /* C function pointer */
    int min_args;            /* Minimum number of arguments */
    int max_args;            /* Maximum number of arguments (-1 = unlimited) */
    char *signature;         /* Function signature (for documentation) */
} EfunEntry;

/* ========== Efun Registry ========== */

struct EfunRegistry {
    EfunEntry *efuns;        /* Array of efuns */
    int efun_count;          /* Number of registered efuns */
    int efun_capacity;       /* Capacity of efuns array */
};

/* ========== Registry Management ========== */

/**
 * Initialize efun registry
 * 
 * @return Pointer to efun registry, or NULL on failure
 */
EfunRegistry* efun_init(void);

/**
 * Register an efun
 * 
 * @param registry Efun registry
 * @param name Efun name
 * @param callback Function pointer
 * @param min_args Minimum arguments
 * @param max_args Maximum arguments (-1 = unlimited)
 * @param signature Function signature string
 * @return 0 on success, -1 on failure
 */
int efun_register(EfunRegistry *registry, const char *name, EfunCallback callback,
                  int min_args, int max_args, const char *signature);

/**
 * Find efun by name
 * 
 * @param registry Efun registry
 * @param name Efun name
 * @return EfunEntry pointer, or NULL if not found
 */
EfunEntry* efun_find(EfunRegistry *registry, const char *name);

/**
 * Call an efun
 * 
 * @param registry Efun registry
 * @param vm Virtual machine
 * @param name Efun name
 * @param args Array of arguments
 * @param arg_count Number of arguments
 * @return Result value
 */
VMValue efun_call(EfunRegistry *registry, VirtualMachine *vm, 
                  const char *name, VMValue *args, int arg_count);

/**
 * Free efun registry
 * 
 * @param registry Efun registry to free
 */
void efun_free(EfunRegistry *registry);

/* ========== Standard Efuns: String Functions ========== */

VMValue efun_strlen(VirtualMachine *vm, VMValue *args, int arg_count);
VMValue efun_substring(VirtualMachine *vm, VMValue *args, int arg_count);
VMValue efun_explode(VirtualMachine *vm, VMValue *args, int arg_count);
VMValue efun_implode(VirtualMachine *vm, VMValue *args, int arg_count);
VMValue efun_upper_case(VirtualMachine *vm, VMValue *args, int arg_count);
VMValue efun_lower_case(VirtualMachine *vm, VMValue *args, int arg_count);
VMValue efun_trim(VirtualMachine *vm, VMValue *args, int arg_count);

/* ========== Standard Efuns: Array Functions ========== */

VMValue efun_sizeof(VirtualMachine *vm, VMValue *args, int arg_count);
VMValue efun_keys(VirtualMachine *vm, VMValue *args, int arg_count);
VMValue efun_arrayp(VirtualMachine *vm, VMValue *args, int arg_count);
VMValue efun_sort_array(VirtualMachine *vm, VMValue *args, int arg_count);
VMValue efun_reverse_array(VirtualMachine *vm, VMValue *args, int arg_count);

/* ========== Standard Efuns: Math Functions ========== */

VMValue efun_abs(VirtualMachine *vm, VMValue *args, int arg_count);
VMValue efun_sqrt(VirtualMachine *vm, VMValue *args, int arg_count);
VMValue efun_pow(VirtualMachine *vm, VMValue *args, int arg_count);
VMValue efun_random(VirtualMachine *vm, VMValue *args, int arg_count);
VMValue efun_min(VirtualMachine *vm, VMValue *args, int arg_count);
VMValue efun_max(VirtualMachine *vm, VMValue *args, int arg_count);

/* ========== Standard Efuns: Type Checking ========== */

VMValue efun_intp(VirtualMachine *vm, VMValue *args, int arg_count);
VMValue efun_floatp(VirtualMachine *vm, VMValue *args, int arg_count);
VMValue efun_stringp(VirtualMachine *vm, VMValue *args, int arg_count);
VMValue efun_objectp(VirtualMachine *vm, VMValue *args, int arg_count);
VMValue efun_mappingp(VirtualMachine *vm, VMValue *args, int arg_count);

/* ========== Standard Efuns: I/O Functions ========== */

VMValue efun_write(VirtualMachine *vm, VMValue *args, int arg_count);
VMValue efun_printf(VirtualMachine *vm, VMValue *args, int arg_count);
VMValue efun_tell_object(VirtualMachine *vm, VMValue *args, int arg_count);
VMValue efun_read_file(VirtualMachine *vm, VMValue *args, int arg_count);
VMValue efun_write_file(VirtualMachine *vm, VMValue *args, int arg_count);
VMValue efun_file_size(VirtualMachine *vm, VMValue *args, int arg_count);
VMValue efun_get_dir(VirtualMachine *vm, VMValue *args, int arg_count);
VMValue efun_mkdir(VirtualMachine *vm, VMValue *args, int arg_count);
VMValue efun_rm(VirtualMachine *vm, VMValue *args, int arg_count);

/* ========== Object/Player Efuns (core) ========== */

VMValue efun_call_other(VirtualMachine *vm, VMValue *args, int arg_count);
VMValue efun_clone_object(VirtualMachine *vm, VMValue *args, int arg_count);
VMValue efun_find_object(VirtualMachine *vm, VMValue *args, int arg_count);
VMValue efun_present(VirtualMachine *vm, VMValue *args, int arg_count);
VMValue efun_environment(VirtualMachine *vm, VMValue *args, int arg_count);
VMValue efun_move_object(VirtualMachine *vm, VMValue *args, int arg_count);
VMValue efun_this_player(VirtualMachine *vm, VMValue *args, int arg_count);
VMValue efun_file_name(VirtualMachine *vm, VMValue *args, int arg_count);

/* ========== Object/Player Efuns (additional) ========== */

VMValue efun_load_object(VirtualMachine *vm, VMValue *args, int arg_count);
VMValue efun_all_inventory(VirtualMachine *vm, VMValue *args, int arg_count);
VMValue efun_users(VirtualMachine *vm, VMValue *args, int arg_count);
VMValue efun_function_exists(VirtualMachine *vm, VMValue *args, int arg_count);
VMValue efun_enable_commands(VirtualMachine *vm, VMValue *args, int arg_count);
VMValue efun_add_action(VirtualMachine *vm, VMValue *args, int arg_count);
VMValue efun_query_verb(VirtualMachine *vm, VMValue *args, int arg_count);

/* ========== Utility Functions ========== */

/* Debugging efuns */
VMValue efun_debug_set_flags(VirtualMachine *vm, VMValue *args, int arg_count);
VMValue efun_debug_get_flags(VirtualMachine *vm, VMValue *args, int arg_count);
VMValue efun_debug_dump_call_stack(VirtualMachine *vm, VMValue *args, int arg_count);
VMValue efun_debug_dump_bytecode(VirtualMachine *vm, VMValue *args, int arg_count);
VMValue efun_debug_mem_stats(VirtualMachine *vm, VMValue *args, int arg_count);

/**
 * Register all standard efuns
 * 
 * @param registry Efun registry
 * @return Number of efuns registered
 */
int efun_register_all(EfunRegistry *registry);

/**
 * Print all registered efuns
 * 
 * @param registry Efun registry
 */
void efun_print_all(EfunRegistry *registry);

/**
 * Get efun count
 * 
 * @param registry Efun registry
 * @return Number of registered efuns
 */
int efun_get_count(EfunRegistry *registry);

#endif /* EFUN_H */
