// Wiz command - List wizard commands

#include <globals.h>

int main(string arg) {
    object user;
    string output;
    
    user = previous_object();
    
    if (user->query_wizard_level() < WIZARD_LEVEL) {
        tell_object(user, "You are not a wizard.\n");
        return 1;
    }
    
    output = "\n";
    output += "========================================\n";
    output += "       Wizard Commands\n";
    output += "========================================\n";
    output += "\n";
    output += "Building:\n";
    output += "  goto <path>    - Teleport to room\n";
    output += "  clone <path>   - Clone an object\n";
    output += "  destruct <ob>  - Destroy an object\n";
    output += "  update <path>  - Reload object\n";
    output += "\n";
    output += "Information:\n";
    output += "  stat <object>  - Show object stats\n";
    output += "  objects        - List loaded objects\n";
    output += "  errors         - Show recent errors\n";
    output += "\n";
    output += "World Editing:\n";
    output += "  ed <file>      - Edit file (opens editor)\n";
    output += "  cat <file>     - Display file contents\n";
    output += "  ls <dir>       - List directory\n";
    output += "\n";
    output += "System:\n";
    output += "  shutdown       - Stop the server\n";
    output += "  people         - List all users\n";
    output += "\n";
    
    tell_object(user, output);
    return 1;
}
