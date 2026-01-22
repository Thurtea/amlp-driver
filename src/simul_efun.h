#ifndef SIMUL_EFUN_H
#define SIMUL_EFUN_H

#include "compiler.h"
#include "vm.h"

/**
 * Simulated efuns - LPC functions that behave like built-in efuns
 * Allows LPC code to define custom efun-like functions in a simul_efun object
 */

typedef struct {
    char *name;
    Program *program;
    int function_index;
} simul_efun_t;

typedef struct {
    simul_efun_t *efuns;
    size_t count;
    size_t capacity;
    Program *simul_efun_program;  // The simul_efun.c object
} simul_efun_registry_t;

// Registry management
simul_efun_registry_t* simul_efun_registry_new(void);
void simul_efun_registry_free(simul_efun_registry_t *registry);

// Loading simul_efun object
int simul_efun_load_object(simul_efun_registry_t *registry, const char *filename);
int simul_efun_register(simul_efun_registry_t *registry, const char *name,
                        Program *program, int function_index);

// Efun lookup and execution
int simul_efun_find(simul_efun_registry_t *registry, const char *name);
VMValue simul_efun_call(simul_efun_registry_t *registry, const char *name,
                        VMValue *args, int arg_count);

// Utility
int simul_efun_count(simul_efun_registry_t *registry);
const char* simul_efun_name(simul_efun_registry_t *registry, int index);

#endif
