#include "room.h"
#include "session_internal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ROOMS 100

/* World data */
static Room world_rooms[MAX_ROOMS];
static int num_rooms = 0;

/* External function from session.c */
extern void send_to_player(PlayerSession *session, const char *format, ...);

/* Initialize the game world */
void room_init_world(void) {
    /* Room 0: The Void (starting room) */
    Room *void_room = &world_rooms[0];
    void_room->id = 0;
    void_room->name = strdup("The Void");
    void_room->description = strdup(
        "You stand in an endless expanse of swirling energy and mist.\n"
        "This liminal space exists between worlds, a nexus of possibilities.\n"
        "To the north, you sense the pull of civilization."
    );
    void_room->exits.north = 1;
    void_room->exits.south = -1;
    void_room->exits.east = -1;
    void_room->exits.west = -1;
    void_room->exits.up = -1;
    void_room->exits.down = -1;
    void_room->items = NULL;
    void_room->players = calloc(10, sizeof(PlayerSession*));
    void_room->num_players = 0;
    void_room->max_players = 10;
    
    /* Room 1: Town Plaza */
    Room *plaza = &world_rooms[1];
    plaza->id = 1;
    plaza->name = strdup("Chi-Town Plaza");
    plaza->description = strdup(
        "A bustling town square in the heart of Chi-Town. Coalition troops patrol\n"
        "the perimeter while traders hawk their wares from makeshift stalls.\n"
        "The towering spires of Coalition headquarters rise to the east.\n"
        "To the west lies the merchant district, and south returns to the void."
    );
    plaza->exits.north = -1;
    plaza->exits.south = 0;
    plaza->exits.east = 2;
    plaza->exits.west = 3;
    plaza->exits.up = -1;
    plaza->exits.down = -1;
    plaza->items = NULL;
    plaza->players = calloc(10, sizeof(PlayerSession*));
    plaza->num_players = 0;
    plaza->max_players = 10;
    
    /* Room 2: Coalition HQ Entrance */
    Room *hq = &world_rooms[2];
    hq->id = 2;
    hq->name = strdup("Coalition Headquarters - Entrance");
    hq->description = strdup(
        "Massive adamantium doors loom before you, flanked by heavily armed\n"
        "SAMAS power armor suits. This is the nerve center of Coalition authority.\n"
        "Only the plaza to the west offers escape."
    );
    hq->exits.north = -1;
    hq->exits.south = -1;
    hq->exits.east = -1;
    hq->exits.west = 1;
    hq->exits.up = -1;
    hq->exits.down = -1;
    hq->items = NULL;
    hq->players = calloc(10, sizeof(PlayerSession*));
    hq->num_players = 0;
    hq->max_players = 10;
    
    /* Room 3: Merchant District */
    Room *market = &world_rooms[3];
    market->id = 3;
    market->name = strdup("Merchant District");
    market->description = strdup(
        "A narrow street lined with shops selling everything from energy weapons\n"
        "to magical trinkets. The air smells of oil and ozone. Merchants call out,\n"
        "trying to attract customers. The plaza lies to the east."
    );
    market->exits.north = -1;
    market->exits.south = -1;
    market->exits.east = 1;
    market->exits.west = -1;
    market->exits.up = -1;
    market->exits.down = -1;
    market->items = NULL;
    market->players = calloc(10, sizeof(PlayerSession*));
    market->num_players = 0;
    market->max_players = 10;
    
    num_rooms = 4;
    
    fprintf(stderr, "[Room] Initialized %d rooms\n", num_rooms);
}

/* Cleanup world */
void room_cleanup_world(void) {
    for (int i = 0; i < num_rooms; i++) {
        if (world_rooms[i].name) free(world_rooms[i].name);
        if (world_rooms[i].description) free(world_rooms[i].description);
        if (world_rooms[i].players) free(world_rooms[i].players);
    }
}

/* Get room by ID */
Room* room_get_by_id(int id) {
    if (id < 0 || id >= num_rooms) return NULL;
    return &world_rooms[id];
}

/* Get starting room */
Room* room_get_start(void) {
    return &world_rooms[0];
}

/* Add player to room */
void room_add_player(Room *room, PlayerSession *player) {
    if (!room || !player) return;
    
    /* Expand array if needed */
    if (room->num_players >= room->max_players) {
        room->max_players *= 2;
        room->players = realloc(room->players, room->max_players * sizeof(PlayerSession*));
    }
    
    room->players[room->num_players++] = player;
}

/* Remove player from room */
void room_remove_player(Room *room, PlayerSession *player) {
    if (!room || !player) return;
    
    for (int i = 0; i < room->num_players; i++) {
        if (room->players[i] == player) {
            /* Shift remaining players */
            for (int j = i; j < room->num_players - 1; j++) {
                room->players[j] = room->players[j + 1];
            }
            room->num_players--;
            return;
        }
    }
}

/* Broadcast message to all players in room (optionally excluding one) */
void room_broadcast(Room *room, const char *message, PlayerSession *exclude) {
    if (!room || !message) return;
    
    for (int i = 0; i < room->num_players; i++) {
        if (room->players[i] != exclude && room->players[i]->username) {
            send_to_player(room->players[i], "%s", message);
        }
    }
}

/* Look command */
void cmd_look(PlayerSession *sess, const char *args) {
    if (!sess || !sess->current_room) {
        send_to_player(sess, "You are nowhere.\n");
        return;
    }
    
    Room *room = sess->current_room;
    
    send_to_player(sess, "\n%s\n", room->name);
    send_to_player(sess, "%s\n", room->description);
    
    /* List exits */
    int exit_count = 0;
    char *exit_dirs[6];
    
    if (room->exits.north >= 0) exit_dirs[exit_count++] = "north";
    if (room->exits.south >= 0) exit_dirs[exit_count++] = "south";
    if (room->exits.east >= 0) exit_dirs[exit_count++] = "east";
    if (room->exits.west >= 0) exit_dirs[exit_count++] = "west";
    if (room->exits.up >= 0) exit_dirs[exit_count++] = "up";
    if (room->exits.down >= 0) exit_dirs[exit_count++] = "down";
    
    if (exit_count == 0) {
        send_to_player(sess, "\033[1;32mThere are no exits.\033[0m\n");
    } else if (exit_count == 1) {
        send_to_player(sess, "\033[1;32mThere is one exit: %s\033[0m\n", exit_dirs[0]);
    } else {
        send_to_player(sess, "\033[1;32mThere are %d exits: ", exit_count);
        for (int i = 0; i < exit_count; i++) {
            send_to_player(sess, "%s", exit_dirs[i]);
            if (i < exit_count - 1) {
                send_to_player(sess, ", ");
            }
        }
        send_to_player(sess, "\033[0m\n");
    }
    
    /* List other players by race (until introduced) */
    if (room->num_players > 1) {
        for (int i = 0; i < room->num_players; i++) {
            if (room->players[i] != sess && room->players[i]->character.race) {
                const char *race = room->players[i]->character.race;
                /* Determine article (a/an) based on first letter */
                char article = 'A';
                if (race[0] == 'A' || race[0] == 'E' || race[0] == 'I' || 
                    race[0] == 'O' || race[0] == 'U' ||
                    race[0] == 'a' || race[0] == 'e' || race[0] == 'i' || 
                    race[0] == 'o' || race[0] == 'u') {
                    article = 'n';
                }
                
                if (article == 'n') {
                    send_to_player(sess, "An %s is standing around.\n", race);
                } else {
                    send_to_player(sess, "A %s is standing around.\n", race);
                }
            }
        }
    }
}

/* Move command */
void cmd_move(PlayerSession *sess, const char *direction) {
    if (!sess || !sess->current_room || !direction) {
        send_to_player(sess, "You can't move.\n");
        return;
    }
    
    Room *current = sess->current_room;
    int next_room_id = -1;
    
    /* Determine direction */
    if (strcmp(direction, "north") == 0 || strcmp(direction, "n") == 0) {
        next_room_id = current->exits.north;
    } else if (strcmp(direction, "south") == 0 || strcmp(direction, "s") == 0) {
        next_room_id = current->exits.south;
    } else if (strcmp(direction, "east") == 0 || strcmp(direction, "e") == 0) {
        next_room_id = current->exits.east;
    } else if (strcmp(direction, "west") == 0 || strcmp(direction, "w") == 0) {
        next_room_id = current->exits.west;
    } else if (strcmp(direction, "up") == 0 || strcmp(direction, "u") == 0) {
        next_room_id = current->exits.up;
    } else if (strcmp(direction, "down") == 0 || strcmp(direction, "d") == 0) {
        next_room_id = current->exits.down;
    }
    
    if (next_room_id < 0) {
        send_to_player(sess, "You can't go that way.\n");
        return;
    }
    
    Room *next_room = room_get_by_id(next_room_id);
    if (!next_room) {
        send_to_player(sess, "That room doesn't exist!\n");
        return;
    }
    
    /* Notify room of departure */
    for (int i = 0; i < current->num_players; i++) {
        if (current->players[i] != sess) {
            send_to_player(current->players[i], "%s leaves %s.\n", 
                          sess->username, direction);
        }
    }
    
    /* Move player */
    room_remove_player(current, sess);
    room_add_player(next_room, sess);
    sess->current_room = next_room;
    
    /* Notify new room of arrival */
    for (int i = 0; i < next_room->num_players; i++) {
        if (next_room->players[i] != sess) {
            send_to_player(next_room->players[i], "%s arrives.\n", sess->username);
        }
    }
    
    /* Auto-look */
    cmd_look(sess, "");
}
