// /cmds/admin/wiz.lpc
// Shorthand command for wiztool operations
//
// This is a convenience wrapper that calls the wiztool command
// with arguments, so you can type "wiz help" instead of "wiztool help"
//
// Usage:
//   wiz <command> [args] - Execute wiztool command
//   wiz help             - Show all wiztool commands
//   wiz                  - Show help (same as wiz help)

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
        write("Ask an administrator to promote you: promote <name> 1\n");
        return 1;
    }
    
    // Check if wiztool is loaded
    wiztool = present("wiztool", player);
    
    if (!wiztool) {
        write("You need to load your wiztool first.\n");
        write("Type: wiztool\n");
        return 1;
    }
    
    // If no arguments, show help
    if (!args || args == "") {
        args = "help";
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
    write("wiz - Quick wiztool command access\n\n");
    write("Usage: wiz <command> [arguments]\n\n");
    write("This is a shorthand for using the wiztool.\n");
    write("Type 'wiz help' to see all available commands.\n\n");
    write("First, load your wiztool:\n");
    write("  > wiztool\n\n");
    write("Then use wiz commands:\n");
    write("  > wiz help\n");
    write("  > wiz clone /std/object\n");
    write("  > wiz cd /domains/start\n");
}
