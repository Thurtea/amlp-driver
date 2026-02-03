// std/living.c - Base object for all living things (NPCs, players)
// Inherits from std/object and adds life-related properties

inherit "std/object.c";

int hp;
int max_hp;
int level;
string name;

void create() {
    ::create();
    hp = 10;
    max_hp = 10;
    level = 1;
    name = "creature";
}

int query_hp() {
    return hp;
}

void set_hp(int new_hp) {
    hp = new_hp;
}

int query_max_hp() {
    return max_hp;
}

void set_max_hp(int new_max) {
    max_hp = new_max;
}

int query_level() {
    return level;
}

void set_level(int new_level) {
    level = new_level;
}

string query_name() {
    return name;
}

void set_name(string new_name) {
    name = new_name;
}
