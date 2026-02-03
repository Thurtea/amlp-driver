/* session_internal.h - Internal PlayerSession definition shared by driver and session.c */
#ifndef SESSION_INTERNAL_H
#define SESSION_INTERNAL_H

#include <time.h>
#include <netinet/in.h>
#include "websocket.h"
#include "chargen.h"  /* Character generation system */

/* Forward declarations */
typedef struct Room Room;

#define INPUT_BUFFER_SIZE 2048
#define WS_BUFFER_SIZE 65536

typedef enum {
    STATE_CONNECTING,
    STATE_GET_NAME,
    STATE_GET_PASSWORD,
    STATE_NEW_PASSWORD,
    STATE_CONFIRM_PASSWORD,
    STATE_CHARGEN,      /* New state for character generation */
    STATE_PLAYING,
    STATE_DISCONNECTING
} SessionState;

typedef enum {
    CONN_TELNET,
    CONN_WEBSOCKET
} ConnectionType;

typedef struct PlayerSession {
    int fd;
    SessionState state;
    ConnectionType connection_type;
    WSState ws_state;
    char username[64];
    char password_buffer[128];
    char input_buffer[INPUT_BUFFER_SIZE];
    size_t input_length;
    uint8_t ws_buffer[WS_BUFFER_SIZE];
    size_t ws_buffer_length;
    time_t last_activity;
    time_t connect_time;
    void *player_object;
    char ip_address[INET_ADDRSTRLEN];
    int privilege_level;
    char *current_dir;       /* Wizard/admin filesystem navigation directory */
    
    /* Character generation and game state */
    ChargenState chargen_state;
    int chargen_page;        /* Current page for paginated menus */
    int chargen_temp_choice; /* Temporary storage for menu selection */
    Character character;
    Room *current_room;
} PlayerSession;

#endif /* SESSION_INTERNAL_H */
