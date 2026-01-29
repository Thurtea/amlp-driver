/**
 * test_efun.c - Efun (External Functions) Test Suite
 * 
 * Comprehensive tests for built-in functions.
 * 
 * Phase 5 - January 22, 2026
 */

#include "efun.h"
#include "vm.h"
#include "object.h"
#include "array.h"
#include "mapping.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <unistd.h>
#include <sys/stat.h>

/* ========== Test Framework ========== */

static int test_count = 0;
static int test_passed = 0;
static int test_failed = 0;

void test_setup(const char *test_name) {
    test_count++;
    printf("\n[TEST %d] %s\n", test_count, test_name);
}

void test_assert(int condition, const char *message) {
    if (condition) {
        printf("  ✓ PASS\n");
        test_passed++;
    } else {
        printf("  ✗ FAIL: %s\n", message);
        test_failed++;
    }
}

/* ========== TESTS: Registry ========== */

void test_efun_init(void) {
    test_setup("Initialize efun registry");
    
    EfunRegistry *registry = efun_init();
    
    test_assert(registry != NULL, "Registry should be initialized");
    test_assert(efun_get_count(registry) == 0, "Initial count should be 0");
    
    efun_free(registry);
}

void test_efun_register(void) {
    test_setup("Register a custom efun");
    
    EfunRegistry *registry = efun_init();
    
    int result = efun_register(registry, "test_func", efun_strlen, 1, 1, "int test_func(string)");
    
    test_assert(result == 0, "Registration should succeed");
    test_assert(efun_get_count(registry) == 1, "Count should be 1");
    
    efun_free(registry);
}

void test_efun_find(void) {
    test_setup("Find registered efun");
    
    EfunRegistry *registry = efun_init();
    efun_register(registry, "strlen", efun_strlen, 1, 1, "int strlen(string)");
    
    EfunEntry *found = efun_find(registry, "strlen");
    EfunEntry *not_found = efun_find(registry, "nonexistent");
    
    test_assert(found != NULL, "Should find strlen");
    test_assert(found->callback == efun_strlen, "Should have correct callback");
    test_assert(not_found == NULL, "Should not find nonexistent");
    
    efun_free(registry);
}

void test_efun_register_all(void) {
    test_setup("Register all standard efuns");
    
    EfunRegistry *registry = efun_init();
    int count = efun_register_all(registry);
    
    test_assert(count > 0, "Should register efuns");
    test_assert(efun_get_count(registry) == count, "Count should match");
    test_assert(efun_find(registry, "strlen") != NULL, "Should have strlen");
    test_assert(efun_find(registry, "abs") != NULL, "Should have abs");
    
    efun_free(registry);
}

/* ========== TESTS: String Functions ========== */

void test_efun_strlen(void) {
    test_setup("strlen() function");
    
    VirtualMachine *vm = vm_init();
    VMValue args[1];
    
    args[0] = vm_value_create_string("Hello");
    VMValue result = efun_strlen(vm, args, 1);
    
    test_assert(result.type == VALUE_INT, "Should return int");
    test_assert(result.data.int_value == 5, "Should return 5");
    
    vm_free(vm);
}

void test_efun_substring(void) {
    test_setup("substring() function");
    
    VirtualMachine *vm = vm_init();
    VMValue args[3];
    
    args[0] = vm_value_create_string("Hello World");
    args[1] = vm_value_create_int(0);
    args[2] = vm_value_create_int(5);
    
    VMValue result = efun_substring(vm, args, 3);
    
    test_assert(result.type == VALUE_STRING, "Should return string");
    test_assert(strcmp(result.data.string_value, "Hello") == 0, "Should return 'Hello'");
    
    vm_free(vm);
}

void test_efun_upper_case(void) {
    test_setup("upper_case() function");
    
    VirtualMachine *vm = vm_init();
    VMValue args[1];
    
    args[0] = vm_value_create_string("hello");
    VMValue result = efun_upper_case(vm, args, 1);
    
    test_assert(result.type == VALUE_STRING, "Should return string");
    test_assert(strcmp(result.data.string_value, "HELLO") == 0, "Should return 'HELLO'");
    
    vm_free(vm);
}

void test_efun_lower_case(void) {
    test_setup("lower_case() function");
    
    VirtualMachine *vm = vm_init();
    VMValue args[1];
    
    args[0] = vm_value_create_string("WORLD");
    VMValue result = efun_lower_case(vm, args, 1);
    
    test_assert(result.type == VALUE_STRING, "Should return string");
    test_assert(strcmp(result.data.string_value, "world") == 0, "Should return 'world'");
    
    vm_free(vm);
}

void test_efun_trim(void) {
    test_setup("trim() function");
    
    VirtualMachine *vm = vm_init();
    VMValue args[1];
    
    args[0] = vm_value_create_string("  hello  ");
    VMValue result = efun_trim(vm, args, 1);
    
    test_assert(result.type == VALUE_STRING, "Should return string");
    test_assert(strcmp(result.data.string_value, "hello") == 0, "Should trim whitespace");
    
    vm_free(vm);
}

/* ========== TESTS: Array Functions ========== */

void test_efun_sizeof_array(void) {
    test_setup("sizeof() for array");
    
    VirtualMachine *vm = vm_init();
    VMValue args[1];
    
    array_t *arr = array_new(vm->gc, 5);
    array_push(arr, vm_value_create_int(1));
    array_push(arr, vm_value_create_int(2));
    array_push(arr, vm_value_create_int(3));
    
    args[0] = vm_value_create_null();
    args[0].type = VALUE_ARRAY;
    args[0].data.array_value = arr;
    
    VMValue result = efun_sizeof(vm, args, 1);
    
    test_assert(result.type == VALUE_INT, "Should return int");
    test_assert(result.data.int_value == 3, "Should return 3");
    
    array_free(arr);
    vm_free(vm);
}

void test_efun_sizeof_string(void) {
    test_setup("sizeof() for string");
    
    VirtualMachine *vm = vm_init();
    VMValue args[1];
    
    args[0] = vm_value_create_string("test");
    VMValue result = efun_sizeof(vm, args, 1);
    
    test_assert(result.type == VALUE_INT, "Should return int");
    test_assert(result.data.int_value == 4, "Should return 4");
    
    vm_free(vm);
}

void test_efun_arrayp(void) {
    test_setup("arrayp() type check");
    
    VirtualMachine *vm = vm_init();
    VMValue args[1];
    
    array_t *arr = array_new(vm->gc, 1);
    args[0] = vm_value_create_null();
    args[0].type = VALUE_ARRAY;
    args[0].data.array_value = arr;
    
    VMValue result1 = efun_arrayp(vm, args, 1);
    test_assert(result1.data.int_value == 1, "Array should return 1");
    
    args[0] = vm_value_create_int(42);
    VMValue result2 = efun_arrayp(vm, args, 1);
    test_assert(result2.data.int_value == 0, "Int should return 0");
    
    array_free(arr);
    vm_free(vm);
}

/* ========== TESTS: Math Functions ========== */

void test_efun_abs_int(void) {
    test_setup("abs() for integer");
    
    VirtualMachine *vm = vm_init();
    VMValue args[1];
    
    args[0] = vm_value_create_int(-42);
    VMValue result = efun_abs(vm, args, 1);
    
    test_assert(result.type == VALUE_INT, "Should return int");
    test_assert(result.data.int_value == 42, "Should return 42");
    
    vm_free(vm);
}

void test_efun_abs_float(void) {
    test_setup("abs() for float");
    
    VirtualMachine *vm = vm_init();
    VMValue args[1];
    
    args[0] = vm_value_create_float(-3.14);
    VMValue result = efun_abs(vm, args, 1);
    
    test_assert(result.type == VALUE_FLOAT, "Should return float");
    test_assert(result.data.float_value > 3.13 && result.data.float_value < 3.15, 
                "Should return ~3.14");
    
    vm_free(vm);
}

void test_efun_sqrt(void) {
    test_setup("sqrt() function");
    
    VirtualMachine *vm = vm_init();
    VMValue args[1];
    
    args[0] = vm_value_create_int(16);
    VMValue result = efun_sqrt(vm, args, 1);
    
    test_assert(result.type == VALUE_FLOAT, "Should return float");
    test_assert(result.data.float_value > 3.99 && result.data.float_value < 4.01,
                "Should return ~4.0");
    
    vm_free(vm);
}

void test_efun_pow(void) {
    test_setup("pow() function");
    
    VirtualMachine *vm = vm_init();
    VMValue args[2];
    
    args[0] = vm_value_create_int(2);
    args[1] = vm_value_create_int(3);
    
    VMValue result = efun_pow(vm, args, 2);
    
    test_assert(result.type == VALUE_FLOAT, "Should return float");
    test_assert(result.data.float_value > 7.99 && result.data.float_value < 8.01,
                "2^3 should be ~8.0");
    
    vm_free(vm);
}

void test_efun_random(void) {
    test_setup("random() function");
    
    VirtualMachine *vm = vm_init();
    VMValue args[1];
    
    args[0] = vm_value_create_int(100);
    VMValue result = efun_random(vm, args, 1);
    
    test_assert(result.type == VALUE_INT, "Should return int");
    test_assert(result.data.int_value >= 0 && result.data.int_value < 100,
                "Should be in range [0, 100)");
    
    vm_free(vm);
}

void test_efun_min(void) {
    test_setup("min() function");
    
    VirtualMachine *vm = vm_init();
    VMValue args[2];
    
    args[0] = vm_value_create_int(10);
    args[1] = vm_value_create_int(20);
    
    VMValue result = efun_min(vm, args, 2);
    
    test_assert(result.type == VALUE_INT, "Should return int");
    test_assert(result.data.int_value == 10, "Should return 10");
    
    vm_free(vm);
}

void test_efun_max(void) {
    test_setup("max() function");
    
    VirtualMachine *vm = vm_init();
    VMValue args[2];
    
    args[0] = vm_value_create_int(10);
    args[1] = vm_value_create_int(20);
    
    VMValue result = efun_max(vm, args, 2);
    
    test_assert(result.type == VALUE_INT, "Should return int");
    test_assert(result.data.int_value == 20, "Should return 20");
    
    vm_free(vm);
}

/* ========== TESTS: Type Checking Functions ========== */

void test_efun_intp(void) {
    test_setup("intp() type check");
    
    VirtualMachine *vm = vm_init();
    VMValue args[1];
    
    args[0] = vm_value_create_int(42);
    test_assert(efun_intp(vm, args, 1).data.int_value == 1, "Int should return 1");
    
    args[0] = vm_value_create_string("test");
    test_assert(efun_intp(vm, args, 1).data.int_value == 0, "String should return 0");
    
    vm_free(vm);
}

void test_efun_floatp(void) {
    test_setup("floatp() type check");
    
    VirtualMachine *vm = vm_init();
    VMValue args[1];
    
    args[0] = vm_value_create_float(3.14);
    test_assert(efun_floatp(vm, args, 1).data.int_value == 1, "Float should return 1");
    
    args[0] = vm_value_create_int(42);
    test_assert(efun_floatp(vm, args, 1).data.int_value == 0, "Int should return 0");
    
    vm_free(vm);
}

void test_efun_stringp(void) {
    test_setup("stringp() type check");
    
    VirtualMachine *vm = vm_init();
    VMValue args[1];
    
    args[0] = vm_value_create_string("test");
    test_assert(efun_stringp(vm, args, 1).data.int_value == 1, "String should return 1");
    
    args[0] = vm_value_create_int(42);
    test_assert(efun_stringp(vm, args, 1).data.int_value == 0, "Int should return 0");
    
    vm_free(vm);
}

void test_efun_mappingp(void) {
    test_setup("mappingp() type check");
    
    VirtualMachine *vm = vm_init();
    VMValue args[1];
    
    mapping_t *map = mapping_new(vm->gc, 10);
    args[0] = vm_value_create_null();
    args[0].type = VALUE_MAPPING;
    args[0].data.mapping_value = map;
    
    test_assert(efun_mappingp(vm, args, 1).data.int_value == 1, "Mapping should return 1");
    
    args[0] = vm_value_create_int(42);
    test_assert(efun_mappingp(vm, args, 1).data.int_value == 0, "Int should return 0");
    
    mapping_free(map);
    vm_free(vm);
}

/* ========== TESTS: I/O Functions ========== */

void test_efun_write(void) {
    test_setup("write() function");
    
    VirtualMachine *vm = vm_init();
    VMValue args[1];
    
    args[0] = vm_value_create_string("test output");
    VMValue result = efun_write(vm, args, 1);
    
    test_assert(result.type == VALUE_INT, "Should return int");
    test_assert(result.data.int_value == 1, "Should return 1");
    
    vm_free(vm);
}

void test_efun_printf(void) {
    test_setup("printf() function");
    
    VirtualMachine *vm = vm_init();
    VMValue args[1];
    
    args[0] = vm_value_create_string("test");
    VMValue result = efun_printf(vm, args, 1);
    
    test_assert(result.type == VALUE_INT, "Should return int");
    test_assert(result.data.int_value == 1, "Should return 1");
    
    vm_free(vm);
}

/* ========== TESTS: Efun Call Mechanism ========== */

void test_efun_call(void) {
    test_setup("Call efun through registry");
    
    EfunRegistry *registry = efun_init();
    efun_register_all(registry);
    
    VirtualMachine *vm = vm_init();
    VMValue args[1];
    args[0] = vm_value_create_string("hello");
    
    VMValue result = efun_call(registry, vm, "strlen", args, 1);
    
    test_assert(result.type == VALUE_INT, "Should return int");
    test_assert(result.data.int_value == 5, "Should return 5");
    
    vm_free(vm);
    efun_free(registry);
}

void test_efun_call_invalid(void) {
    test_setup("Call non-existent efun");
    
    EfunRegistry *registry = efun_init();
    efun_register_all(registry);
    
    VirtualMachine *vm = vm_init();
    VMValue args[1];
    args[0] = vm_value_create_int(42);
    
    VMValue result = efun_call(registry, vm, "nonexistent", args, 1);
    
    test_assert(result.type == VALUE_NULL, "Should return null");
    
    vm_free(vm);
    efun_free(registry);
}

void test_efun_call_wrong_args(void) {
    test_setup("Call efun with wrong argument count");
    
    EfunRegistry *registry = efun_init();
    efun_register_all(registry);
    
    VirtualMachine *vm = vm_init();
    VMValue args[2];
    args[0] = vm_value_create_string("test");
    args[1] = vm_value_create_int(1);
    
    VMValue result = efun_call(registry, vm, "strlen", args, 2);  /* strlen takes 1 arg */
    
    test_assert(result.type == VALUE_NULL, "Should return null for wrong arg count");
    
    vm_free(vm);
    efun_free(registry);
}

void test_efun_tell_object(void) {
    test_setup("tell_object() messaging efun");

    VirtualMachine *vm = vm_init();

    /* Create a simple method that returns its first argument */
    VMFunction *recv = vm_function_create("receive_message", 1, 1);
    VMInstruction ins;
    ins.opcode = OP_LOAD_LOCAL;
    ins.operand.int_operand = 0;
    vm_function_add_instruction(recv, ins);
    ins.opcode = OP_RETURN;
    vm_function_add_instruction(recv, ins);

    vm_add_function(vm, recv);

    obj_t *o = obj_new("test_obj");
    obj_add_method(o, recv);

    VMValue args[2];
    args[0] = vm_value_create_null();
    args[0].type = VALUE_OBJECT;
    args[0].data.object_value = o;
    args[1] = vm_value_create_string("hello world");

    VMValue res = efun_tell_object(vm, args, 2);
    test_assert(res.type == VALUE_INT && res.data.int_value == 1, "tell_object should return 1 on success");

    vm_value_free(&args[1]);
    obj_free(o);
    vm_free(vm);
}

void test_efun_file_dir_ops(void) {
    test_setup("File and directory efuns: mkdir, write_file, read_file, file_size, get_dir, rm");

    VirtualMachine *vm = vm_init();

    pid_t pid = getpid();
    char dirpath[256];
    char filepath[320];
    snprintf(dirpath, sizeof(dirpath), "tests/_efun_tmp_%d", (int)pid);
    snprintf(filepath, sizeof(filepath), "%s/test.txt", dirpath);

    /* Cleanup any previous leftovers */
    unlink(filepath);
    rmdir(dirpath);

    VMValue a1[1];
    a1[0] = vm_value_create_string(dirpath);
    VMValue mk = efun_mkdir(vm, a1, 1);
    test_assert(mk.type == VALUE_INT && mk.data.int_value == 1, "mkdir should succeed");
    vm_value_free(&a1[0]);

    VMValue a2[2];
    a2[0] = vm_value_create_string(filepath);
    a2[1] = vm_value_create_string("line1\nline2\n");
    VMValue w = efun_write_file(vm, a2, 2);
    test_assert(w.type == VALUE_INT && w.data.int_value == 1, "write_file should succeed");
    vm_value_free(&a2[0]);
    vm_value_free(&a2[1]);

    VMValue rarg[1];
    rarg[0] = vm_value_create_string(filepath);
    VMValue read = efun_read_file(vm, rarg, 1);
    test_assert(read.type == VALUE_STRING && strstr(read.data.string_value, "line1") != NULL,
                "read_file should return written content");
    vm_value_free(&read);
    vm_value_free(&rarg[0]);

    VMValue sarg[1];
    sarg[0] = vm_value_create_string(filepath);
    VMValue fsize = efun_file_size(vm, sarg, 1);
    test_assert(fsize.type == VALUE_INT && fsize.data.int_value == -1, "file_size should indicate regular file (-1)");
    vm_value_free(&sarg[0]);

    VMValue darg[1];
    darg[0] = vm_value_create_string(dirpath);
    VMValue dsize = efun_file_size(vm, darg, 1);
    test_assert(dsize.type == VALUE_INT && dsize.data.int_value == -2, "file_size should indicate directory (-2)");
    vm_value_free(&darg[0]);

    VMValue garg[1];
    garg[0] = vm_value_create_string(dirpath);
    VMValue listing = efun_get_dir(vm, garg, 1);
    test_assert(listing.type == VALUE_ARRAY, "get_dir should return an array");
    if (listing.type == VALUE_ARRAY) {
        array_t *arr = listing.data.array_value;
        test_assert(array_length(arr) >= 1, "Directory listing should contain at least one entry");
        /* look for our file name */
        int found = 0;
        for (size_t i = 0; i < array_length(arr); i++) {
            VMValue ent = array_get(arr, i);
            if (ent.type == VALUE_STRING && strcmp(ent.data.string_value, "test.txt") == 0) {
                found = 1; break;
            }
        }
        test_assert(found, "Directory listing should include test.txt");
        array_free(arr);
    }

    vm_value_free(&garg[0]);

    VMValue rmar[1];
    rmar[0] = vm_value_create_string(filepath);
    VMValue rmres = efun_rm(vm, rmar, 1);
    test_assert(rmres.type == VALUE_INT && rmres.data.int_value == 1, "rm should remove the file");
    vm_value_free(&rmar[0]);

    /* After removal, file_size should report 0 (not found) */
    VMValue sarg2[1];
    sarg2[0] = vm_value_create_string(filepath);
    VMValue fsize2 = efun_file_size(vm, sarg2, 1);
    test_assert(fsize2.type == VALUE_INT && fsize2.data.int_value == 0, "file_size should report 0 for missing file");
    vm_value_free(&sarg2[0]);

    /* Cleanup directory */
    rmdir(dirpath);

    vm_free(vm);
}

/* ========== Main Test Runner ========== */

int main(void) {
    printf("\n========================================\n");
    printf("AMLP Efun System - Test Suite\n");
    printf("========================================\n");
    
    /* Registry Tests */
    test_efun_init();
    test_efun_register();
    test_efun_find();
    test_efun_register_all();
    
    /* String Function Tests */
    test_efun_strlen();
    test_efun_substring();
    test_efun_upper_case();
    test_efun_lower_case();
    test_efun_trim();
    
    /* Array Function Tests */
    test_efun_sizeof_array();
    test_efun_sizeof_string();
    test_efun_arrayp();
    
    /* Math Function Tests */
    test_efun_abs_int();
    test_efun_abs_float();
    test_efun_sqrt();
    test_efun_pow();
    test_efun_random();
    test_efun_min();
    test_efun_max();
    
    /* Type Checking Tests */
    test_efun_intp();
    test_efun_floatp();
    test_efun_stringp();
    test_efun_mappingp();
    
    /* I/O Function Tests */
    test_efun_write();
    test_efun_printf();
    
    /* Efun Call Mechanism Tests */
    test_efun_call();
    test_efun_call_invalid();
    test_efun_call_wrong_args();
    
    /* Summary */
    printf("\n========================================\n");
    printf("Test Results: %d/%d passed", test_passed, test_count);
    if (test_failed > 0) {
        printf(" (%d failed)", test_failed);
    }
    printf("\n========================================\n\n");
    
    return (test_failed == 0) ? 0 : 1;
}
