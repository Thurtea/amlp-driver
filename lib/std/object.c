// Base object - foundation for all objects

private string id_name;
private string short_desc;
private string long_desc;
private object environment;

void create(void) {
    id_name = "object";
    short_desc = "an object";
    long_desc = "An ordinary object.";
}

void set_id(string str) {
    id_name = str;
}

string query_id(void) {
    return id_name;
}

void set_short(string str) {
    short_desc = str;
}

string query_short(void) {
    return short_desc;
}

void set_long(string str) {
    long_desc = str;
}

string query_long(void) {
    return long_desc;
}

void set_environment(object ob) {
    environment = ob;
}

object query_environment(void) {
    return environment;
}

void move(object dest) {
    if (environment) {
        environment->remove_object(this_object());
    }
    if (dest) {
        dest->add_object(this_object());
        environment = dest;
    }
}

void destruct_me(void) {
    destruct(this_object());
}
