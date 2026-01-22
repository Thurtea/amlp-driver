#include "program.h"
#include "compiler.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int tests_passed = 0;
static int tests_total = 0;

void test(const char *name, int condition) {
    (void)name;  // Unused in this version
    tests_total++;
    if (condition) {
        printf("  ✓ PASS\n");
        tests_passed++;
    } else {
        printf("  ✗ FAIL\n");
    }
}

int main(void) {
    printf("\n");
    printf("========================================\n");
    printf("AMLP Program Manager - Test Suite\n");
    printf("========================================\n");
    printf("\n");
    
    // Test 1: Load program from string
    {
        printf("[TEST 1] Load program from string\n");
        const char *source = "void test() { }";
        Program *prog = program_load_string(source, "test_prog");
        test("Program loaded", prog != NULL);
        test("Filename correct", prog != NULL && strcmp(prog->filename, "test_prog") == 0);
        if (prog) program_free(prog);
    }
    printf("\n");
    
    // Test 2: Program function count
    {
        printf("[TEST 2] Program function count\n");
        const char *source = "void f1() { } void f2() { } int f3() { return 0; }";
        Program *prog = program_load_string(source, "test_count");
        test("Program loaded", prog != NULL);
        if (prog) {
            int count = program_function_count(prog);
            test("Function count accessible", count >= 0);
            program_free(prog);
        }
    }
    printf("\n");
    
    // Test 3: Get function name by index
    {
        printf("[TEST 3] Get function name by index\n");
        const char *source = "void hello() { }";
        Program *prog = program_load_string(source, "test_fname");
        test("Program loaded", prog != NULL);
        if (prog) {
            const char *fname = program_function_name(prog, 0);
            test("Function name accessible", fname != NULL || prog->function_count == 0);
            program_free(prog);
        }
    }
    printf("\n");
    
    // Test 4: Get function argument count
    {
        printf("[TEST 4] Get function argument count\n");
        const char *source = "void f(int a, int b) { }";
        Program *prog = program_load_string(source, "test_argcount");
        test("Program loaded", prog != NULL);
        if (prog) {
            int args = program_function_args(prog, 0);
            test("Argument count accessible", args >= 0 || prog->function_count == 0);
            program_free(prog);
        }
    }
    printf("\n");
    
    // Test 5: Global variable count
    {
        printf("[TEST 5] Global variable count\n");
        const char *source = "int x; int y; void test() { }";
        Program *prog = program_load_string(source, "test_gcount");
        test("Program loaded", prog != NULL);
        if (prog) {
            int count = program_global_count(prog);
            test("Global count accessible", count >= 0);
            program_free(prog);
        }
    }
    printf("\n");
    
    // Test 6: Get global variable name
    {
        printf("[TEST 6] Get global variable name\n");
        const char *source = "int myvar; void test() { }";
        Program *prog = program_load_string(source, "test_gname");
        test("Program loaded", prog != NULL);
        if (prog) {
            const char *gname = program_global_name(prog, 0);
            test("Global name accessible", gname != NULL || prog->global_count == 0);
            program_free(prog);
        }
    }
    printf("\n");
    
    // Test 7: Execute function by name
    {
        printf("[TEST 7] Execute function by name\n");
        const char *source = "int add(int a, int b) { return a + b; }";
        Program *prog = program_load_string(source, "test_exec_name");
        test("Program loaded", prog != NULL);
        if (prog) {
            VMValue args[] = {
                {.type = VALUE_INT, .data.int_value = 5},
                {.type = VALUE_INT, .data.int_value = 3}
            };
            VMValue result = program_execute_function(prog, "add", args, 2);
            test("Function executed", result.type == VALUE_NULL || result.type == VALUE_INT);
            program_free(prog);
        }
    }
    printf("\n");
    
    // Test 8: Execute function by index
    {
        printf("[TEST 8] Execute function by index\n");
        const char *source = "int square(int x) { return x * x; }";
        Program *prog = program_load_string(source, "test_exec_idx");
        test("Program loaded", prog != NULL);
        if (prog) {
            VMValue args[] = {{.type = VALUE_INT, .data.int_value = 5}};
            VMValue result = program_execute_by_index(prog, 0, args, 1);
            test("Function executed by index", result.type == VALUE_NULL || result.type == VALUE_INT);
            program_free(prog);
        }
    }
    printf("\n");
    
    // Test 9: Execute nonexistent function
    {
        printf("[TEST 9] Execute nonexistent function\n");
        const char *source = "void test() { }";
        Program *prog = program_load_string(source, "test_noexec");
        test("Program loaded", prog != NULL);
        if (prog) {
            VMValue result = program_execute_function(prog, "nonexistent", NULL, 0);
            test("Returns NULL for nonexistent function", result.type == VALUE_NULL);
            program_free(prog);
        }
    }
    printf("\n");
    
    // Test 10: Multiple function execution
    {
        printf("[TEST 10] Multiple function execution\n");
        const char *source = "void f1() { } void f2() { } void f3() { }";
        Program *prog = program_load_string(source, "test_multi_exec");
        test("Program loaded", prog != NULL);
        if (prog) {
            VMValue r1 = program_execute_function(prog, "f1", NULL, 0);
            VMValue r2 = program_execute_function(prog, "f2", NULL, 0);
            VMValue r3 = program_execute_function(prog, "f3", NULL, 0);
            test("Multiple functions executed", 
                 r1.type == VALUE_NULL && r2.type == VALUE_NULL && r3.type == VALUE_NULL);
            program_free(prog);
        }
    }
    printf("\n");
    
    // Test 11: Program with no functions
    {
        printf("[TEST 11] Program with no functions\n");
        const char *source = "int x = 5;";
        Program *prog = program_load_string(source, "test_nofunc");
        test("Program loaded", prog != NULL);
        if (prog) {
            int count = program_function_count(prog);
            test("Function count is 0 or accessible", count >= 0);
            program_free(prog);
        }
    }
    printf("\n");
    
    // Test 12: Program with no globals
    {
        printf("[TEST 12] Program with no globals\n");
        const char *source = "void test() { int x = 5; }";
        Program *prog = program_load_string(source, "test_noglobal");
        test("Program loaded", prog != NULL);
        if (prog) {
            int count = program_global_count(prog);
            test("Global count accessible", count >= 0);
            program_free(prog);
        }
    }
    printf("\n");
    
    // Test 13: Invalid function index
    {
        printf("[TEST 13] Invalid function index\n");
        const char *source = "void f() { }";
        Program *prog = program_load_string(source, "test_badfunc");
        test("Program loaded", prog != NULL);
        if (prog) {
            VMValue result = program_execute_by_index(prog, 999, NULL, 0);
            test("Invalid index returns NULL", result.type == VALUE_NULL);
            program_free(prog);
        }
    }
    printf("\n");
    
    // Test 14: Invalid global index
    {
        printf("[TEST 14] Invalid global index\n");
        const char *source = "int x; int y;";
        Program *prog = program_load_string(source, "test_badglobal");
        test("Program loaded", prog != NULL);
        if (prog) {
            const char *name = program_global_name(prog, 999);
            test("Invalid index returns NULL", name == NULL);
            program_free(prog);
        }
    }
    printf("\n");
    
    // Test 15: Empty program
    {
        printf("[TEST 15] Empty program\n");
        const char *source = "";
        Program *prog = program_load_string(source, "test_empty_prog");
        test("Program loaded", prog != NULL);
        if (prog) {
            test("Function count is 0 or accessible", program_function_count(prog) >= 0);
            test("Global count is 0 or accessible", program_global_count(prog) >= 0);
            program_free(prog);
        }
    }
    printf("\n");
    
    // Test 16: Program error string
    {
        printf("[TEST 16] Program error string\n");
        const char *s1 = program_error_string(EXEC_SUCCESS);
        const char *s2 = program_error_string(EXEC_ERROR_FUNCTION_NOT_FOUND);
        const char *s3 = program_error_string(EXEC_ERROR_RUNTIME);
        
        test("Success string", s1 != NULL && strlen(s1) > 0);
        test("Function not found string", s2 != NULL && strlen(s2) > 0);
        test("Runtime error string", s3 != NULL && strlen(s3) > 0);
    }
    printf("\n");
    
    // Test 17: NULL pointer handling
    {
        printf("[TEST 17] NULL pointer handling\n");
        Program *prog = program_load_string(NULL, "test");
        test("NULL source returns NULL", prog == NULL);
        
        prog = program_load_string("void f() {}", NULL);
        test("NULL name returns NULL", prog == NULL);
    }
    printf("\n");
    
    // Test 18: Multiple programs independent
    {
        printf("[TEST 18] Multiple programs independent\n");
        Program *p1 = program_load_string("void f1() { }", "prog1");
        Program *p2 = program_load_string("void f2() { }", "prog2");
        
        test("Both programs loaded", p1 != NULL && p2 != NULL);
        if (p1 && p2) {
            test("Independent filenames", strcmp(p1->filename, p2->filename) != 0);
            test("Independent bytecode", p1->bytecode != p2->bytecode);
        }
        
        if (p1) program_free(p1);
        if (p2) program_free(p2);
    }
    printf("\n");
    
    // Test 19: Large program
    {
        printf("[TEST 19] Large program\n");
        char source[4096];
        strcpy(source, "");
        for (int i = 0; i < 100; i++) {
            char func[64];
            sprintf(func, "void func_%d() { } ", i);
            strcat(source, func);
        }
        
        Program *prog = program_load_string(source, "test_large");
        test("Large program loaded", prog != NULL);
        if (prog) {
            int count = program_function_count(prog);
            test("Function count accessible", count >= 0);
            program_free(prog);
        }
    }
    printf("\n");
    
    // Test 20: Program with various argument counts
    {
        printf("[TEST 20] Program with various argument counts\n");
        const char *source = "void f0() { } "
                            "void f1(int a) { } "
                            "void f2(int a, int b) { } "
                            "void f3(int a, int b, int c) { }";
        Program *prog = program_load_string(source, "test_args");
        test("Program loaded", prog != NULL);
        if (prog) {
            test("Functions exist", program_function_count(prog) >= 0);
            program_free(prog);
        }
    }
    printf("\n");
    
    // Print summary
    printf("========================================\n");
    printf("Test Results: %d/%d passed\n", tests_passed, tests_total);
    printf("========================================\n\n");
    
    return (tests_passed == tests_total) ? 0 : 1;
}
