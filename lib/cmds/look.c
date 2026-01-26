// Look command

#include <globals.h>

int main(string arg) {
    object room, user;
    
    user = previous_object();
    room = user->query_environment();
    
    if (!room) {
        tell_object(user, "You are nowhere.\n");
        return 1;
    }
    
    tell_object(user, room->look());
    return 1;
}
