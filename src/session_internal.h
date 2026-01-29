/* session_internal.h - Internal PlayerSession definition shared by driver and session.c */
#ifndef SESSION_INTERNAL_H
#define SESSION_INTERNAL_H

#include <time.h>
#include <netinet/in.h>
#include "websocket.h"

#define INPUT_BUFFER_SIZE 2048
#define WS_BUFFER_SIZE 65536

typedef enum {
    STATE_CONNECTING,
    STATE_GET_NAME,
    STATE_GET_PASSWORD,
    STATE_NEW_PASSWORD,
    STATE_CONFIRM_PASSWORD,
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
} PlayerSession;

#endif /* SESSION_INTERNAL_H */
