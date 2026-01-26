// Base room

inherit "/std/object";

private mapping exits;
private object *inventory;

void create(void) {
    ::create();
    exits = ([]);
    inventory = ({});
    set_id("room");
}

void add_exit(string direction, string destination) {
    if (!exits) exits = ([]);
    exits[direction] = destination;
}

string query_exit(string direction) {
    if (!exits) return 0;
    return exits[direction];
}

string *query_exits(void) {
    if (!exits) return ({});
    return keys(exits);
}

void add_object(object ob) {
    if (!inventory) inventory = ({});
    inventory += ({ ob });
}

void remove_object(object ob) {
    if (!inventory) return;
    inventory -= ({ ob });
}

object *query_inventory(void) {
    if (!inventory) return ({});
    return inventory;
}

string look(void) {
    string desc, *exit_list;
    int i;
    
    desc = query_long() + "\n";
    
    exit_list = query_exits();
    if (sizeof(exit_list) > 0) {
        desc += "Exits: ";
        for (i = 0; i < sizeof(exit_list); i++) {
            desc += exit_list[i];
            if (i < sizeof(exit_list) - 1) desc += ", ";
        }
        desc += "\n";
    }
    
    return desc;
}
