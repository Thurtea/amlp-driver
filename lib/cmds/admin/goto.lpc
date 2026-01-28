// Goto command - Teleport to room

#include <globals.h>

int main(string arg) {
    object user, dest;
    
    user = previous_object();
    
    if (user->query_wizard_level() < WIZARD_LEVEL) {
        tell_object(user, "You are not a wizard.\n");
        return 1;
    }
    
    if (!arg || arg == "") {
        tell_object(user, "Goto where?\nUsage: goto <room_path>\n");
        return 1;
    }
    
    catch(dest = load_object(arg));
    
    if (!dest) {
        tell_object(user, "Failed to load: " + arg + "\n");
        return 1;
    }
    
    tell_object(user, "Teleporting to " + arg + "...\n");
    user->move(dest);
    user->force_me("look");
    
    return 1;
}
