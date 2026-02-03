// std/container.c - Base object for containers and inventory
// Inherits from std/object and adds container functionality

inherit "std/object.c";

object *inventory;
int max_weight;
int current_weight;

void create() {
    ::create();
    inventory = ({});
    max_weight = 1000;
    current_weight = 0;
}

void add_item(object item) {
    inventory += ({ item });
}

void remove_item(object item) {
    inventory -= ({ item });
}

object *query_inventory() {
    return copy(inventory);
}

int query_max_weight() {
    return max_weight;
}

void set_max_weight(int weight) {
    max_weight = weight;
}

int query_current_weight() {
    return current_weight;
}
