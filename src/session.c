/* session.c - Driver session implementation for VM context access */

#include "session.h"
#include "session_internal.h"

/* Opaque player session pointer implementation lives in driver; we keep a
 * single pointer here that the driver sets when routing commands into
 * the VM. This file is linked into the driver executable. */

static PlayerSession *vm_current_session = NULL;

void *get_current_player_object(void) {
    return vm_current_session ? vm_current_session->player_object : NULL;
}

void set_current_session(void *session) {
    vm_current_session = (PlayerSession *)session;
}
