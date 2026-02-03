// std/room.c - Base object for all rooms/locations
// Inherits from std/object and adds room-specific features

inherit "std/object.c";

string short_desc;
string long_desc;
mapping exits;

void create() {
    ::create();
    short_desc = "A generic room";
    long_desc = "This is a generic room.";
    exits = ([]);
}

string query_short() {
    return short_desc;
}

void set_short(string desc) {
    short_desc = desc;
}

string query_long() {
    return long_desc;
}

void set_long(string desc) {
    long_desc = desc;
}

void add_exit(string direction, string destination) {
    exits[direction] = destination;
}

mapping query_exits() {
    return exits;
}

string *query_exit_dirs() {
    return keys(exits);
}
