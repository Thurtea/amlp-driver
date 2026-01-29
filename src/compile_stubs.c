/* compile_stubs.c - Provide minimal stubs for compiler/program loader
 *
 * Tests that build efun.c may not link the full compiler/program_loader
 * (these are large and not needed for many unit tests). Provide tiny
 * stubs so tests link successfully; real implementations are used
 * when linking the full driver.
 */

#include "compiler.h"
#include "program_loader.h"

#include "session.h"


Program *compiler_compile_file(const char *path) {
    (void)path;
    return NULL;
}

int program_loader_load(VirtualMachine *vm, Program *prog) {
    (void)vm; (void)prog; return -1;
}

void program_free(Program *prog) {
    (void)prog;
}

/* Provide session stubs used by unit tests so they don't need to link
 * the driver. These are weak/simple and return NULL/do-nothing. */
void *get_current_player_object(void) {
    return NULL;
}

void set_current_session(void *session) {
    (void)session;
}
