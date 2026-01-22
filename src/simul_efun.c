#include "simul_efun.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define INITIAL_SIMUL_EFUN_CAPACITY 32

/**
 * Create new simul efun registry
 */
simul_efun_registry_t* simul_efun_registry_new(void) {
    simul_efun_registry_t *registry = malloc(sizeof(simul_efun_registry_t));
    if (!registry) return NULL;
    
    registry->capacity = INITIAL_SIMUL_EFUN_CAPACITY;
    registry->efuns = malloc(sizeof(simul_efun_t) * registry->capacity);
    registry->count = 0;
    registry->simul_efun_program = NULL;
    
    if (!registry->efuns) {
        free(registry);
        return NULL;
    }
    
    return registry;
}

/**
 * Free simul efun registry
 */
void simul_efun_registry_free(simul_efun_registry_t *registry) {
    if (!registry) return;
    
    if (registry->efuns) {
        for (size_t i = 0; i < registry->count; i++) {
            free(registry->efuns[i].name);
        }
        free(registry->efuns);
    }
    
    if (registry->simul_efun_program) {
        program_decref(registry->simul_efun_program);
    }
    
    free(registry);
}

/**
 * Load simul_efun object from file
 * Returns 0 on success, -1 on failure
 */
int simul_efun_load_object(simul_efun_registry_t *registry, const char *filename) {
    if (!registry || !filename) return -1;
    
    Program *prog = compiler_compile_file(filename);
    if (!prog) {
        fprintf(stderr, "Error: Failed to load simul_efun object from '%s'\n", filename);
        return -1;
    }
    
    if (registry->simul_efun_program) {
        program_decref(registry->simul_efun_program);
    }
    
    registry->simul_efun_program = prog;
    program_incref(prog);
    
    // Register all functions from simul_efun object as efuns
    for (size_t i = 0; i < prog->function_count; i++) {
        simul_efun_register(registry, prog->functions[i].name, prog, i);
    }
    
    return 0;
}

/**
 * Register a simul efun
 */
int simul_efun_register(simul_efun_registry_t *registry, const char *name,
                        Program *program, int function_index) {
    if (!registry || !name || !program || function_index < 0) {
        return -1;
    }
    
    // Check if already registered
    for (size_t i = 0; i < registry->count; i++) {
        if (strcmp(registry->efuns[i].name, name) == 0) {
            // Update existing
            registry->efuns[i].program = program;
            registry->efuns[i].function_index = function_index;
            return i;
        }
    }
    
    // Add new
    if (registry->count >= registry->capacity) {
        registry->capacity *= 2;
        registry->efuns = realloc(registry->efuns,
                                 sizeof(simul_efun_t) * registry->capacity);
        if (!registry->efuns) return -1;
    }
    
    int index = registry->count++;
    registry->efuns[index].name = malloc(strlen(name) + 1);
    strcpy(registry->efuns[index].name, name);
    registry->efuns[index].program = program;
    registry->efuns[index].function_index = function_index;
    
    return index;
}

/**
 * Find simul efun by name
 * Returns index if found, -1 otherwise
 */
int simul_efun_find(simul_efun_registry_t *registry, const char *name) {
    if (!registry || !name) return -1;
    
    for (size_t i = 0; i < registry->count; i++) {
        if (strcmp(registry->efuns[i].name, name) == 0) {
            return i;
        }
    }
    
    return -1;
}

/**
 * Call a simul efun
 * Phase 7 Note: Execution integration in Phase 7 Iteration 2
 */
VMValue simul_efun_call(simul_efun_registry_t *registry, const char *name,
                        VMValue *args, int arg_count) {
    (void)args;        // Unused in Phase 7 iteration 1
    (void)arg_count;   // Unused in Phase 7 iteration 1
    
    if (!registry || !name) {
        return (VMValue){.type = VALUE_NULL};
    }
    
    int index = simul_efun_find(registry, name);
    if (index < 0) {
        return (VMValue){.type = VALUE_NULL};
    }
    
    // simul_efun_t *efun = &registry->efuns[index];
    
    // Phase 7 Iteration 2: Execute the simul efun function
    // For now, return NULL as placeholder
    
    return (VMValue){.type = VALUE_NULL};
}

/**
 * Get count of registered simul efuns
 */
int simul_efun_count(simul_efun_registry_t *registry) {
    if (!registry) return 0;
    return registry->count;
}

/**
 * Get simul efun name by index
 */
const char* simul_efun_name(simul_efun_registry_t *registry, int index) {
    if (!registry || index < 0 || index >= (int)registry->count) {
        return NULL;
    }
    return registry->efuns[index].name;
}
