#ifndef SESSION_H
#define SESSION_H

/* session.h - Session API for efuns to access current player object */

/* Return the current player object for the executing VM context, or NULL. */
void *get_current_player_object(void);

/* Set the current session for the VM context (opaque pointer). */
void set_current_session(void *session);

#endif /* SESSION_H */
