#ifndef ROOM_H
#define ROOM_H

#include <stddef.h>

/* Forward declarations */
typedef struct InventoryItem InventoryItem;

/* Forward declare from session_internal.h */
typedef struct PlayerSession PlayerSession;

/* Room structure */
typedef struct Room {
    int id;
    char *name;
    char *description;
    struct {
        int north;
        int south;
        int east;
        int west;
        int up;
        int down;
    } exits;
    InventoryItem *items;  /* Items in room */
    PlayerSession **players;  /* Array of players in room */
    int num_players;
    int max_players;
} Room;

/* World management */
void room_init_world(void);
void room_cleanup_world(void);
Room* room_get_by_id(int id);
Room* room_get_start(void);

/* Room operations */
void room_add_player(Room *room, PlayerSession *player);
void room_remove_player(Room *room, PlayerSession *player);
void room_add_item(Room *room, InventoryItem *item);
InventoryItem* room_find_item(Room *room, const char *name);
void room_remove_item(Room *room, InventoryItem *item);

/* Room commands */
void cmd_look(PlayerSession *sess, const char *args);
void cmd_move(PlayerSession *sess, const char *direction);

#endif /* ROOM_H */
