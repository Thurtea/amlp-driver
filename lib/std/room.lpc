// lib/std/room.c - Room object with exit system
// Based on Nightmare and Dead Souls implementations

inherit "/std/object";

private mapping exits;           // Direction -> room path mapping
private mapping exit_descriptions; // Direction -> custom description
private mapping items;            // Searchable items in room
private object *inventory;        // Objects/players in room
private string long_desc;         // Main room description
private string short_desc;        // Room title
private int light_level;          // Ambient light (0 = dark, 1+ = lit)

void create() {
    ::create();
    exits = ([]);
    exit_descriptions = ([]);
    items = ([]);
    inventory = ({});
    light_level = 1;  // Rooms are lit by default
    short_desc = "A room";
    long_desc = "You see nothing special.\n";
}

// Description management
void set_short(string desc) { short_desc = desc; }
string short() { return short_desc; }

void set_long(string desc) { long_desc = desc; }
string long() {
    string result = short_desc + "\n" + long_desc;
    
    // Add exit list
    string *exit_list = query_exits();
    if (sizeof(exit_list) > 0) {
        result += "\nObvious exits: " + implode(exit_list, ", ") + "\n";
    } else {
        result += "\nThere are no obvious exits.\n";
    }
    
    // List objects/players in room
    if (sizeof(inventory) > 0) {
        foreach(object ob : inventory) {
            if (ob && ob != this_player()) {
                string s = ob->short();
                if (s && s != "") {
                    result += s + " is here.\n";
                }
            }
        }
    }
    
    return result;
}

// Exit management
void add_exit(string direction, string destination) {
    if (!direction || !destination) {
        return;
    }
    exits[direction] = destination;
}

void remove_exit(string direction) {
    if (exits[direction]) {
        map_delete(exits, direction);
    }
}

string query_exit(string direction) {
    return exits[direction];
}

string *query_exits() {
    if (!exits || !sizeof(exits)) return ({});
    return keys(exits);
}

void set_exits(mapping exit_map) {
    exits = exit_map ? copy(exit_map) : ([]);
}

mapping query_exit_map() {
    return copy(exits);
}

// Exit descriptions for look <direction>
void set_exit_description(string direction, string desc) {
    exit_descriptions[direction] = desc;
}

string query_exit_description(string direction) {
    return exit_descriptions[direction];
}

// Searchable items (for "look at table" etc)
void add_item(string id, string description) {
    items[id] = description;
}

void remove_item(string id) {
    if (items[id]) {
        map_delete(items, id);
    }
}

string query_item(string id) {
    return items[id];
}

mapping query_items() {
    return copy(items);
}

// Light management
void set_light(int level) {
    light_level = level;
}

int query_light() {
    return light_level;
}

int is_lit() {
    return light_level > 0;
}

// Inventory management for room
void add_inventory(object ob) {
    if (!ob) return;
    inventory += ({ ob });
    ob->set_environment(this_object());
}

void remove_inventory(object ob) {
    if (!ob) return;
    inventory -= ({ ob });
}

object *query_inventory() {
    return copy(inventory);
}

object *all_inventory() {
    return copy(inventory);
}

// Player entry/exit messages
void tell_room(object exclude, string message) {
    if (!message || message == "") return;
    
    foreach(object ob : inventory) {
        if (ob && ob != exclude) {
            // TODO: When tell_object efun ready:
            // tell_object(ob, message);
            
            // For now, if it's a player session, use C server
            ob->receive_message(message);
        }
    }
}

// Called when player enters room
void enter_room(object who) {
    if (!who) return;
    
    string name = who->query_name();
    if (name && name != "") {
        tell_room(who, name + " arrives.\n");
    }
}

// Called when player leaves room
void exit_room(object who, string direction) {
    if (!who) return;
    
    string name = who->query_name();
    if (name && name != "" && direction && direction != "") {
        string dir_msg = direction;
        // Convert direction to readable form
        switch(direction) {
            case "n": case "north": dir_msg = "north"; break;
            case "s": case "south": dir_msg = "south"; break;
            case "e": case "east": dir_msg = "east"; break;
            case "w": case "west": dir_msg = "west"; break;
            case "u": case "up": dir_msg = "up"; break;
            case "d": case "down": dir_msg = "down"; break;
        }
        tell_room(who, name + " leaves " + dir_msg + ".\n");
    }
}

// Reset function - called periodically by driver
void reset() {
    // Override this in individual rooms to respawn items/NPCs
}

// Clean up function
void clean_up() {
    // Remove objects that have been inactive too long
    // Override if needed
}
