// Say command

#include <globals.h>

int main(string arg) {
    object *inv;
    object room, user;
    string name;
    int i;
    
    user = previous_object();
    if (!arg || arg == "") {
        tell_object(user, "Say what?\n");
        return 1;
    }
    
    name = user->query_name();
    room = user->query_environment();
    
    if (room) {
        inv = all_inventory(room);
        for (i = 0; i < sizeof(inv); i++) {
            if (inv[i] != user) {
                tell_object(inv[i], name + " says: " + arg + "\n");
            }
        }
    }
    
    tell_object(user, "You say: " + arg + "\n");
    return 1;
}
