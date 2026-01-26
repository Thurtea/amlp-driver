// Base living - for NPCs and players

inherit "/std/object";

private string name;
private int hp, max_hp;

void create(void) {
    ::create();
    hp = 100;
    max_hp = 100;
}

void set_name(string str) {
    name = str;
    set_id(str);
    set_short(str);
}

string query_name(void) {
    return name;
}

void set_hp(int n) {
    hp = n;
}

int query_hp(void) {
    return hp;
}

void receive_message(string msg) {
    tell_object(this_object(), msg);
}
