// /cmds/admin/wiztool.lpc
// Command to load and interact with the wiztool
//
// Usage:
//   wiztool              - Load the wiztool if not already loaded
//   wiz <command> [args] - Execute a wiztool command
//
// First time usage:
//   > wiztool
//   You load your wiztool.
//
// Then use it:
//   > wiz help
//   > wiz clone /std/object
//   > wiz eval 1 + 1

int main(string args) {
    object player = this_player();
    object wiztool;
    string cmd, cmd_args;
    int space_pos;
    
    if (!player) {
        write("Error: No player context.\n");
        return 1;
    }
    
    // Check if player is wizard or admin
    if (player->query_privilege_level() < 1) {
        write("You don't have permission to use the wiztool.\n");
        return 1;
    }
    
    // Check if player already has wiztool loaded
    wiztool = present("wiztool", player);
    
    if (!wiztool) {
        // Load the wiztool for the first time
        wiztool = clone_object("/std/wiztool");
        if (!wiztool) {
            write("Error: Failed to load wiztool.\n");
            return 1;
        }
        
        wiztool->set_owner(player);
        wiztool->move(player);
        write("You load your wiztool. Type 'wiz help' for commands.\n");
        return 1;
    }
    
    // If no arguments, just confirm wiztool is loaded
    if (!args || args == "") {
        write("Your wiztool is already loaded. Type 'wiz help' for commands.\n");
        return 1;
    }
    
    // Parse command and arguments
    space_pos = strstr(args, " ");
    if (space_pos == -1) {
        cmd = args;
        cmd_args = "";
    } else {
        cmd = args[0..space_pos-1];
        cmd_args = args[space_pos+1..];
    }
    
    // Process the wiztool command
    string result = wiztool->process_command(cmd, cmd_args);
    write(result);
    
    return 1;
}

void help() {
    write("Wiztool - Administrative Building Tool\n\n");
    write("Usage:\n");
    write("  wiztool              - Load the wiztool\n");
    write("  wiz <command> [args] - Execute wiztool command\n\n");
    write("Examples:\n");
    write("  wiztool             - Load tool\n");
    write("  wiz help            - Show all commands\n");
    write("  wiz clone /std/object\n");
    write("  wiz eval 1 + 1\n");
    write("  wiz cd /domains/start/room\n");
    write("  wiz ls\n\n");
    write("Type 'wiz help' after loading for full command list.\n");
}
