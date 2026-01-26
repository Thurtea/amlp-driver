// lib/std/player.c - Base Player Object with Privilege System
// 
// Privilege Levels:
// 0 = Player (normal user)
// 1 = Wizard (builder access)
// 2 = Admin (full system access)

inherit "/std/living";

private string name;
private string password_hash;
private int privilege_level;
private string title;
private object *inventory;
private object environment;
private mapping stats;
private int last_login;
private int total_login_time;

// Initialization
void create() {
    ::create();
    inventory = ({ });
    stats = ([
        "strength": 10,
        "dexterity": 10,
        "intelligence": 10,
        "level": 1,
        "experience": 0,
        "hp": 100,
        "max_hp": 100,
    ]);
    privilege_level = 0;  // Default to player
    title = "Player";
}

// Setup new player
void setup_player(string player_name, string pass_hash) {
    name = player_name;
    password_hash = pass_hash;
    set_name(player_name);
    set_short(player_name + " the " + title);
    set_long("This is " + player_name + ", a " + title + ".\n");
    last_login = time();
}

// Privilege system
int query_privilege_level() { return privilege_level; }
int is_player() { return privilege_level == 0; }
int is_wizard() { return privilege_level >= 1; }
int is_admin() { return privilege_level >= 2; }

void set_privilege_level(int level) {
    if (level < 0 || level > 2) return;
    privilege_level = level;
    
    switch(level) {
        case 0:
            title = "Player";
            break;
        case 1:
            title = "Wizard";
            break;
        case 2:
            title = "Admin";
            break;
    }
    
    set_short(name + " the " + title);
}

string query_privilege_title() { return title; }

// Command processing - CRITICAL FUNCTION
mixed process_command(string cmd) {
    string verb, args;
    
    if (!cmd || cmd == "") {
        return "Empty command.\n";
    }
    
    // Parse command into verb and arguments
    if (sscanf(cmd, "%s %s", verb, args) != 2) {
        verb = cmd;
        args = "";
    }
    
    verb = lower_case(verb);
    
    // Route to command handlers
    switch(verb) {
        case "look":
        case "l":
            return cmd_look(args);
        
        case "inventory":
        case "i":
            return cmd_inventory();
        
        case "say":
            return cmd_say(args);
        
        case "emote":
            return cmd_emote(args);
        
        case "who":
            return cmd_who();
        
        case "stats":
            return cmd_stats();
        
        // Wizard commands
        case "goto":
            if (!is_wizard()) return "You don't have permission to use that command.\n";
            return cmd_goto(args);
        
        case "clone":
            if (!is_wizard()) return "You don't have permission to use that command.\n";
            return cmd_clone(args);
        
        // Admin commands
        case "promote":
            if (!is_admin()) return "You don't have permission to use that command.\n";
            return cmd_promote(args);
        
        case "shutdown":
            if (!is_admin()) return "You don't have permission to use that command.\n";
            return cmd_shutdown(args);
        
        case "users":
            if (!is_admin()) return "You don't have permission to use that command.\n";
            return cmd_users();
        
        // Movement
        case "north":
        case "n":
            return cmd_move("north");
        case "south":
        case "s":
            return cmd_move("south");
        case "east":
        case "e":
            return cmd_move("east");
        case "west":
        case "w":
            return cmd_move("west");
        case "up":
        case "u":
            return cmd_move("up");
        case "down":
        case "d":
            return cmd_move("down");
        
        default:
            return "Unknown command: " + verb + "\nType 'help' for available commands.\n";
    }
}

// Command implementations
string cmd_look(string args) {
    if (!environment) {
        return "You are nowhere.\n";
    }
    return environment->long();
}

string cmd_inventory() {
    if (sizeof(inventory) == 0) {
        return "You are carrying nothing.\n";
    }
    
    string result = "You are carrying:\n";
    foreach(object item : inventory) {
        result += "  " + item->short() + "\n";
    }
    return result;
}

string cmd_say(string message) {
    if (!message || message == "") {
        return "Say what?\n";
    }
    
    // Broadcast to room
    if (environment) {
        environment->tell_room(this_object(), name + " says: " + message + "\n");
    }
    
    return "You say: " + message + "\n";
}

string cmd_emote(string action) {
    if (!action || action == "") {
        return "Emote what?\n";
    }
    
    // Broadcast to room
    if (environment) {
        environment->tell_room(this_object(), name + " " + action + "\n");
    }
    
    return name + " " + action + "\n";
}

string cmd_who() {
    object *players = users();
    string result = "Players online:\n";
    
    foreach(object player : players) {
        string pname = player->query_name();
        string ptitle = player->query_privilege_title();
        int idle = query_idle(player);
        
        result += sprintf("  %-15s [%s] (idle: %d seconds)\n", 
                         pname, ptitle, idle);
    }
    
    result += sprintf("\nTotal: %d player%s\n", 
                     sizeof(players), 
                     sizeof(players) == 1 ? "" : "s");
    
    return result;
}

string cmd_stats() {
    string result = "Your stats:\n";
    result += sprintf("  Name: %s\n", name);
    result += sprintf("  Title: %s\n", title);
    result += sprintf("  Privilege: %d (%s)\n", privilege_level, 
                     is_admin() ? "Admin" : is_wizard() ? "Wizard" : "Player");
    result += sprintf("  Level: %d\n", stats["level"]);
    result += sprintf("  HP: %d/%d\n", stats["hp"], stats["max_hp"]);
    result += sprintf("  Strength: %d\n", stats["strength"]);
    result += sprintf("  Dexterity: %d\n", stats["dexterity"]);
    result += sprintf("  Intelligence: %d\n", stats["intelligence"]);
    return result;
}

// Wizard commands
string cmd_goto(string location) {
    if (!location || location == "") {
        return "Goto where?\n";
    }
    
    object target = find_object(location);
    if (!target) {
        return "Location not found: " + location + "\n";
    }
    
    move_player(target);
    return "You teleport to " + location + ".\n" + cmd_look("");
}

string cmd_clone(string obj_path) {
    if (!obj_path || obj_path == "") {
        return "Clone what?\n";
    }
    
    object new_obj = clone_object(obj_path);
    if (!new_obj) {
        return "Failed to clone: " + obj_path + "\n";
    }
    
    new_obj->move(this_object());
    return "You clone " + new_obj->short() + ".\n";
}

// Admin commands
string cmd_promote(string args) {
    string target_name, level_str;
    int new_level;
    
    if (sscanf(args, "%s %s", target_name, level_str) != 2) {
        return "Usage: promote <player> <level>\nLevels: 0=player, 1=wizard, 2=admin\n";
    }
    
    new_level = to_int(level_str);
    if (new_level < 0 || new_level > 2) {
        return "Invalid level. Use 0 (player), 1 (wizard), or 2 (admin).\n";
    }
    
    object target = find_player(target_name);
    if (!target) {
        return "Player not found: " + target_name + "\n";
    }
    
    target->set_privilege_level(new_level);
    
    string level_name = (new_level == 2) ? "Admin" : 
                       (new_level == 1) ? "Wizard" : "Player";
    
    return sprintf("Promoted %s to %s (level %d).\n", 
                   target_name, level_name, new_level);
}

string cmd_shutdown(string args) {
    // Broadcast shutdown warning
    tell_all_players("SYSTEM: Admin " + name + " is shutting down the server.\n");
    
    // Call master object shutdown
    call_other(master(), "shutdown", to_int(args));
    
    return "Shutdown initiated.\n";
}

string cmd_users() {
    object *players = users();
    string result = "Connected users:\n";
    result += sprintf("%-15s %-10s %-15s %s\n", 
                     "Name", "Privilege", "IP Address", "Idle");
    result += "--------------------------------------------------------\n";
    
    foreach(object player : players) {
        string pname = player->query_name();
        string ptitle = player->query_privilege_title();
        string ip = query_ip_number(player);
        int idle = query_idle(player);
        
        result += sprintf("%-15s %-10s %-15s %d sec\n", 
                         pname, ptitle, ip, idle);
    }
    
    return result;
}

// Movement
string cmd_move(string direction) {
    if (!environment) {
        return "You are nowhere.\n";
    }
    
    object exit = environment->query_exit(direction);
    if (!exit) {
        return "You can't go that way.\n";
    }
    
    move_player(exit);
    return cmd_look("");
}

void move_player(object destination) {
    if (environment) {
        environment->remove_player(this_object());
    }
    
    environment = destination;
    
    if (destination) {
        destination->add_player(this_object());
    }
}

// Accessors
string query_name() { return name; }
string query_password_hash() { return password_hash; }
object query_environment() { return environment; }
void set_environment(object env) { environment = env; }

// Save/restore (for future file I/O system)
mapping save_data() {
    return ([
        "name": name,
        "password_hash": password_hash,
        "privilege_level": privilege_level,
        "title": title,
        "stats": stats,
        "last_login": last_login,
        "total_login_time": total_login_time,
    ]);
}

void restore_data(mapping data) {
    name = data["name"];
    password_hash = data["password_hash"];
    privilege_level = data["privilege_level"];
    title = data["title"];
    stats = data["stats"];
    last_login = data["last_login"];
    total_login_time = data["total_login_time"];
    
    set_name(name);
    set_short(name + " the " + title);
}
