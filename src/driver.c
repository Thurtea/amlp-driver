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
 * Default master: lib/secure/master.lpc
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
#include "efun.h"
#include "skills.h"
#include "combat.h"
#include "item.h"
#include "psionics.h"
#include "magic.h"
#include "websocket.h"
#include "session.h"
#include "object.h"
#include "room.h"
#include "chargen.h"

#define MAX_CLIENTS 100
#define BUFFER_SIZE 4096
#define INPUT_BUFFER_SIZE 2048
#define WS_BUFFER_SIZE 65536
#define DEFAULT_PORT 3000
#define DEFAULT_WS_PORT 3001
#define DEFAULT_MASTER_PATH "lib/secure/master.lpc"
#define SESSION_TIMEOUT 1800  /* 30 minutes */

/* Connection types and session state are defined in session_internal.h */

/* Player session data (shared) */
#include "session_internal.h"

/* Global state */
static volatile sig_atomic_t server_running = 1;
static VirtualMachine *global_vm = NULL;
static PlayerSession *sessions[MAX_CLIENTS];
static int first_player_created = 0;  /* Track if first player has logged in */

/* session management functions are implemented in src/session.c */

/* Function prototypes */
void handle_shutdown_signal(int sig);
int initialize_vm(const char *master_path);
void cleanup_vm(void);
int test_parse_file(const char *filename);  /* ADD THIS LINE */
void init_session(PlayerSession *session, int fd, const char *ip, ConnectionType conn_type);
void free_session(PlayerSession *session);
void handle_session_input(PlayerSession *session, const char *input);
void handle_websocket_data(PlayerSession *session, const uint8_t *data, size_t len);
void process_login_state(PlayerSession *session, const char *input);
void process_chargen_state(PlayerSession *session, const char *input);
void process_playing_state(PlayerSession *session, const char *input);
void send_to_player(PlayerSession *session, const char *format, ...);
void send_prompt(PlayerSession *session);
VMValue execute_command(PlayerSession *session, const char *command);
void broadcast_message(const char *message, PlayerSession *exclude);
void check_session_timeouts(void);
void* create_player_object(const char *username, const char *password_hash);
VMValue call_player_command(void *player_obj, const char *command);
int setup_ws_listener(int port);

/* Filesystem command functions (implemented in server.c) */
int cmd_ls_filesystem(PlayerSession *session, const char *args);
int cmd_cd_filesystem(PlayerSession *session, const char *args);
int cmd_pwd_filesystem(PlayerSession *session);
int cmd_cat_filesystem(PlayerSession *session, const char *args);

/* Priority gameplay command functions */
static int cmd_tell(PlayerSession *session, const char *arg);
static int cmd_chat(PlayerSession *session, const char *arg);
static int cmd_whisper(PlayerSession *session, const char *arg);
static int cmd_shout(PlayerSession *session, const char *arg);
static int cmd_exits(PlayerSession *session, const char *arg);
static int cmd_examine(PlayerSession *session, const char *arg);
static int cmd_give_item(PlayerSession *session, const char *arg);

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
    
    /* TEMPORARY: Bypass LPC object system until parser is fixed
     * Return a non-NULL pointer as a placeholder to allow character creation testing */
    fprintf(stderr, "[Server] BYPASS MODE: Creating stub player object (LPC parser disabled)\n");
    fprintf(stderr, "[Server] Player: %s will use C-level session only\n", username ? username : "unknown");
    
    /* Return a placeholder pointer (just needs to be non-NULL for now) */
    return (void *)0x1;
}

/* Call player object's process_command method */
VMValue call_player_command(void *player_obj, const char *command) {
    VMValue result;
    result.type = VALUE_NULL;

    if (!player_obj || !global_vm || !command) {
        return result;
    }

    fprintf(stderr, "[Server] Calling player command: %s\n", command);

    // Cast player_obj to obj_t*
    /* Guard: during early development a non-pointer sentinel (1) is
     * returned from create_player_object(). Do not attempt to treat
     * that sentinel as a real object pointer. Return a null result so
     * execute_command will fall back to built-in handlers. */
    if (player_obj == (void*)1) {
        return result;
    }

    obj_t *obj = (obj_t *)player_obj;

    // Prepare argument (command string)
    VMValue cmd_arg = vm_value_create_string(command);

    /* Debug: check whether object exposes process_command */
    VMFunction *m = obj_get_method(obj, "process_command");
    if (!m) {
        fprintf(stderr, "[Server] DEBUG: player object '%s' has no process_command()\n",
                obj->name ? obj->name : "<unnamed>");
    } else {
        fprintf(stderr, "[Server] DEBUG: player object '%s' has process_command (%d params)\n",
                obj->name ? obj->name : "<unnamed>", m->param_count);
    }

    // Call process_command on the player object
    result = obj_call_method(global_vm, obj, "process_command", &cmd_arg, 1);

    /* Debug: log return type */
    if (result.type == VALUE_STRING) {
        fprintf(stderr, "[Server] DEBUG: process_command returned string: %s\n",
                result.data.string_value ? result.data.string_value : "(null)");
    } else if (result.type == VALUE_INT) {
        fprintf(stderr, "[Server] DEBUG: process_command returned int: %ld\n",
                result.data.int_value);
    } else {
        fprintf(stderr, "[Server] DEBUG: process_command returned type %d\n", result.type);
    }

    // Clean up
    vm_value_free(&cmd_arg);

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
        /* Persist a minimal savefile on disconnect as a temporary measure
         * until the VM-backed LPC objects are fully implemented. This
         * creates lib/save/players/<name>.o with basic info so the mudlib
         * can detect a savefile exists. */
        if (session->username && session->username[0]) {
            char path[512];
            snprintf(path, sizeof(path), "lib/save/players/%s.o", session->username);
            FILE *f = fopen(path, "w");
            if (f) {
                fprintf(f, "# AMLP minimal save file\n");
                fprintf(f, "name:%s\n", session->username);
                fprintf(f, "priv:%d\n", session->privilege_level);
                fclose(f);
                fprintf(stderr, "[Server] Wrote minimal savefile: %s\n", path);
            } else {
                fprintf(stderr, "[Server] WARNING: failed to write savefile %s\n", path);
            }
        }

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
                "=========================================\r\n"
                "    AMLP Driver - Development Server\r\n"
                "             Version 0.1.0\r\n"
                "=========================================\r\n"
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

/* ========================================================================
 * PRIORITY GAMEPLAY COMMANDS
 * ======================================================================== */

/* Helper: Find a player session by name */
static PlayerSession* find_player_by_name(const char *name) {
    if (!name || !*name) return NULL;
    
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (sessions[i] && 
            sessions[i]->state == STATE_PLAYING && 
            sessions[i]->username && 
            strcasecmp(sessions[i]->username, name) == 0) {
            return sessions[i];
        }
    }
    return NULL;
}

/* 1. TELL - Send private message to another player */
static int cmd_tell(PlayerSession *session, const char *arg) {
    char target_name[64];
    char message[1024];
    
    if (!arg || !*arg) {
        send_to_player(session, "Usage: tell <player> <message>\n");
        return 1;
    }
    
    /* Parse: "playername message text" */
    if (sscanf(arg, "%63s %1023[^\n]", target_name, message) != 2) {
        send_to_player(session, "Usage: tell <player> <message>\n");
        return 1;
    }
    
    /* Find target player */
    PlayerSession *target = find_player_by_name(target_name);
    if (!target) {
        send_to_player(session, "Player '%s' not found.\n", target_name);
        return 1;
    }
    
    if (target == session) {
        send_to_player(session, "Talking to yourself again?\n");
        return 1;
    }
    
    /* Send messages */
    send_to_player(target, "\n<%s tells you> %s\n", session->username, message);
    send_to_player(session, "<You tell %s> %s\n", target->username, message);
    
    return 1;
}

/* 2. CHAT - Global chat channel */
static int cmd_chat(PlayerSession *session, const char *arg) {
    if (!arg || !*arg) {
        send_to_player(session, "Usage: chat <message>\n");
        return 1;
    }
    
    /* Broadcast to all players */
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (sessions[i] && sessions[i]->state == STATE_PLAYING) {
            if (sessions[i] == session) {
                send_to_player(sessions[i], "[CHAT: %s] %s\n", session->username, arg);
            } else {
                send_to_player(sessions[i], "\n[CHAT: %s] %s\n", session->username, arg);
            }
        }
    }
    
    return 1;
}

/* 3. WHISPER - Private message to someone in the same room */
static int cmd_whisper(PlayerSession *session, const char *arg) {
    char target_name[64];
    char message[1024];
    
    if (!arg || !*arg) {
        send_to_player(session, "Usage: whisper <player> <message>\n");
        return 1;
    }
    
    if (!session->current_room) {
        send_to_player(session, "You are nowhere.\n");
        return 1;
    }
    
    /* Parse: "playername message text" */
    if (sscanf(arg, "%63s %1023[^\n]", target_name, message) != 2) {
        send_to_player(session, "Usage: whisper <player> <message>\n");
        return 1;
    }
    
    /* Find player in current room */
    PlayerSession *target = NULL;
    Room *room = session->current_room;
    
    for (int i = 0; i < room->num_players; i++) {
        if (room->players[i] && room->players[i] != session &&
            room->players[i]->username &&
            strcasecmp(room->players[i]->username, target_name) == 0) {
            target = room->players[i];
            break;
        }
    }
    
    if (!target) {
        send_to_player(session, "Player '%s' not found in this room.\n", target_name);
        return 1;
    }
    
    /* Send whisper */
    send_to_player(target, "%s whispers to you: %s\n", session->username, message);
    send_to_player(session, "You whisper to %s: %s\n", target->username, message);
    
    /* Let others know something was whispered (but not what) */
    for (int i = 0; i < room->num_players; i++) {
        if (room->players[i] && room->players[i] != session && room->players[i] != target) {
            send_to_player(room->players[i], "%s whispers something to %s.\n",
                          session->username, target->username);
        }
    }
    
    return 1;
}

/* 4. SHOUT - Loud message heard in area */
static int cmd_shout(PlayerSession *session, const char *arg) {
    if (!arg || !*arg) {
        send_to_player(session, "Usage: shout <message>\n");
        return 1;
    }
    
    if (!session->current_room) {
        send_to_player(session, "You are nowhere.\n");
        return 1;
    }
    
    Room *room = session->current_room;
    
    /* Shout in current room */
    for (int i = 0; i < room->num_players; i++) {
        if (room->players[i] && room->players[i] != session) {
            send_to_player(room->players[i], "%s shouts: %s\n", session->username, arg);
        }
    }
    send_to_player(session, "You shout: %s\n", arg);
    
    /* TODO: Shout to adjacent rooms (requires room linkage) */
    
    return 1;
}

/* 5. EXITS - Show available exits from current room */
static int cmd_exits(PlayerSession *session, const char *arg) {
    (void)arg;  /* Unused */
    
    if (!session->current_room) {
        send_to_player(session, "You are nowhere.\n");
        return 1;
    }
    
    Room *room = session->current_room;
    send_to_player(session, "\nObvious exits:\n");
    
    int has_exit = 0;
    if (room->exits.north >= 0) {
        send_to_player(session, "  North\n");
        has_exit = 1;
    }
    if (room->exits.south >= 0) {
        send_to_player(session, "  South\n");
        has_exit = 1;
    }
    if (room->exits.east >= 0) {
        send_to_player(session, "  East\n");
        has_exit = 1;
    }
    if (room->exits.west >= 0) {
        send_to_player(session, "  West\n");
        has_exit = 1;
    }
    if (room->exits.up >= 0) {
        send_to_player(session, "  Up\n");
        has_exit = 1;
    }
    if (room->exits.down >= 0) {
        send_to_player(session, "  Down\n");
        has_exit = 1;
    }
    
    if (!has_exit) {
        send_to_player(session, "  None.\n");
    }
    send_to_player(session, "\n");
    
    return 1;
}

/* 6. EXAMINE - Inspect objects/players in detail */
static int cmd_examine(PlayerSession *session, const char *arg) {
    if (!arg || !*arg) {
        send_to_player(session, "Examine what?\n");
        return 1;
    }
    
    if (!session->current_room) {
        send_to_player(session, "You are nowhere.\n");
        return 1;
    }
    
    /* Check for "room" or "here" */
    if (strcasecmp(arg, "room") == 0 || strcasecmp(arg, "here") == 0) {
        cmd_look(session, "");
        return 1;
    }
    
    /* Look for player in room */
    Room *room = session->current_room;
    for (int i = 0; i < room->num_players; i++) {
        if (room->players[i] && room->players[i]->username &&
            strcasecmp(room->players[i]->username, arg) == 0) {
            
            PlayerSession *target = room->players[i];
            if (target == session) {
                send_to_player(session, "That's you!\n");
            } else {
                send_to_player(session, "You examine %s.\n", target->username);
                send_to_player(session, "Race: %s  O.C.C.: %s\n",
                              target->character.race,
                              target->character.occ);
                send_to_player(session, "They appear to be in good health.\n");
            }
            return 1;
        }
    }
    
    /* TODO: Check for items in room/inventory */
    send_to_player(session, "You don't see that here.\n");
    
    return 1;
}

/* 10. GIVE - Give items to other players */
static int cmd_give_item(PlayerSession *session, const char *arg) {
    if (!arg || !*arg) {
        send_to_player(session, "Usage: give <item> <player>\n");
        return 1;
    }
    
    if (!session->current_room) {
        send_to_player(session, "You are nowhere.\n");
        return 1;
    }
    
    /* TODO: Implement when item system is ready */
    send_to_player(session, "You don't have that item.\n");
    
    return 1;
}

/* Execute command through VM */
VMValue execute_command(PlayerSession *session, const char *command) {
    VMValue result;
    result.type = VALUE_NULL;
    
    if (!global_vm || !session) {
        return result;
    }
    
    /* Parse command early for filesystem checks */
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
    
    /* Filesystem commands for wizards/admins - CHECK FIRST BEFORE PLAYER OBJECT */
    if (session->privilege_level >= 1) {
        if (strcmp(cmd, "ls") == 0 || strcmp(cmd, "dir") == 0) {
            cmd_ls_filesystem(session, args);
            result.type = VALUE_STRING;
            result.data.string_value = strdup("");
            return result;
        }
        
        if (strcmp(cmd, "cd") == 0) {
            cmd_cd_filesystem(session, args);
            result.type = VALUE_STRING;
            result.data.string_value = strdup("");
            return result;
        }
        
        if (strcmp(cmd, "pwd") == 0) {
            cmd_pwd_filesystem(session);
            result.type = VALUE_STRING;
            result.data.string_value = strdup("");
            return result;
        }
        
        if (strcmp(cmd, "cat") == 0 || strcmp(cmd, "more") == 0) {
            cmd_cat_filesystem(session, args);
            result.type = VALUE_STRING;
            result.data.string_value = strdup("");
            return result;
        }
    }
    
    /* If player object exists, route command through it. Set the
     * current VM session so efuns like this_player() can access it. */
    if (session->player_object) {
        set_current_session(session);
        result = call_player_command(session->player_object, command);
        set_current_session(NULL);

        /* If VM returns valid result, use it */
        if (result.type == VALUE_STRING && result.data.string_value) {
            return result;
        }
    }
    
    /* Fallback to built-in commands if no player object or no result */
    
    /* Built-in commands */
    
    /* Movement commands */
    if (strcmp(cmd, "north") == 0 || strcmp(cmd, "n") == 0) {
        cmd_move(session, "north");
        result.type = VALUE_NULL;
        return result;
    }
    if (strcmp(cmd, "south") == 0 || strcmp(cmd, "s") == 0) {
        cmd_move(session, "south");
        result.type = VALUE_NULL;
        return result;
    }
    if (strcmp(cmd, "east") == 0 || strcmp(cmd, "e") == 0) {
        cmd_move(session, "east");
        result.type = VALUE_NULL;
        return result;
    }
    if (strcmp(cmd, "west") == 0 || strcmp(cmd, "w") == 0) {
        cmd_move(session, "west");
        result.type = VALUE_NULL;
        return result;
    }
    if (strcmp(cmd, "up") == 0 || strcmp(cmd, "u") == 0) {
        cmd_move(session, "up");
        result.type = VALUE_NULL;
        return result;
    }
    if (strcmp(cmd, "down") == 0 || strcmp(cmd, "d") == 0) {
        cmd_move(session, "down");
        result.type = VALUE_NULL;
        return result;
    }
    
    if (strcmp(cmd, "quit") == 0 || strcmp(cmd, "logout") == 0) {
        /* Auto-save before quitting */
        send_to_player(session, "\r\nSaving your character...\r\n");
        if (save_character(session)) {
            send_to_player(session, " Character saved.\r\n");
        } else {
            send_to_player(session, " Warning: Failed to save character.\r\n");
        }
        
        result.type = VALUE_STRING;
        result.data.string_value = strdup("quit");
        return result;
    }
    
    if (strcmp(cmd, "save") == 0) {
        if (save_character(session)) {
            send_to_player(session, " Character saved successfully.\r\n");
        } else {
            send_to_player(session, " Failed to save character.\r\n");
        }
        result.type = VALUE_NULL;
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
            "  save                 - Save your character\r\n"
            "  quit / logout        - Save and disconnect\r\n"
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
        cmd_look(session, args ? args : "");
        result.type = VALUE_NULL;
        return result;
    }
    
    if (strcmp(cmd, "stats") == 0 || strcmp(cmd, "score") == 0) {
        cmd_stats(session, args ? args : "");
        result.type = VALUE_NULL;
        return result;
    }
    
    if (strcmp(cmd, "skills") == 0) {
        cmd_skills(session, args ? args : "");
        result.type = VALUE_NULL;
        return result;
    }
    
    if (strcmp(cmd, "attack") == 0) {
        cmd_attack(session, args ? args : "");
        result.type = VALUE_NULL;
        return result;
    }
    
    if (strcmp(cmd, "strike") == 0) {
        cmd_strike(session, args ? args : "");
        result.type = VALUE_NULL;
        return result;
    }
    
    if (strcmp(cmd, "shoot") == 0) {
        cmd_shoot(session, args ? args : "");
        result.type = VALUE_NULL;
        return result;
    }
    
    if (strcmp(cmd, "dodge") == 0) {
        cmd_dodge(session, args ? args : "");
        result.type = VALUE_NULL;
        return result;
    }
    
    if (strcmp(cmd, "flee") == 0) {
        cmd_flee(session, args ? args : "");
        result.type = VALUE_NULL;
        return result;
    }
    
    if (strcmp(cmd, "inventory") == 0 || strcmp(cmd, "i") == 0) {
        cmd_inventory(session, args ? args : "");
        result.type = VALUE_NULL;
        return result;
    }
    
    if (strcmp(cmd, "equip") == 0 || strcmp(cmd, "eq") == 0 || strcmp(cmd, "wield") == 0 || strcmp(cmd, "wear") == 0) {
        cmd_equip(session, args ? args : "");
        result.type = VALUE_NULL;
        return result;
    }
    
    if (strcmp(cmd, "unequip") == 0 || strcmp(cmd, "uneq") == 0 || strcmp(cmd, "remove") == 0) {
        cmd_unequip(session, args ? args : "");
        result.type = VALUE_NULL;
        return result;
    }
    
    if (strcmp(cmd, "worn") == 0 || strcmp(cmd, "equipment") == 0 || strcmp(cmd, "eq") == 0) {
        cmd_worn(session, args ? args : "");
        result.type = VALUE_NULL;
        return result;
    }
    
    if (strcmp(cmd, "get") == 0 || strcmp(cmd, "take") == 0) {
        cmd_get(session, args ? args : "");
        result.type = VALUE_NULL;
        return result;
    }
    
    if (strcmp(cmd, "drop") == 0) {
        cmd_drop(session, args ? args : "");
        result.type = VALUE_NULL;
        return result;
    }
    
    /* Psionics commands (Phase 5) */
    if (strcmp(cmd, "use") == 0) {
        cmd_use_power(session, args ? args : "");
        result.type = VALUE_NULL;
        return result;
    }
    
    if (strcmp(cmd, "powers") == 0 || strcmp(cmd, "abilities") == 0) {
        cmd_powers(session, args ? args : "");
        result.type = VALUE_NULL;
        return result;
    }
    
    if (strcmp(cmd, "isp") == 0 || strcmp(cmd, "inner_strength") == 0) {
        cmd_isp(session, args ? args : "");
        result.type = VALUE_NULL;
        return result;
    }
    
    /* Magic commands (Phase 5) */
    if (strcmp(cmd, "cast") == 0) {
        cmd_cast(session, args ? args : "");
        result.type = VALUE_NULL;
        return result;
    }
    
    if (strcmp(cmd, "spells") == 0 || strcmp(cmd, "grimoire") == 0) {
        cmd_spells(session, args ? args : "");
        result.type = VALUE_NULL;
        return result;
    }
    
    if (strcmp(cmd, "ppe") == 0 || strcmp(cmd, "ppp") == 0) {
        cmd_ppe(session, args ? args : "");
        result.type = VALUE_NULL;
        return result;
    }
    
    if (strcmp(cmd, "meditate") == 0) {
        cmd_meditate(session, args ? args : "");
        result.type = VALUE_NULL;
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
    
    /* Priority gameplay commands */
    if (strcmp(cmd, "tell") == 0) {
        cmd_tell(session, args ? args : "");
        result.type = VALUE_NULL;
        return result;
    }
    
    if (strcmp(cmd, "chat") == 0) {
        cmd_chat(session, args ? args : "");
        result.type = VALUE_NULL;
        return result;
    }
    
    if (strcmp(cmd, "whisper") == 0) {
        cmd_whisper(session, args ? args : "");
        result.type = VALUE_NULL;
        return result;
    }
    
    if (strcmp(cmd, "shout") == 0) {
        cmd_shout(session, args ? args : "");
        result.type = VALUE_NULL;
        return result;
    }
    
    if (strcmp(cmd, "exits") == 0) {
        cmd_exits(session, args ? args : "");
        result.type = VALUE_NULL;
        return result;
    }
    
    if (strcmp(cmd, "examine") == 0 || strcmp(cmd, "exam") == 0) {
        cmd_examine(session, args ? args : "");
        result.type = VALUE_NULL;
        return result;
    }
    
    if (strcmp(cmd, "give") == 0) {
        cmd_give_item(session, args ? args : "");
        result.type = VALUE_NULL;
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
    
    /* Wizard commands */
    if (strcmp(cmd, "goto") == 0) {
        if (session->privilege_level < 1) {
            result.type = VALUE_STRING;
            result.data.string_value = strdup("You don't have permission to use that command.\r\n");
            return result;
        }
        
        if (!args || *args == '\0') {
            result.type = VALUE_STRING;
            result.data.string_value = strdup(
                "Usage: goto <room_id>\r\n"
                "Available rooms: 0=Void, 1=Chi-Town Plaza, 2=Coalition HQ, 3=Merchant District\r\n");
            return result;
        }
        
        int room_id = atoi(args);
        Room *target_room = room_get_by_id(room_id);
        
        if (!target_room) {
            result.type = VALUE_STRING;
            result.data.string_value = strdup("Invalid room ID.\r\n");
            return result;
        }
        
        /* Remove from current room */
        if (session->current_room) {
            room_remove_player(session->current_room, session);
            
            char leave_msg[256];
            snprintf(leave_msg, sizeof(leave_msg), 
                    "%s vanishes in a puff of smoke.\r\n", session->username);
            room_broadcast(session->current_room, leave_msg, NULL);
        }
        
        /* Add to target room */
        session->current_room = target_room;
        room_add_player(target_room, session);
        
        char arrive_msg[256];
        snprintf(arrive_msg, sizeof(arrive_msg), 
                "%s appears in a puff of smoke.\r\n", session->username);
        room_broadcast(target_room, arrive_msg, session);
        
        /* Show new room */
        cmd_look(session, "");
        
        result.type = VALUE_NULL;
        return result;
    }
    
    if (strcmp(cmd, "clone") == 0) {
        if (session->privilege_level < 1) {
            result.type = VALUE_STRING;
            result.data.string_value = strdup("You don't have permission to use that command.\r\n");
            return result;
        }
        
        if (!args || *args == '\0') {
            result.type = VALUE_STRING;
            result.data.string_value = strdup(
                "Usage: clone <object>\r\n"
                "Available objects: sword, shield, potion\r\n");
            return result;
        }
        
        char msg[512];
        if (strcmp(args, "sword") == 0) {
            snprintf(msg, sizeof(msg), 
                    "You conjure a gleaming sword from thin air!\r\n"
                    "The sword materializes in your hands.\r\n");
        } else if (strcmp(args, "shield") == 0) {
            snprintf(msg, sizeof(msg), 
                    "You conjure a sturdy shield from thin air!\r\n"
                    "The shield materializes on your arm.\r\n");
        } else if (strcmp(args, "potion") == 0) {
            snprintf(msg, sizeof(msg), 
                    "You conjure a health potion from thin air!\r\n"
                    "The potion appears in a small glass vial.\r\n");
        } else {
            snprintf(msg, sizeof(msg), 
                    "Unknown object: %s\r\n"
                    "Available objects: sword, shield, potion\r\n", args);
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
            
            /* Check if character exists */
            if (character_exists(session->username)) {
                send_to_player(session, 
                    "\r\nWelcome back, %s!\r\n", 
                    session->username);
                session->state = STATE_GET_PASSWORD;
            } else {
                send_to_player(session, 
                    "\r\nWelcome, %s! You appear to be new here.\r\n", 
                    session->username);
                session->state = STATE_NEW_PASSWORD;
            }
            send_prompt(session);
            break;
            
        case STATE_GET_PASSWORD:
            /* TODO: Verify password hash */
            
            /* Load existing character */
            if (load_character(session, session->username)) {
                send_to_player(session, "\r\nWelcome back!\r\n");
                send_to_player(session, "Your character has been restored.\r\n\r\n");
                
                session->state = STATE_PLAYING;
                
                /* Add player to their saved room */
                if (session->current_room) {
                    room_add_player(session->current_room, session);
                } else {
                    /* Fallback to start room if load failed */
                    Room *start = room_get_start();
                    if (start) {
                        session->current_room = start;
                        room_add_player(start, session);
                    }
                }
                
                /* Show room description */
                cmd_look(session, "");
                send_prompt(session);
                
                /* Announce login */
                char login_msg[256];
                snprintf(login_msg, sizeof(login_msg), 
                        "%s has entered the game.\r\n", session->username);
                broadcast_message(login_msg, session);
            } else {
                send_to_player(session, 
                    "\r\nError loading character. Please contact an administrator.\r\n");
                session->state = STATE_DISCONNECTING;
            }
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
            } else {
                session->privilege_level = 0;  /* Regular player */
            }
            
            /* Start character generation */
            memset(session->password_buffer, 0, sizeof(session->password_buffer));
            session->state = STATE_CHARGEN;
            chargen_init(session);
            break;
            
        default:
            break;
    }
}

/* Process input during character generation */
void process_chargen_state(PlayerSession *session, const char *input) {
    chargen_process_input(session, input);
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
    } else if (result.type == VALUE_NULL) {
        /* Command handled its own output, just send prompt */
        send_prompt(session);
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
        } else if (session->state == STATE_CHARGEN) {
            if (strlen(line_start) > 0) {
                process_chargen_state(session, line_start);
            }
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

/* Test mode: parse a single file and report results */
int test_parse_file(const char *filename) {
    fprintf(stderr, "[Parser Test] File: %s\n", filename);
    fprintf(stderr, "[Parser Test] Initializing compiler...\n");
    
    /* Initialize VM (needed for compiler) */
    global_vm = vm_init();
    if (!global_vm) {
        fprintf(stderr, "[Parser Test] FAILED: Could not initialize VM\n");
        return 1;
    }
    
    /* Attempt to compile the file */
    fprintf(stderr, "[Parser Test] Compiling...\n");
    Program *prog = compiler_compile_file(filename);
    
    if (!prog) {
        fprintf(stderr, "[Parser Test] FAILED: Compilation failed\n");
        vm_free(global_vm);
        return 1;
    }
    
    fprintf(stderr, "[Parser Test] SUCCESS: File parsed successfully\n");
    fprintf(stderr, "[Parser Test] Functions found: %zu\n", prog->function_count);
    fprintf(stderr, "[Parser Test] Bytecode size: %zu bytes\n", prog->bytecode_len);
    
    /* Clean up */
    program_free(prog);
    vm_free(global_vm);
    
    return 0;
}

/* Main server */
int main(int argc, char **argv) {
    /* ADD THIS BLOCK AT THE START OF main() */
    /* Check for --parse-test mode */
    if (argc >= 3 && strcmp(argv[1], "--parse-test") == 0) {
        return test_parse_file(argv[2]);
    }
    
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
    
    /* Initialize game world */
    room_init_world();
    
    /* Initialize skill system */
    skill_init();
    
    /* Initialize combat system */
    combat_init();
    
    /* Initialize item system */
    item_init();
    
    /* Initialize psionics system (Phase 5) */
    psionics_init();
    
    /* Initialize magic system (Phase 5) */
    magic_init();
    
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
