// domains/tutorial/npc/guard.c - Test NPC object
// Inherits from std/living and represents a guard NPC

inherit "std/living.c";

void create() {
    ::create();
    set_name("Guard");
    set_hp(50);
    set_max_hp(50);
    set_level(5);
}

void setup() {
    ::setup();
}

string query_short() {
    return "a guard";
}

string query_long() {
    return "This is a guard with " + query_hp() + "/" + query_max_hp() + " HP, level " + query_level() + ".";
}
