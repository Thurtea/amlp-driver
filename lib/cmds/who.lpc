// Who command - Show online players

#include <globals.h>

int main(string arg) {
    object *players;
    object user;
    int i;
    string output;
    
    user = previous_object();
    players = users();
    
    output = "\n";
    output += "Players online:\n";
    output += "========================================\n";
    
    for (i = 0; i < sizeof(players); i++) {
        if (players[i]->query_name()) {
            output += players[i]->query_title();
            output += players[i]->query_name();
            output += " the " + capitalize(players[i]->query_race());
            output += "\n";
        }
    }
    
    output += "========================================\n";
    output += "Total: " + sizeof(players) + " player";
    if (sizeof(players) != 1) output += "s";
    output += "\n\n";
    
    tell_object(user, output);
    return 1;
}
