// domains/tutorial/objects/weapon.c - Test weapon object
// Inherits from std/object and adds weapon-specific properties

inherit "std/object.c";

int damage;
string weapon_type;

void create() {
    ::create();
    damage = 5;
    weapon_type = "sword";
}

void setup() {
    ::setup();
}

int query_damage() {
    return damage;
}

void set_damage(int dmg) {
    damage = dmg;
}

string query_weapon_type() {
    return weapon_type;
}

void set_weapon_type(string type) {
    weapon_type = type;
}

string query_short() {
    return "a " + weapon_type;
}

string query_long() {
    return "This is a " + weapon_type + " that deals " + damage + " damage.";
}
