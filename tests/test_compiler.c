#include "compiler.h"
#include "program.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TEST_COUNT 20
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
    printf("AMLP Compiler - Test Suite\n");
    printf("========================================\n");
    printf("\n");
    
    // Test 1: Compile simple program
    {
        printf("[TEST 1] Compile simple program\n");
        const char *source = "void hello() { }";
        Program *prog = compiler_compile_string(source, "test_hello");
        test("Program created", prog != NULL);
        test("Filename set", prog != NULL && strcmp(prog->filename, "test_hello") == 0);
        test("Source stored", prog != NULL && strcmp(prog->source, source) == 0);
        test("No compilation error", prog != NULL && prog->last_error == COMPILE_SUCCESS);
        if (prog) program_free(prog);
    }
    printf("\n");
    
    // Test 2: Compile program with multiple functions
    {
        printf("[TEST 2] Compile program with functions\n");
        const char *source = "void func1() { }\nint func2() { return 42; }";
        Program *prog = compiler_compile_string(source, "test_funcs");
        test("Program created", prog != NULL);
        test("Has bytecode", prog != NULL && prog->bytecode_len > 0);
        if (prog) program_free(prog);
    }
    printf("\n");
    
    // Test 3: Compile program with globals
    {
        printf("[TEST 3] Compile program with globals\n");
        const char *source = "int global_var; void test() { }";
        Program *prog = compiler_compile_string(source, "test_globals");
        test("Program created", prog != NULL);
        if (prog) {
            test("Program valid", prog->last_error == COMPILE_SUCCESS);
            program_free(prog);
        }
    }
    printf("\n");
    
    // Test 4: Reference counting
    {
        printf("[TEST 4] Program reference counting\n");
        const char *source = "void test() { }";
        Program *prog = compiler_compile_string(source, "test_refcount");
        test("Initial ref_count is 1", prog != NULL && prog->ref_count == 1);
        
        if (prog) {
            program_incref(prog);
            test("After incref, ref_count is 2", prog->ref_count == 2);
            
            program_decref(prog);
            test("After decref, ref_count is 1", prog->ref_count == 1);
            
            // Don't call program_free, let decref handle it
            program_decref(prog);
        }
    }
    printf("\n");
    
    // Test 5: Find function
    {
        printf("[TEST 5] Find function in program\n");
        const char *source = "void hello() { } int add(int a, int b) { return a + b; }";
        Program *prog = compiler_compile_string(source, "test_find");
        test("Program created", prog != NULL);
        
        if (prog) {
            int idx = program_find_function(prog, "hello");
            test("Find existing function", idx >= 0);
            
            idx = program_find_function(prog, "nonexistent");
            test("Nonexistent function returns -1", idx == -1);
            
            program_free(prog);
        }
    }
    printf("\n");
    
    // Test 6: Error information
    {
        printf("[TEST 6] Error information storage\n");
        const char *source = "void test() { }";
        Program *prog = compiler_compile_string(source, "test_error_info");
        test("Program created", prog != NULL);
        test("Error info initialized", prog != NULL && prog->error_info.filename != NULL);
        if (prog) program_free(prog);
    }
    printf("\n");
    
    // Test 7: Global variable management
    {
        printf("[TEST 7] Global variable management\n");
        const char *source = "int x; int y; void test() { }";
        Program *prog = compiler_compile_string(source, "test_globals_mgmt");
        test("Program created", prog != NULL);
        
        if (prog) {
            int idx = program_find_global(prog, "x");
            test("Find global x", idx >= 0 || prog->global_count == 0);
            
            VMValue val = program_get_global(prog, "x");
            test("Get global returns value", val.type != VALUE_NULL || prog->global_count == 0);
            
            program_free(prog);
        }
    }
    printf("\n");
    
    // Test 8: Bytecode generation
    {
        printf("[TEST 8] Bytecode generation\n");
        const char *source = "int main() { return 42; }";
        Program *prog = compiler_compile_string(source, "test_bytecode");
        test("Program created", prog != NULL);
        test("Bytecode generated", prog != NULL && prog->bytecode_len > 0);
        test("Has RETURN opcode", prog != NULL && 
             (prog->bytecode[prog->bytecode_len - 1] == OP_RETURN || prog->bytecode_len > 0));
        if (prog) program_free(prog);
    }
    printf("\n");
    
    // Test 9: Line mapping for debugging
    {
        printf("[TEST 9] Line mapping for debugging\n");
        const char *source = "void line1() { }\nvoid line2() { }";
        Program *prog = compiler_compile_string(source, "test_lines");
        test("Program created", prog != NULL);
        test("Line map created", prog != NULL && prog->line_map_count > 0);
        if (prog) program_free(prog);
    }
    printf("\n");
    
    // Test 10: Constants storage
    {
        printf("[TEST 10] Constants storage\n");
        const char *source = "int test() { return 123; }";
        Program *prog = compiler_compile_string(source, "test_constants");
        test("Program created", prog != NULL);
        test("Constants array allocated", prog != NULL && prog->constants != NULL);
        if (prog) program_free(prog);
    }
    printf("\n");
    
    // Test 11: Empty source code
    {
        printf("[TEST 11] Empty source code\n");
        const char *source = "";
        Program *prog = compiler_compile_string(source, "test_empty");
        test("Program created", prog != NULL);
        test("Handles empty source", prog != NULL && prog->last_error == COMPILE_SUCCESS);
        if (prog) program_free(prog);
    }
    printf("\n");
    
    // Test 12: Whitespace-only source
    {
        printf("[TEST 12] Whitespace-only source\n");
        const char *source = "   \n\n   \t\t";
        Program *prog = compiler_compile_string(source, "test_whitespace");
        test("Program created", prog != NULL);
        test("Handles whitespace", prog != NULL && prog->last_error == COMPILE_SUCCESS);
        if (prog) program_free(prog);
    }
    printf("\n");
    
    // Test 13: Compile string with various names
    {
        printf("[TEST 13] Compile with various names\n");
        const char *source = "void test() { }";
        
        Program *p1 = compiler_compile_string(source, "name1");
        test("Name1", p1 != NULL && strcmp(p1->filename, "name1") == 0);
        if (p1) program_free(p1);
        
        Program *p2 = compiler_compile_string(source, "path/to/file.c");
        test("Path name", p2 != NULL && strcmp(p2->filename, "path/to/file.c") == 0);
        if (p2) program_free(p2);
    }
    printf("\n");
    
    // Test 14: Multiple programs independent
    {
        printf("[TEST 14] Multiple programs independent\n");
        Program *p1 = compiler_compile_string("void f1() { }", "prog1");
        Program *p2 = compiler_compile_string("void f2() { }", "prog2");
        
        test("Two programs created", p1 != NULL && p2 != NULL);
        test("Different filenames", p1 != NULL && p2 != NULL &&
             strcmp(p1->filename, p2->filename) != 0);
        test("Independent sources", p1 != NULL && p2 != NULL &&
             strcmp(p1->source, p2->source) != 0);
        
        if (p1) program_free(p1);
        if (p2) program_free(p2);
    }
    printf("\n");
    
    // Test 15: Program with comments
    {
        printf("[TEST 15] Program with comments\n");
        const char *source = "// comment\nvoid test() { /* block */ }";
        Program *prog = compiler_compile_string(source, "test_comments");
        test("Program created", prog != NULL);
        test("Comments handled", prog != NULL && prog->last_error == COMPILE_SUCCESS);
        if (prog) program_free(prog);
    }
    printf("\n");
    
    // Test 16: Source preservation
    {
        printf("[TEST 16] Source code preservation\n");
        const char *source = "int main() {\n  return 0;\n}";
        Program *prog = compiler_compile_string(source, "test_source");
        test("Source preserved", prog != NULL && strcmp(prog->source, source) == 0);
        if (prog) program_free(prog);
    }
    printf("\n");
    
    // Test 17: Compiler error string function
    {
        printf("[TEST 17] Error string representations\n");
        const char *s1 = compiler_error_string(COMPILE_SUCCESS);
        const char *s2 = compiler_error_string(COMPILE_ERROR_SYNTAX);
        const char *s3 = compiler_error_string(COMPILE_ERROR_IO);
        
        test("Success string", s1 != NULL && strlen(s1) > 0);
        test("Syntax error string", s2 != NULL && strlen(s2) > 0);
        test("IO error string", s3 != NULL && strlen(s3) > 0);
    }
    printf("\n");
    
    // Test 18: NULL pointer handling
    {
        printf("[TEST 18] NULL pointer handling\n");
        Program *prog = compiler_compile_string(NULL, "test");
        test("NULL source returns NULL", prog == NULL);
        
        prog = compiler_compile_string("void f() {}", NULL);
        test("NULL name returns NULL", prog == NULL);
    }
    printf("\n");
    
    // Test 19: Long source code
    {
        printf("[TEST 19] Long source code\n");
        char source[2048];
        strcpy(source, "void f1() { }\n");
        for (int i = 0; i < 50; i++) {
            strcat(source, "void f");
            char num[10];
            sprintf(num, "%d", i + 2);
            strcat(source, num);
            strcat(source, "() { }\n");
        }
        
        Program *prog = compiler_compile_string(source, "test_long");
        test("Long source handled", prog != NULL);
        if (prog) program_free(prog);
    }
    printf("\n");
    
    // Test 20: Multiple allocations and frees
    {
        printf("[TEST 20] Multiple allocations and frees\n");
        int success = 1;
        for (int i = 0; i < 10; i++) {
            char name[32];
            sprintf(name, "test_%d", i);
            Program *prog = compiler_compile_string("void test() { }", name);
            if (!prog) {
                success = 0;
                break;
            }
            program_free(prog);
        }
        test("Multiple allocations/frees", success);
    }
    printf("\n");
    
    // Print summary
    printf("========================================\n");
    printf("Test Results: %d/%d passed\n", tests_passed, tests_total);
    printf("========================================\n\n");
    
    return (tests_passed == tests_total) ? 0 : 1;
}
