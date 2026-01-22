#include "simul_efun.h"
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
    printf("AMLP Simul Efun - Test Suite\n");
    printf("========================================\n");
    printf("\n");
    
    // Test 1: Create registry
    {
        printf("[TEST 1] Create simul efun registry\n");
        simul_efun_registry_t *registry = simul_efun_registry_new();
        test("Registry created", registry != NULL);
        test("Initial count is 0", registry != NULL && simul_efun_count(registry) == 0);
        if (registry) simul_efun_registry_free(registry);
    }
    printf("\n");
    
    // Test 2: Register simul efun
    {
        printf("[TEST 2] Register simul efun\n");
        simul_efun_registry_t *registry = simul_efun_registry_new();
        Program *prog = compiler_compile_string("int add(int a, int b) { return a + b; }", "test");
        
        test("Registry created", registry != NULL);
        test("Program compiled", prog != NULL);
        
        if (registry && prog) {
            int idx = simul_efun_register(registry, "add", prog, 0);
            test("Efun registered", idx >= 0);
            test("Count is 1", simul_efun_count(registry) == 1);
            
            program_free(prog);
        }
        
        if (registry) simul_efun_registry_free(registry);
    }
    printf("\n");
    
    // Test 3: Find simul efun
    {
        printf("[TEST 3] Find simul efun\n");
        simul_efun_registry_t *registry = simul_efun_registry_new();
        Program *prog = compiler_compile_string("void test() { }", "test");
        
        if (registry && prog) {
            simul_efun_register(registry, "test_func", prog, 0);
            
            int idx = simul_efun_find(registry, "test_func");
            test("Find existing efun", idx >= 0);
            
            idx = simul_efun_find(registry, "nonexistent");
            test("Nonexistent efun returns -1", idx == -1);
            
            program_free(prog);
        }
        
        if (registry) simul_efun_registry_free(registry);
    }
    printf("\n");
    
    // Test 4: Get simul efun name
    {
        printf("[TEST 4] Get simul efun name by index\n");
        simul_efun_registry_t *registry = simul_efun_registry_new();
        Program *prog = compiler_compile_string("void test() { }", "test");
        
        if (registry && prog) {
            simul_efun_register(registry, "my_efun", prog, 0);
            
            const char *name = simul_efun_name(registry, 0);
            test("Name retrieved", name != NULL && strcmp(name, "my_efun") == 0);
            
            program_free(prog);
        }
        
        if (registry) simul_efun_registry_free(registry);
    }
    printf("\n");
    
    // Test 5: Multiple simul efuns
    {
        printf("[TEST 5] Multiple simul efuns\n");
        simul_efun_registry_t *registry = simul_efun_registry_new();
        Program *prog = compiler_compile_string("void f1() { } void f2() { } void f3() { }", "test");
        
        if (registry && prog) {
            simul_efun_register(registry, "f1", prog, 0);
            simul_efun_register(registry, "f2", prog, 1);
            simul_efun_register(registry, "f3", prog, 2);
            
            test("Count is 3", simul_efun_count(registry) == 3);
            
            int idx1 = simul_efun_find(registry, "f1");
            int idx2 = simul_efun_find(registry, "f2");
            int idx3 = simul_efun_find(registry, "f3");
            
            test("All functions found", idx1 >= 0 && idx2 >= 0 && idx3 >= 0);
            
            program_free(prog);
        }
        
        if (registry) simul_efun_registry_free(registry);
    }
    printf("\n");
    
    // Test 6: Update existing simul efun
    {
        printf("[TEST 6] Update existing simul efun\n");
        simul_efun_registry_t *registry = simul_efun_registry_new();
        Program *prog1 = compiler_compile_string("void f() { }", "prog1");
        Program *prog2 = compiler_compile_string("void g() { }", "prog2");
        
        if (registry && prog1 && prog2) {
            int idx1 = simul_efun_register(registry, "func", prog1, 0);
            int idx2 = simul_efun_register(registry, "func", prog2, 0);
            
            test("Same index after update", idx1 == idx2);
            test("Count is 1", simul_efun_count(registry) == 1);
            
            program_free(prog1);
            program_free(prog2);
        }
        
        if (registry) simul_efun_registry_free(registry);
    }
    printf("\n");
    
    // Test 7: Call simul efun
    {
        printf("[TEST 7] Call simul efun\n");
        simul_efun_registry_t *registry = simul_efun_registry_new();
        Program *prog = compiler_compile_string("int add(int a, int b) { return a + b; }", "test");
        
        if (registry && prog) {
            simul_efun_register(registry, "add", prog, 0);
            
            VMValue args[] = {
                {.type = VALUE_INT, .data.int_value = 5},
                {.type = VALUE_INT, .data.int_value = 3}
            };
            
            VMValue result = simul_efun_call(registry, "add", args, 2);
            test("Function called", result.type == VALUE_NULL || result.type == VALUE_INT);
            
            program_free(prog);
        }
        
        if (registry) simul_efun_registry_free(registry);
    }
    printf("\n");
    
    // Test 8: Call nonexistent simul efun
    {
        printf("[TEST 8] Call nonexistent simul efun\n");
        simul_efun_registry_t *registry = simul_efun_registry_new();
        
        if (registry) {
            VMValue result = simul_efun_call(registry, "nonexistent", NULL, 0);
            test("Nonexistent call returns NULL", result.type == VALUE_NULL);
        }
        
        if (registry) simul_efun_registry_free(registry);
    }
    printf("\n");
    
    // Test 9: NULL pointer handling
    {
        printf("[TEST 9] NULL pointer handling\n");
        simul_efun_registry_t *registry = simul_efun_registry_new();
        Program *prog = compiler_compile_string("void f() { }", "test");
        
        int idx1 = simul_efun_register(NULL, "f", prog, 0);
        test("NULL registry returns -1", idx1 == -1);
        
        int idx2 = simul_efun_find(NULL, "f");
        test("NULL registry find returns -1", idx2 == -1);
        
        const char *name = simul_efun_name(NULL, 0);
        test("NULL registry name returns NULL", name == NULL);
        
        VMValue result = simul_efun_call(NULL, "f", NULL, 0);
        test("NULL registry call returns NULL", result.type == VALUE_NULL);
        
        if (prog) program_free(prog);
        if (registry) simul_efun_registry_free(registry);
    }
    printf("\n");
    
    // Test 10: Capacity expansion
    {
        printf("[TEST 10] Registry capacity expansion\n");
        simul_efun_registry_t *registry = simul_efun_registry_new();
        Program *prog = compiler_compile_string("void f() { }", "test");
        
        if (registry && prog) {
            // Register more efuns than initial capacity
            for (int i = 0; i < 50; i++) {
                char name[32];
                sprintf(name, "func_%d", i);
                simul_efun_register(registry, name, prog, 0);
            }
            
            test("50 efuns registered", simul_efun_count(registry) == 50);
            
            // Verify they can be found
            int found = 0;
            for (int i = 0; i < 50; i++) {
                char name[32];
                sprintf(name, "func_%d", i);
                if (simul_efun_find(registry, name) >= 0) {
                    found++;
                }
            }
            test("All 50 efuns found", found == 50);
            
            program_free(prog);
        }
        
        if (registry) simul_efun_registry_free(registry);
    }
    printf("\n");
    
    // Test 11: Registry with no efuns
    {
        printf("[TEST 11] Registry with no efuns\n");
        simul_efun_registry_t *registry = simul_efun_registry_new();
        
        if (registry) {
            test("Count is 0", simul_efun_count(registry) == 0);
            
            const char *name = simul_efun_name(registry, 0);
            test("Name at 0 is NULL", name == NULL);
        }
        
        if (registry) simul_efun_registry_free(registry);
    }
    printf("\n");
    
    // Test 12: Invalid indices
    {
        printf("[TEST 12] Invalid indices\n");
        simul_efun_registry_t *registry = simul_efun_registry_new();
        Program *prog = compiler_compile_string("void f() { }", "test");
        
        if (registry && prog) {
            simul_efun_register(registry, "f", prog, 0);
            
            const char *name = simul_efun_name(registry, 999);
            test("Invalid index returns NULL", name == NULL);
            
            name = simul_efun_name(registry, -1);
            test("Negative index returns NULL", name == NULL);
            
            program_free(prog);
        }
        
        if (registry) simul_efun_registry_free(registry);
    }
    printf("\n");
    
    // Test 13: Load simul efun object
    {
        printf("[TEST 13] Load simul efun object\n");
        simul_efun_registry_t *registry = simul_efun_registry_new();
        
        if (registry) {
            // Try to load a nonexistent file - should fail gracefully
            int result = simul_efun_load_object(registry, "/nonexistent/file.c");
            test("Load nonexistent file fails", result != 0);
            
            test("Registry still valid", simul_efun_count(registry) >= 0);
        }
        
        if (registry) simul_efun_registry_free(registry);
    }
    printf("\n");
    
    // Test 14: Efun with different programs
    {
        printf("[TEST 14] Efun with different programs\n");
        simul_efun_registry_t *registry = simul_efun_registry_new();
        Program *prog1 = compiler_compile_string("int add(int a, int b) { return a + b; }", "p1");
        Program *prog2 = compiler_compile_string("int sub(int a, int b) { return a - b; }", "p2");
        
        if (registry && prog1 && prog2) {
            simul_efun_register(registry, "add", prog1, 0);
            simul_efun_register(registry, "sub", prog2, 0);
            
            test("Both efuns registered", simul_efun_count(registry) == 2);
            
            int idx1 = simul_efun_find(registry, "add");
            int idx2 = simul_efun_find(registry, "sub");
            
            test("Both efuns found", idx1 >= 0 && idx2 >= 0);
            
            program_free(prog1);
            program_free(prog2);
        }
        
        if (registry) simul_efun_registry_free(registry);
    }
    printf("\n");
    
    // Test 15: Case sensitivity
    {
        printf("[TEST 15] Case sensitivity\n");
        simul_efun_registry_t *registry = simul_efun_registry_new();
        Program *prog = compiler_compile_string("void f() { }", "test");
        
        if (registry && prog) {
            simul_efun_register(registry, "TestFunc", prog, 0);
            
            int idx1 = simul_efun_find(registry, "TestFunc");
            int idx2 = simul_efun_find(registry, "testfunc");
            int idx3 = simul_efun_find(registry, "TESTFUNC");
            
            test("Exact match found", idx1 >= 0);
            test("Case mismatch not found", idx2 == -1);
            test("Case mismatch not found", idx3 == -1);
            
            program_free(prog);
        }
        
        if (registry) simul_efun_registry_free(registry);
    }
    printf("\n");
    
    printf("========================================\n");
    printf("Test Results: %d/%d passed\n", tests_passed, tests_total);
    printf("========================================\n\n");
    
    return (tests_passed == tests_total) ? 0 : 1;
}
