// lib/std/object.c - Base class for all objects

private string short_desc;
private string long_desc;
private string name;
private object *inventory;
private object environment;

void create() {
    short_desc = "an object";
    long_desc = "You see nothing special.\n";
    name = "object";
    inventory = ({ });
    environment = 0;
}

// Descriptions
string short() { return short_desc; }
string long() { return long_desc; }
void set_short(string desc) { short_desc = desc; }
void set_long(string desc) { long_desc = desc; }

// Name
string query_name() { return name; }
void set_name(string n) { name = n; }

// Container functions
int move(object dest) {
    if (environment) {
        environment->remove_inventory(this_object());
    }
    
    environment = dest;
    
    if (dest) {
        dest->add_inventory(this_object());
    }
    
    return 1;
}

object query_environment() { return environment; }
object *query_inventory() { return inventory; }

void add_inventory(object ob) {
    inventory += ({ ob });
}

void remove_inventory(object ob) {
    inventory -= ({ ob });
}
