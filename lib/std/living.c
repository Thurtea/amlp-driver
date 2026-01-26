// lib/std/living.c - Base class for all living creatures

inherit "/std/object";

private int hp;
private int max_hp;
private int is_alive;

void create() {
    ::create();
    hp = 100;
    max_hp = 100;
    is_alive = 1;
}

// HP management
int query_hp() { return hp; }
int query_max_hp() { return max_hp; }
void set_hp(int new_hp) { hp = new_hp; }
void set_max_hp(int new_max) { max_hp = new_max; }

void heal(int amount) {
    hp += amount;
    if (hp > max_hp) hp = max_hp;
}

void damage(int amount) {
    hp -= amount;
    if (hp <= 0) {
        hp = 0;
        die();
    }
}

void die() {
    is_alive = 0;
    // Override in subclasses for death handling
}

int is_living() { return is_alive; }
