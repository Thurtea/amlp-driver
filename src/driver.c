/*
 * driver.c - AMLP MUD Driver with Network Server
 * 
 * Main executable combining:
 * - LPC compiler and Virtual Machine
 * - Multi-client network server using select()
 * - Login system with character creation
 * - Session management and privilege system
 * - Command execution pipeline
 * - WebSocket support for web clients
 * 
 * Usage:
 *   ./driver [telnet_port] [ws_port] [master_file]
 * 
 * Default telnet port: 3000
 * Default WebSocket port: 3001
 * Default master: lib/secure/master.c
 */

#define _GNU_SOURCE  /* for memmem(), strcasestr() */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <time.h>
#include <ctype.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "vm.h"
#include "compiler.h"
#include "master_object.h"
#include "websocket.h"
/* #include "object.h" */

#define MAX_CLIENTS 100
#define BUFFER_SIZE 4096
#define INPUT_BUFFER_SIZE 2048
#define WS_BUFFER_SIZE 65536
#define DEFAULT_PORT 3000
#define DEFAULT_WS_PORT 3001
#define DEFAULT_MASTER_PATH "lib/secure/master.c"
#define SESSION_TIMEOUT 1800  /* 30 minutes */

/* Connection types */
typedef enum {
    CONN_TELNET,       /* Traditional telnet connection */
    CONN_WEBSOCKET     /* WebSocket connection */
} ConnectionType;

/* Player session states */
typedef enum {
    STATE_CONNECTING,      /* Just connected, need to show login prompt */
    STATE_GET_NAME,        /* Waiting for username */
    STATE_GET_PASSWORD,    /* Waiting for password (existing user) */
    STATE_NEW_PASSWORD,    /* Setting password (new user) */
    STATE_CONFIRM_PASSWORD,/* Confirming new password */
    STATE_PLAYING,         /* Logged in and playing */
    STATE_DISCONNECTING    /* Graceful disconnect in progress */
} SessionState;

/* Player session data */
typedef struct {
    int fd;                           /* Socket file descriptor */
    SessionState state;               /* Current session state */
    ConnectionType connection_type;   /* Telnet or WebSocket */
    WSState ws_state;                 /* WebSocket state (if WS) */
    char username[64];                /* Player username */
    char password_buffer[128];        /* Temporary password storage */
    char input_buffer[INPUT_BUFFER_SIZE]; /* Accumulated input */
    size_t input_length;              /* Current input length */
    uint8_t ws_buffer[WS_BUFFER_SIZE]; /* WebSocket frame buffer */
    size_t ws_buffer_length;          /* WebSocket buffer length */
    time_t last_activity;             /* Last activity timestamp */
    time_t connect_time;              /* Connection timestamp */
    void *player_object;            /* Player's in-game object */
    char ip_address[INET_ADDRSTRLEN]; /* Client IP address */
    int privilege_level;              /* 0=player, 1=wizard, 2=admin */
} PlayerSession;

/* Global state */
static volatile sig_atomic_t server_running = 1;
static VirtualMachine *global_vm = NULL;
static PlayerSession *sessions[MAX_CLIENTS];
static int first_player_created = 0;  /* Track if first player has logged in */

/* Function prototypes */
void handle_shutdown_signal(int sig);
int initialize_vm(const char *master_path);
void cleanup_vm(void);
void init_session(PlayerSession *session, int fd, const char *ip, ConnectionType conn_type);
void free_session(PlayerSession *session);
void handle_session_input(PlayerSession *session, const char *input);
void handle_websocket_data(PlayerSession *session, const uint8_t *data, size_t len);
void process_login_state(PlayerSession *session, const char *input);
void process_playing_state(PlayerSession *session, const char *input);
void send_to_player(PlayerSession *session, const char *format, ...);
void send_prompt(PlayerSession *session);
VMValue execute_command(PlayerSession *session, const char *command);
void broadcast_message(const char *message, PlayerSession *exclude);
void check_session_timeouts(void);
void* create_player_object(const char *username, const char *password_hash);
VMValue call_player_command(void *player_obj, const char *command);
int setup_ws_listener(int port);

/* Signal handler */
void handle_shutdown_signal(int sig) {
    (void)sig;
    fprintf(stderr, "\n[Server] Received shutdown signal\n");
    server_running = 0;
}

/* Initialize VM and master object */
int initialize_vm(const char *master_path) {
    fprintf(stderr, "[Server] Initializing VM...\n");
    
    global_vm = vm_init();
    if (!global_vm) {
        fprintf(stderr, "[Server] ERROR: Failed to initialize VM\n");
        return -1;
    }
    
    fprintf(stderr, "[Server] Loading master object: %s\n", master_path);
    
    if (master_object_init(master_path, global_vm) != 0) {
        fprintf(stderr, "[Server] ERROR: Failed to load master object\n");
        vm_free(global_vm);
        global_vm = NULL;
        return -1;
    }
    
    fprintf(stderr, "[Server] VM initialized successfully\n");
    return 0;
}

/* Cleanup VM resources */
void cleanup_vm(void) {
    if (global_vm) {
        fprintf(stderr, "[Server] Cleaning up VM...\n");
        master_object_cleanup();
        vm_free(global_vm);
        global_vm = NULL;
    }
}

/* Create player object through VM */
void* create_player_object(const char *username, const char *password_hash __attribute__((unused))) {
    if (!global_vm) return NULL;
    
    fprintf(stderr, "[Server] Creating player object for: %s\n", username);
    
    /* TODO: Implement when object system is ready
     * 
     * VMValue path = vm_value_create_string("/std/player");
     * vm_push_value(global_vm, path);
     * 
     * VMValue result = master_call("clone_object", &path, 1);
     * 
     * if (result.type == VALUE_OBJECT) {
     *     Object *player = result.data.object_value;
     *     
     *     // Call setup_player(username, password_hash)
     *     VMValue args[2];
     *     args[0] = vm_value_create_string(username);
     *     args[1] = vm_value_create_string(password_hash);
     *     
     *     object_call_method(player, "setup_player", args, 2);
     *     
     *     vm_value_free(&args[0]);
     *     vm_value_free(&args[1]);
     *     
     *     return player;
     * }
     */
    
    /* For now, return placeholder */
    return (void*)1;  /* Non-null to indicate "created" */
}

/* Call player object's process_command method */
VMValue call_player_command(void *player_obj, const char *command) {
    VMValue result;
    result.type = VALUE_NULL;
    
    if (!player_obj || !global_vm) {
        return result;
    }
    
    fprintf(stderr, "[Server] Calling player command: %s\n", command);
    
    /* TODO: Implement when object system is ready
     *
     * VMValue cmd_arg = vm_value_create_string(command);
     * vm_push_value(global_vm, cmd_arg);
     * 
     * result = object_call_method((Object*)player_obj, "process_command", &cmd_arg, 1);
     * 
     * vm_value_free(&cmd_arg);
     */
    
    return result;
}

/* Initialize a new player session */
void init_session(PlayerSession *session, int fd, const char *ip, ConnectionType conn_type) {
    memset(session, 0, sizeof(PlayerSession));
    session->fd = fd;
    session->state = STATE_CONNECTING;
    session->connection_type = conn_type;
    session->ws_state = (conn_type == CONN_WEBSOCKET) ? WS_STATE_CONNECTING : WS_STATE_CLOSED;
    session->last_activity = time(NULL);
    session->connect_time = time(NULL);
    session->player_object = NULL;
    session->privilege_level = 0;  /* Default to player */
    strncpy(session->ip_address, ip, INET_ADDRSTRLEN - 1);
    session->input_length = 0;
    session->ws_buffer_length = 0;
}

/* Free session resources */
void free_session(PlayerSession *session) {
    if (!session) return;
    
    if (session->player_object) {
        /* TODO: Call destruct on player object when object system ready */
        session->player_object = NULL;
    }
    
    if (session->fd > 0) {
        close(session->fd);
        session->fd = -1;
    }
    
    free(session);
}

/* Send formatted output to player */
void send_to_player(PlayerSession *session, const char *format, ...) {
    if (!session || session->fd <= 0) return;
    
    char buffer[BUFFER_SIZE];
    va_list args;
    va_start(args, format);
    int len = vsnprintf(buffer, sizeof(buffer) - 3, format, args);
    va_end(args);
    
    if (len > 0 && len < BUFFER_SIZE - 3) {
        if (session->connection_type == CONN_WEBSOCKET) {
            /* WebSocket: send as text frame */
            if (session->ws_state == WS_STATE_OPEN) {
                /* Convert ANSI codes for web display */
                char *web_text = ws_convert_ansi(buffer, 1);
                if (web_text) {
                    /* Normalize line endings for web */
                    char *normalized = ws_normalize_line_endings(web_text);
                    free(web_text);
                    
                    if (normalized) {
                        size_t frame_len;
                        uint8_t *frame = ws_encode_text(normalized, &frame_len);
                        free(normalized);
                        
                        if (frame) {
                            send(session->fd, frame, frame_len, 0);
                            free(frame);
                        }
                    }
                }
            }
        } else {
            /* Telnet: ensure CRLF line endings */
            if (buffer[len-1] == '\n' && (len < 2 || buffer[len-2] != '\r')) {
                buffer[len-1] = '\r';
                buffer[len] = '\n';
                buffer[len+1] = '\0';
                len++;
            }
            send(session->fd, buffer, len, 0);
        }
    }
}

/* Send command prompt based on state */
void send_prompt(PlayerSession *session) {
    switch (session->state) {
        case STATE_CONNECTING:
            send_to_player(session,
                "\r\n"
                "╔═══════════════════════════════════════╗\r\n"
                "║    AMLP Driver - Development Server    ║\r\n"
                "║             Version 0.1.0             ║\r\n"
                "╚═══════════════════════════════════════╝\r\n"
                "\r\n"
                "Welcome to the AMLP MUD!\r\n"
                "\r\n"
                "Enter your name: ");
            session->state = STATE_GET_NAME;
            break;
            
        case STATE_GET_NAME:
            send_to_player(session, "Enter your name: ");
            break;
            
        case STATE_GET_PASSWORD:
            send_to_player(session, "Password: ");
            break;
            
        case STATE_NEW_PASSWORD:
            send_to_player(session, "Choose a password: ");
            break;
            
        case STATE_CONFIRM_PASSWORD:
            send_to_player(session, "Confirm password: ");
            break;
            
        case STATE_PLAYING:
            send_to_player(session, "\r\n> ");
            break;
            
        default:
            break;
    }
}

/* Execute command through VM */
VMValue execute_command(PlayerSession *session, const char *command) {
    VMValue result;
    result.type = VALUE_NULL;
    
    if (!global_vm || !session) {
        return result;
    }
    
    /* If player object exists, route command through it */
    if (session->player_object) {
        result = call_player_command(session->player_object, command);
        
        /* If VM returns valid result, use it */
        if (result.type == VALUE_STRING && result.data.string_value) {
            return result;
        }
    }
    
    /* Fallback to built-in commands if no player object or no result */
    
    char cmd_buffer[256];
    strncpy(cmd_buffer, command, sizeof(cmd_buffer) - 1);
    cmd_buffer[sizeof(cmd_buffer) - 1] = '\0';
    
    /* Convert to lowercase for comparison */
    char *cmd = cmd_buffer;
    for (int i = 0; cmd[i]; i++) {
        cmd[i] = tolower(cmd[i]);
    }
    
    /* Parse command and arguments */
    char *args = strchr(cmd, ' ');
    if (args) {
        *args = '\0';
        args++;
        while (*args == ' ') args++;
    }
    
    /* Built-in commands */
    if (strcmp(cmd, "quit") == 0 || strcmp(cmd, "logout") == 0) {
        result.type = VALUE_STRING;
        result.data.string_value = strdup("quit");
        return result;
    }
    
    if (strcmp(cmd, "help") == 0) {
        result.type = VALUE_STRING;
        
        char help_text[2048];
        strcpy(help_text, 
            "Available commands:\r\n"
            "  help                 - Show this help\r\n"
            "  look / l             - Look at your surroundings\r\n"
            "  inventory / i        - Check your inventory\r\n"
            "  say <message>        - Say something\r\n"
            "  emote <action>       - Perform an emote\r\n"
            "  who                  - List players online\r\n"
            "  stats                - Show your character stats\r\n"
            "  quit / logout        - Disconnect\r\n"
            "\r\nMovement: north, south, east, west, up, down (or n, s, e, w, u, d)\r\n");
        
        if (session->privilege_level >= 1) {
            strcat(help_text,
                "\r\nWIZARD COMMANDS (Level 1+):\r\n"
                "  goto <room>         - Teleport to a room\r\n"
                "  clone <object>      - Clone an object\r\n");
        }
        
        if (session->privilege_level >= 2) {
            strcat(help_text,
                "\r\nADMIN COMMANDS (Level 2):\r\n"
                "  promote <player> <level> - Promote player (0=player, 1=wizard, 2=admin)\r\n"
                "  users                     - Show detailed user list\r\n"
                "  shutdown [delay]          - Shutdown server (optional delay in seconds)\r\n");
        }
        
        result.data.string_value = strdup(help_text);
        return result;
    }
    
    if (strcmp(cmd, "look") == 0 || strcmp(cmd, "l") == 0) {
        result.type = VALUE_STRING;
        result.data.string_value = strdup(
            "You are in the starting room.\r\n"
            "This is a simple test environment for the AMLP driver.\r\n"
            "Obvious exits: none yet\r\n");
        return result;
    }
    
    if (strcmp(cmd, "inventory") == 0 || strcmp(cmd, "i") == 0) {
        result.type = VALUE_STRING;
        result.data.string_value = strdup("You are carrying nothing.\r\n");
        return result;
    }
    
    if (strcmp(cmd, "say") == 0) {
        if (args && *args) {
            char msg[BUFFER_SIZE];
            snprintf(msg, sizeof(msg), "%s says: %s\r\n", 
                    session->username, args);
            broadcast_message(msg, session);
            
            snprintf(msg, sizeof(msg), "You say: %s\r\n", args);
            result.type = VALUE_STRING;
            result.data.string_value = strdup(msg);
        } else {
            result.type = VALUE_STRING;
            result.data.string_value = strdup("Say what?\r\n");
        }
        return result;
    }
    
    if (strcmp(cmd, "emote") == 0) {
        if (args && *args) {
            char msg[BUFFER_SIZE];
            snprintf(msg, sizeof(msg), "%s %s\r\n", session->username, args);
            broadcast_message(msg, session);
            result.type = VALUE_STRING;
            result.data.string_value = strdup(msg);
        } else {
            result.type = VALUE_STRING;
            result.data.string_value = strdup("Emote what?\r\n");
        }
        return result;
    }
    
    if (strcmp(cmd, "who") == 0) {
        char msg[BUFFER_SIZE];
        int count = 0;
        strcpy(msg, "Players online:\r\n");
        
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (sessions[i] && sessions[i]->state == STATE_PLAYING) {
                char line[128];
                time_t idle = time(NULL) - sessions[i]->last_activity;
                const char *priv = (sessions[i]->privilege_level == 2) ? "[Admin]" :
                                  (sessions[i]->privilege_level == 1) ? "[Wiz]" : "";
                snprintf(line, sizeof(line), "  %-20s %s(idle: %ld seconds)\r\n",
                        sessions[i]->username, priv, idle);
                strcat(msg, line);
                count++;
            }
        }
        
        char footer[64];
        snprintf(footer, sizeof(footer), "\r\nTotal: %d player%s\r\n", 
                count, count == 1 ? "" : "s");
        strcat(msg, footer);
        
        result.type = VALUE_STRING;
        result.data.string_value = strdup(msg);
        return result;
    }
    
    if (strcmp(cmd, "stats") == 0) {
        char msg[512];
        const char *priv_name = (session->privilege_level == 2) ? "Admin" :
                               (session->privilege_level == 1) ? "Wizard" : "Player";
        snprintf(msg, sizeof(msg),
            "Your stats:\r\n"
            "  Name: %s\r\n"
            "  Privilege Level: %d (%s)\r\n"
            "  Level: 1\r\n"
            "  HP: 100/100\r\n"
            "  Strength: 10\r\n"
            "  Dexterity: 10\r\n"
            "  Intelligence: 10\r\n",
            session->username, session->privilege_level, priv_name);
        
        result.type = VALUE_STRING;
        result.data.string_value = strdup(msg);
        return result;
    }
    
    /* Movement commands */
    const char *directions[] = {"north", "south", "east", "west", "up", "down", 
                               "n", "s", "e", "w", "u", "d", NULL};
    for (int i = 0; directions[i]; i++) {
        if (strcmp(cmd, directions[i]) == 0) {
            result.type = VALUE_STRING;
            result.data.string_value = strdup("You can't go that way.\r\n");
            return result;
        }
    }
    
    /* Admin commands */
    if (strcmp(cmd, "promote") == 0) {
        if (session->privilege_level < 2) {
            result.type = VALUE_STRING;
            result.data.string_value = strdup("You don't have permission to use that command.\r\n");
            return result;
        }
        
        if (!args || *args == '\0') {
            result.type = VALUE_STRING;
            result.data.string_value = strdup(
                "Usage: promote <player> <level>\r\n"
                "Levels: 0=player, 1=wizard, 2=admin\r\n");
            return result;
        }
        
        char target_name[64];
        int new_level;
        if (sscanf(args, "%63s %d", target_name, &new_level) != 2) {
            result.type = VALUE_STRING;
            result.data.string_value = strdup(
                "Usage: promote <player> <level>\r\n"
                "Levels: 0=player, 1=wizard, 2=admin\r\n");
            return result;
        }
        
        if (new_level < 0 || new_level > 2) {
            result.type = VALUE_STRING;
            result.data.string_value = strdup("Invalid level. Use 0 (player), 1 (wizard), or 2 (admin).\r\n");
            return result;
        }
        
        // Find and promote player
        int promoted = 0;
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (sessions[i] && sessions[i]->state == STATE_PLAYING &&
                strcmp(sessions[i]->username, target_name) == 0) {
                sessions[i]->privilege_level = new_level;
                promoted = 1;
                break;
            }
        }
        
        if (promoted) {
            char msg[256];
            const char *level_name = (new_level == 2) ? "Admin" : 
                                     (new_level == 1) ? "Wizard" : "Player";
            snprintf(msg, sizeof(msg), 
                    "Promoted %s to %s (level %d).\r\n", 
                    target_name, level_name, new_level);
            result.type = VALUE_STRING;
            result.data.string_value = strdup(msg);
        } else {
            result.type = VALUE_STRING;
            result.data.string_value = strdup("Player not found.\r\n");
        }
        return result;
    }
    
    if (strcmp(cmd, "users") == 0) {
        if (session->privilege_level < 2) {
            result.type = VALUE_STRING;
            result.data.string_value = strdup("You don't have permission to use that command.\r\n");
            return result;
        }
        
        char msg[BUFFER_SIZE];
        strcpy(msg, "Connected users:\r\n");
        strcat(msg, "Name            Privilege      Idle\r\n");
        strcat(msg, "----------------------------------------------\r\n");
        
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (sessions[i] && sessions[i]->state == STATE_PLAYING) {
                const char *priv_name = (sessions[i]->privilege_level == 2) ? "Admin" :
                                       (sessions[i]->privilege_level == 1) ? "Wizard" : "Player";
                time_t idle = time(NULL) - sessions[i]->last_activity;
                char line[128];
                snprintf(line, sizeof(line), "%-15s %-14s %ld sec\r\n",
                        sessions[i]->username, priv_name, idle);
                strcat(msg, line);
            }
        }
        
        result.type = VALUE_STRING;
        result.data.string_value = strdup(msg);
        return result;
    }
    
    if (strcmp(cmd, "shutdown") == 0) {
        if (session->privilege_level < 2) {
            result.type = VALUE_STRING;
            result.data.string_value = strdup("You don't have permission to use that command.\r\n");
            return result;
        }
        
        int delay = 0;
        if (args && *args != '\0') {
            delay = atoi(args);
        }
        
        char msg[256];
        snprintf(msg, sizeof(msg), 
                "SYSTEM: Admin %s is shutting down the server%s%d second%s.\r\n",
                session->username,
                delay > 0 ? " in " : "",
                delay,
                delay == 1 ? "" : "s");
        broadcast_message(msg, NULL);
        
        fprintf(stderr, "[Server] Shutdown initiated by %s\n", session->username);
        server_running = 0;
        result.type = VALUE_STRING;
        result.data.string_value = strdup("Server shutdown initiated.\r\n");
        return result;
    }
    
    /* Unknown command */
    char error_msg[512];
    snprintf(error_msg, sizeof(error_msg), 
            "Unknown command: %.200s\r\nType 'help' for available commands.\r\n", cmd);
    result.type = VALUE_STRING;
    result.data.string_value = strdup(error_msg);
    
    return result;
}

/* Broadcast message to all players except one */
void broadcast_message(const char *message, PlayerSession *exclude) {
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (sessions[i] && 
            sessions[i]->state == STATE_PLAYING && 
            sessions[i] != exclude) {
            send_to_player(sessions[i], "%s", message);
        }
    }
}

/* Process input during login states */
void process_login_state(PlayerSession *session, const char *input) {
    char clean_input[256];
    strncpy(clean_input, input, sizeof(clean_input) - 1);
    clean_input[sizeof(clean_input) - 1] = '\0';
    
    /* Strip whitespace */
    char *start = clean_input;
    while (*start && isspace(*start)) start++;
    char *end = start + strlen(start) - 1;
    while (end > start && isspace(*end)) *end-- = '\0';
    
    switch (session->state) {
        case STATE_GET_NAME:
            if (strlen(start) < 3 || strlen(start) > 15) {
                send_to_player(session, 
                    "Name must be between 3 and 15 characters.\r\n");
                send_prompt(session);
                return;
            }
            
            strncpy(session->username, start, sizeof(session->username) - 1);
            
            /* For now, always treat as new user */
            send_to_player(session, 
                "\r\nWelcome, %s! You appear to be new here.\r\n", 
                session->username);
            session->state = STATE_NEW_PASSWORD;
            send_prompt(session);
            break;
            
        case STATE_GET_PASSWORD:
            send_to_player(session, "\r\nWelcome back, %s!\r\n", session->username);
            session->state = STATE_PLAYING;
            
            send_to_player(session, "\r\nYou materialize in the starting room.\r\n");
            send_prompt(session);
            
            /* Announce login */
            char login_msg[256];
            snprintf(login_msg, sizeof(login_msg), 
                    "%s has entered the game.\r\n", session->username);
            broadcast_message(login_msg, session);
            break;
            
        case STATE_NEW_PASSWORD:
            if (strlen(start) < 6) {
                send_to_player(session, 
                    "Password must be at least 6 characters.\r\n");
                send_prompt(session);
                return;
            }
            
            strncpy(session->password_buffer, start, sizeof(session->password_buffer) - 1);
            session->state = STATE_CONFIRM_PASSWORD;
            send_prompt(session);
            break;
            
        case STATE_CONFIRM_PASSWORD:
            if (strcmp(session->password_buffer, start) != 0) {
                send_to_player(session, 
                    "\r\nPasswords don't match. Let's try again.\r\n");
                memset(session->password_buffer, 0, sizeof(session->password_buffer));
                session->state = STATE_NEW_PASSWORD;
                send_prompt(session);
                return;
            }
            
            /* Create player object */
            session->player_object = create_player_object(
                session->username, 
                session->password_buffer);
            
            if (!session->player_object) {
                send_to_player(session, 
                    "\r\nError: Failed to create player object.\r\n");
                session->state = STATE_DISCONNECTING;
                return;
            }
            
            /* First player becomes admin */
            if (!first_player_created) {
                session->privilege_level = 2;  /* Admin */
                first_player_created = 1;
                fprintf(stderr, "[Server] First player created: %s (privilege: Admin)\n",
                       session->username);
                
                send_to_player(session, 
                    "\r\nCharacter created successfully!\r\n"
                    "As the first player, you have been granted Admin privileges.\r\n"
                    "You materialize in the starting room.\r\n");
            } else {
                session->privilege_level = 0;  /* Regular player */
                send_to_player(session, 
                    "\r\nCharacter created successfully!\r\n"
                    "You materialize in the starting room.\r\n");
            }
            
            memset(session->password_buffer, 0, sizeof(session->password_buffer));
            session->state = STATE_PLAYING;
            send_prompt(session);
            
            /* Announce login */
            char create_msg[256];
            snprintf(create_msg, sizeof(create_msg), 
                    "%s has entered the game for the first time!\r\n", 
                    session->username);
            broadcast_message(create_msg, session);
            break;
            
        default:
            break;
    }
}

/* Process input during playing state */
void process_playing_state(PlayerSession *session, const char *input) {
    VMValue result = execute_command(session, input);
    
    if (result.type == VALUE_STRING && result.data.string_value) {
        if (strcmp(result.data.string_value, "quit") == 0) {
            send_to_player(session, "\r\nGoodbye, %s!\r\n", session->username);
            
            char logout_msg[256];
            snprintf(logout_msg, sizeof(logout_msg), 
                    "%s has left the game.\r\n", session->username);
            broadcast_message(logout_msg, session);
            
            session->state = STATE_DISCONNECTING;
        } else {
            send_to_player(session, "%s", result.data.string_value);
            send_prompt(session);
        }
        
        if (result.data.string_value) {
            free(result.data.string_value);
        }
    } else {
        send_to_player(session, "Command failed to execute.\r\n");
        send_prompt(session);
    }
}

/* Handle incoming input for a session */
void handle_session_input(PlayerSession *session, const char *input) {
    if (!session) return;
    
    session->last_activity = time(NULL);
    
    size_t input_len = strlen(input);
    if (session->input_length + input_len >= INPUT_BUFFER_SIZE - 1) {
        send_to_player(session, "\r\nInput too long. Clearing buffer.\r\n");
        session->input_length = 0;
        send_prompt(session);
        return;
    }
    
    memcpy(session->input_buffer + session->input_length, input, input_len);
    session->input_length += input_len;
    session->input_buffer[session->input_length] = '\0';
    
    char *line_start = session->input_buffer;
    char *newline;
    
    while ((newline = strchr(line_start, '\n')) != NULL) {
        *newline = '\0';
        
        char *cr = strchr(line_start, '\r');
        if (cr) *cr = '\0';
        
        if (session->state == STATE_CONNECTING) {
            send_prompt(session);
        } else if (session->state == STATE_PLAYING) {
            if (strlen(line_start) > 0) {
                process_playing_state(session, line_start);
            } else {
                send_prompt(session);
            }
        } else {
            process_login_state(session, line_start);
        }
        
        line_start = newline + 1;
    }
    
    size_t remaining = strlen(line_start);
    if (remaining > 0) {
        memmove(session->input_buffer, line_start, remaining + 1);
        session->input_length = remaining;
    } else {
        session->input_length = 0;
        session->input_buffer[0] = '\0';
    }
}

/* Check for idle session timeouts */
void check_session_timeouts(void) {
    time_t now = time(NULL);
    
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (sessions[i]) {
            time_t idle = now - sessions[i]->last_activity;
            
            if (idle > SESSION_TIMEOUT) {
                send_to_player(sessions[i], 
                    "\r\nYou have been idle too long. Disconnecting...\r\n");
                fprintf(stderr, "[Server] Timeout disconnect: %s (slot %d)\n",
                       sessions[i]->username[0] ? sessions[i]->username : "guest",
                       i);
                sessions[i]->state = STATE_DISCONNECTING;
            }
        }
    }
}

/* Handle WebSocket data */
void handle_websocket_data(PlayerSession *session, const uint8_t *data, size_t len) {
    if (!session || !data) return;
    
    session->last_activity = time(NULL);
    
    /* Append to WebSocket buffer */
    if (session->ws_buffer_length + len >= WS_BUFFER_SIZE) {
        fprintf(stderr, "[Server] WebSocket buffer overflow, clearing\n");
        session->ws_buffer_length = 0;
        return;
    }
    
    memcpy(session->ws_buffer + session->ws_buffer_length, data, len);
    session->ws_buffer_length += len;
    
    /* Handle based on WebSocket state */
    if (session->ws_state == WS_STATE_CONNECTING) {
        /* Check for complete HTTP request (ends with \r\n\r\n) */
        if (memmem(session->ws_buffer, session->ws_buffer_length, "\r\n\r\n", 4) != NULL) {
            /* Null-terminate for string processing */
            session->ws_buffer[session->ws_buffer_length] = '\0';
            
            WSHandshake handshake;
            if (ws_handle_handshake((char*)session->ws_buffer, session->ws_buffer_length, &handshake) == 0) {
                /* Send handshake response */
                send(session->fd, handshake.response, handshake.response_len, 0);
                ws_handshake_free(&handshake);
                
                session->ws_state = WS_STATE_OPEN;
                session->ws_buffer_length = 0;
                
                fprintf(stderr, "[Server] WebSocket handshake complete for slot\n");
                
                /* Send welcome prompt */
                send_prompt(session);
            } else {
                /* Invalid handshake */
                fprintf(stderr, "[Server] WebSocket handshake failed\n");
                session->state = STATE_DISCONNECTING;
            }
        }
        return;
    }
    
    /* Process WebSocket frames */
    while (session->ws_buffer_length > 0 && session->ws_state == WS_STATE_OPEN) {
        WSFrame frame;
        size_t consumed;
        
        int result = ws_decode_frame(session->ws_buffer, session->ws_buffer_length, &frame, &consumed);
        
        if (result > 0) {
            /* Need more data */
            break;
        }
        
        if (result < 0) {
            /* Error */
            fprintf(stderr, "[Server] WebSocket frame decode error\n");
            session->state = STATE_DISCONNECTING;
            break;
        }
        
        /* Handle frame by opcode */
        switch (frame.opcode) {
            case WS_OPCODE_TEXT:
                /* Process as normal input */
                if (frame.payload && frame.payload_len > 0) {
                    /* Add newline if not present (for command processing) */
                    char input[INPUT_BUFFER_SIZE];
                    size_t copy_len = frame.payload_len;
                    if (copy_len >= sizeof(input) - 2) {
                        copy_len = sizeof(input) - 2;
                    }
                    memcpy(input, frame.payload, copy_len);
                    input[copy_len] = '\n';
                    input[copy_len + 1] = '\0';
                    
                    handle_session_input(session, input);
                }
                break;
                
            case WS_OPCODE_BINARY:
                /* Ignore binary frames for now */
                break;
                
            case WS_OPCODE_CLOSE:
                /* Client initiated close */
                fprintf(stderr, "[Server] WebSocket close received\n");
                {
                    size_t close_len;
                    uint8_t *close_frame = ws_encode_close(WS_CLOSE_NORMAL, "Goodbye", &close_len);
                    if (close_frame) {
                        send(session->fd, close_frame, close_len, 0);
                        free(close_frame);
                    }
                }
                session->ws_state = WS_STATE_CLOSED;
                session->state = STATE_DISCONNECTING;
                break;
                
            case WS_OPCODE_PING:
                /* Respond with pong */
                {
                    size_t pong_len;
                    uint8_t *pong_frame = ws_encode_pong(frame.payload, frame.payload_len, &pong_len);
                    if (pong_frame) {
                        send(session->fd, pong_frame, pong_len, 0);
                        free(pong_frame);
                    }
                }
                break;
                
            case WS_OPCODE_PONG:
                /* Received pong, ignore */
                break;
        }
        
        ws_frame_free(&frame);
        
        /* Remove consumed bytes from buffer */
        if (consumed < session->ws_buffer_length) {
            memmove(session->ws_buffer, session->ws_buffer + consumed, 
                    session->ws_buffer_length - consumed);
            session->ws_buffer_length -= consumed;
        } else {
            session->ws_buffer_length = 0;
        }
    }
}

/* Setup WebSocket listener socket */
int setup_ws_listener(int port) {
    int ws_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (ws_fd < 0) {
        fprintf(stderr, "[Server] ERROR: WebSocket socket() failed: %s\n", strerror(errno));
        return -1;
    }
    
    int opt = 1;
    setsockopt(ws_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    
    struct sockaddr_in ws_addr;
    memset(&ws_addr, 0, sizeof(ws_addr));
    ws_addr.sin_family = AF_INET;
    ws_addr.sin_addr.s_addr = INADDR_ANY;
    ws_addr.sin_port = htons(port);
    
    if (bind(ws_fd, (struct sockaddr*)&ws_addr, sizeof(ws_addr)) < 0) {
        fprintf(stderr, "[Server] ERROR: WebSocket bind() failed: %s\n", strerror(errno));
        close(ws_fd);
        return -1;
    }
    
    if (listen(ws_fd, 10) < 0) {
        fprintf(stderr, "[Server] ERROR: WebSocket listen() failed: %s\n", strerror(errno));
        close(ws_fd);
        return -1;
    }
    
    return ws_fd;
}

/* Main server */
int main(int argc, char **argv) {
    int port = DEFAULT_PORT;
    int ws_port = DEFAULT_WS_PORT;
    const char *master_path = DEFAULT_MASTER_PATH;
    
    if (argc >= 2) {
        port = atoi(argv[1]);
        if (port <= 0 || port > 65535) {
            fprintf(stderr, "Usage: %s [telnet_port] [ws_port] [master_path]\n", argv[0]);
            return 1;
        }
    }
    
    if (argc >= 3) {
        ws_port = atoi(argv[2]);
        if (ws_port <= 0 || ws_port > 65535) {
            fprintf(stderr, "Usage: %s [telnet_port] [ws_port] [master_path]\n", argv[0]);
            return 1;
        }
    }
    
    if (argc >= 4) {
        master_path = argv[3];
    }
    
    signal(SIGINT, handle_shutdown_signal);
    signal(SIGTERM, handle_shutdown_signal);
    signal(SIGPIPE, SIG_IGN);
    
    if (initialize_vm(master_path) != 0) {
        return 1;
    }
    
    for (int i = 0; i < MAX_CLIENTS; i++) {
        sessions[i] = NULL;
    }
    
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        fprintf(stderr, "[Server] ERROR: socket() failed: %s\n", strerror(errno));
        cleanup_vm();
        return 1;
    }
    
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);
    
    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        fprintf(stderr, "[Server] ERROR: bind() failed: %s\n", strerror(errno));
        close(server_fd);
        cleanup_vm();
        return 1;
    }
    
    if (listen(server_fd, 10) < 0) {
        fprintf(stderr, "[Server] ERROR: listen() failed: %s\n", strerror(errno));
        close(server_fd);
        cleanup_vm();
        return 1;
    }
    
    /* Setup WebSocket listener */
    int ws_fd = setup_ws_listener(ws_port);
    if (ws_fd < 0) {
        fprintf(stderr, "[Server] WARNING: WebSocket listener failed, continuing with telnet only\n");
    }
    
    fprintf(stderr, "[Server] Telnet listening on port %d\n", port);
    if (ws_fd > 0) {
        fprintf(stderr, "[Server] WebSocket listening on port %d\n", ws_port);
    }
    fprintf(stderr, "[Server] Ready for connections\n\n");
    
    time_t last_timeout_check = time(NULL);
    
    while (server_running) {
        fd_set read_fds;
        FD_ZERO(&read_fds);
        FD_SET(server_fd, &read_fds);
        
        int max_fd = server_fd;
        
        /* Add WebSocket listener to select set */
        if (ws_fd > 0) {
            FD_SET(ws_fd, &read_fds);
            if (ws_fd > max_fd) {
                max_fd = ws_fd;
            }
        }
        
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (sessions[i] && sessions[i]->fd > 0) {
                FD_SET(sessions[i]->fd, &read_fds);
                if (sessions[i]->fd > max_fd) {
                    max_fd = sessions[i]->fd;
                }
            }
        }
        
        struct timeval timeout;
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;
        
        int activity = select(max_fd + 1, &read_fds, NULL, NULL, &timeout);
        
        if (activity < 0 && errno != EINTR) {
            fprintf(stderr, "[Server] ERROR: select() failed: %s\n", strerror(errno));
            break;
        }
        
        if (activity < 0) continue;
        
        /* Handle telnet connections */
        if (FD_ISSET(server_fd, &read_fds)) {
            struct sockaddr_in client_addr;
            socklen_t addr_len = sizeof(client_addr);
            int new_fd = accept(server_fd, (struct sockaddr*)&client_addr, &addr_len);
            
            if (new_fd >= 0) {
                int slot = -1;
                for (int i = 0; i < MAX_CLIENTS; i++) {
                    if (!sessions[i]) {
                        slot = i;
                        break;
                    }
                }
                
                if (slot >= 0) {
                    sessions[slot] = malloc(sizeof(PlayerSession));
                    init_session(sessions[slot], new_fd, inet_ntoa(client_addr.sin_addr), CONN_TELNET);
                    fprintf(stderr, "[Server] Telnet connection slot %d from %s\n", 
                           slot, sessions[slot]->ip_address);
                    send_prompt(sessions[slot]);
                } else {
                    const char *msg = "Server full.\r\n";
                    send(new_fd, msg, strlen(msg), 0);
                    close(new_fd);
                }
            }
        }
        
        /* Handle WebSocket connections */
        if (ws_fd > 0 && FD_ISSET(ws_fd, &read_fds)) {
            struct sockaddr_in client_addr;
            socklen_t addr_len = sizeof(client_addr);
            int new_fd = accept(ws_fd, (struct sockaddr*)&client_addr, &addr_len);
            
            if (new_fd >= 0) {
                int slot = -1;
                for (int i = 0; i < MAX_CLIENTS; i++) {
                    if (!sessions[i]) {
                        slot = i;
                        break;
                    }
                }
                
                if (slot >= 0) {
                    sessions[slot] = malloc(sizeof(PlayerSession));
                    init_session(sessions[slot], new_fd, inet_ntoa(client_addr.sin_addr), CONN_WEBSOCKET);
                    fprintf(stderr, "[Server] WebSocket connection slot %d from %s\n", 
                           slot, sessions[slot]->ip_address);
                    /* Don't send prompt yet - wait for handshake */
                } else {
                    const char *msg = "HTTP/1.1 503 Service Unavailable\r\n\r\n";
                    send(new_fd, msg, strlen(msg), 0);
                    close(new_fd);
                }
            }
        }
        
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (!sessions[i] || sessions[i]->fd <= 0) continue;
            
            if (sessions[i]->state == STATE_DISCONNECTING) {
                fprintf(stderr, "[Server] Closing slot %d\n", i);
                free_session(sessions[i]);
                sessions[i] = NULL;
                continue;
            }
            
            if (FD_ISSET(sessions[i]->fd, &read_fds)) {
                char buffer[BUFFER_SIZE];
                ssize_t bytes = recv(sessions[i]->fd, buffer, sizeof(buffer) - 1, 0);
                
                if (bytes <= 0) {
                    fprintf(stderr, "[Server] Disconnect slot %d\n", i);
                    free_session(sessions[i]);
                    sessions[i] = NULL;
                } else {
                    if (sessions[i]->connection_type == CONN_WEBSOCKET) {
                        /* Handle as WebSocket data */
                        handle_websocket_data(sessions[i], (uint8_t*)buffer, bytes);
                    } else {
                        /* Handle as telnet data */
                        buffer[bytes] = '\0';
                        handle_session_input(sessions[i], buffer);
                    }
                }
            }
        }
        
        time_t now = time(NULL);
        if (now - last_timeout_check > 60) {
            check_session_timeouts();
            last_timeout_check = now;
        }
    }
    
    fprintf(stderr, "\n[Server] Shutting down...\n");
    
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (sessions[i]) {
            send_to_player(sessions[i], "\r\nServer shutting down...\r\n");
            free_session(sessions[i]);
        }
    }
    
    close(server_fd);
    if (ws_fd > 0) {
        close(ws_fd);
    }
    cleanup_vm();
    
    fprintf(stderr, "[Server] Shutdown complete\n");
    return 0;
}
