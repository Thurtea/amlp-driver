#include <globals.h>
// lib/std/player.c - Base Player Object with Privilege System
// 
// Privilege Levels:
// 0 = Player (normal user)
// 1 = Wizard (builder access)
// 2 = Admin (full system access)
//
// Now inherits enhanced living.lpc with:
// - Language system (speaking, understanding, proficiency)
// - Introduction system (name visibility)
// - Palladium stats (IQ, ME, MA, PS, PP, PE, PB, SPD)
// - Skills, Psionics, and Magic

inherit "/std/living";

private string name;
private string password_hash;
private int privilege_level;
private string title;
private object *inventory;
private object environment;
private int last_login;
private int total_login_time;
private int level;           /* Character level */
private int experience;      /* Experience points */
private object wiztool;      /* Wiztool for wizards/admins */

/* Character creation data */
private string race_selected;
private string occ_selected;

// Initialization
void create() {
    ::create();
    inventory = ({ });
    privilege_level = 0;  // Default to player
    title = "Player";
    level = 1;
    experience = 0;
    
    /* Set real name for introduction system */
    set_real_name("");
    
    /* Default starting language is American */
    add_language("american", 98);
    set_primary_language("american");
}

// Setup new player
void setup_player(string player_name, string pass_hash) {
    name = player_name;
    password_hash = pass_hash;
    set_name(player_name);
    set_real_name(player_name);  /* For introduction system */
    set_short(player_name + " the " + title);
    set_long("This is " + player_name + ", a " + title + ".\n");
    last_login = time();
    
    // Attach wiztool for wizards and admins
    if (privilege_level >= 1) {
        attach_wiztool();
    }
}

// Level and experience
int query_level() { return level; }
void set_level(int lvl) { level = lvl; }
int query_experience() { return experience; }
void set_experience(int exp) { experience = exp; }
void add_experience(int exp) {
    experience += exp;
    /* Check for level up - basic formula */
    int needed = level * 1000;
    while (experience >= needed && level < 15) {
        level++;
        needed = level * 1000;
        tell_object(this_object(), 
            "\n*** LEVEL UP! You are now level " + level + "! ***\n\n");
        on_level_up();
    }
}

/* Called when player levels up - can be overridden */
void on_level_up() {
    /* ISP regeneration for psionics */
    string psi_cat = query_psionic_category();
    if (psi_cat && psi_cat != "none") {
        int isp_gain;
        switch (psi_cat) {
            case "minor": isp_gain = random(6) + 1; break;
            case "major": isp_gain = random(6) + 4; break;
            case "master": isp_gain = random(10) + 5; break;
            default: isp_gain = 0;
        }
        set_max_isp(query_max_isp() + isp_gain);
        tell_object(this_object(), "Your max ISP increased by " + isp_gain + ".\n");
    }
    
    /* PPE for spell casters */
    if (query_spell_caster_level() > 0) {
        int ppe_gain = random(6) + random(6) + 2;
        set_max_ppe(query_max_ppe() + ppe_gain);
        set_spell_caster_level(level);  /* Caster level = character level for primary */
        tell_object(this_object(), "Your max P.P.E. increased by " + ppe_gain + ".\n");
    }
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

// Wiztool functions
void attach_wiztool() {
    if (wiztool) {
        // Already have one
        return;
    }
    
    wiztool = clone_object("/std/wiztool");
    if (wiztool) {
        wiztool->attach(this_object());
    }
}

void detach_wiztool() {
    if (wiztool) {
        wiztool->detach();
        destruct(wiztool);
        wiztool = 0;
    }
}

object query_wiztool() {
    return wiztool;
}

void force_me(string cmd) {
    if (!cmd || cmd == "") return;
    process_command(cmd);
}

int query_privilege() {
    return privilege_level;
}

// Command processing - CRITICAL FUNCTION
mixed process_command(string cmd) {
    if (!cmd || cmd == "") {
        return "Empty command.\n";
    }
    
    // Route command to command daemon
    // The command daemon will check wiztool first, then other sources
    object cmd_daemon = load_object(COMMAND_D);
    if (cmd_daemon) {
        return cmd_daemon->execute_command(this_object(), cmd);
    }
    
    // Fallback if command daemon not available
    return "Command system not initialized.\n";
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

string cmd_ooc(string message) {
    if (!message || message == "") {
        return "OOC what?\n";
    }
    
    /* OOC chat - no language processing, broadcasts globally */
    object *players = users();
    string output = "[OOC] " + name + ": " + message + "\n";
    
    foreach (object player : players) {
        if (player != this_object()) {
            tell_object(player, output);
        }
    }
    
    return "[OOC] You say: " + message + "\n";
}

string cmd_who() {
    object *players = users();
    int total = sizeof(players);
    string result = "";
    
    /* Wizards and admins see everyone */
    if (is_wizard()) {
        result = "Players online:\n";
        
        foreach(object player : players) {
            string pname = player->query_name();
            string ptitle = player->query_privilege_title();
            int idle = query_idle(player);
            
            result += sprintf("  %-15s [%s] (idle: %d seconds)\n", 
                             pname, ptitle, idle);
        }
        
        result += sprintf("\nTotal: %d player%s\n", 
                         total, total == 1 ? "" : "s");
    } else {
        /* Players only see wizards and admins */
        result = "Staff online:\n";
        int staff_count = 0;
        
        foreach(object player : players) {
            if (player->is_wizard()) {
                string pname = player->query_name();
                string ptitle = player->query_privilege_title();
                result += sprintf("  %-15s [%s]\n", pname, ptitle);
                staff_count++;
            }
        }
        
        if (staff_count == 0) {
            result = "No staff currently online.\n";
        }
        
        /* Population indicator */
        string pop_level;
        if (total <= 10) pop_level = "Low";
        else if (total <= 50) pop_level = "Medium";
        else pop_level = "High";
        
        result += "\nWorld Population: " + pop_level + "\n";
    }
    
    return result;
}

string cmd_stats() {
    string result = "";
    
    result += "╔══════════════════════════════════════════════════════╗\n";
    result += "║  " + sprintf("%-50s", name + " - " + title) + "  ║\n";
    result += "╠══════════════════════════════════════════════════════╣\n";
    
    /* Basic info */
    result += sprintf("║  Level: %-5d  Experience: %-10d              ║\n", 
                     level, experience);
    result += sprintf("║  Race: %-15s  OCC: %-18s  ║\n",
                     query_race_name(), query_occ_id() ? query_occ_id() : "None");
    result += "╠══════════════════════════════════════════════════════╣\n";
    
    /* Health */
    result += sprintf("║  HP: %d/%d  S.D.C.: %d/%d  M.D.C.: %d/%d             ║\n", 
                     query_hp(), query_max_hp(),
                     query_sdc(), query_max_sdc(),
                     query_mdc(), query_max_mdc());
    
    /* ISP and PPE */
    if (query_psionic_category() != "none" || query_spell_caster_level() > 0) {
        result += "╠══════════════════════════════════════════════════════╣\n";
        if (query_psionic_category() != "none") {
            result += sprintf("║  ISP: %d/%d  (%s Psionics)                       ║\n",
                             query_isp(), query_max_isp(), 
                             capitalize(query_psionic_category()));
        }
        if (query_spell_caster_level() > 0) {
            result += sprintf("║  P.P.E.: %d/%d  (Caster Level %d)                    ║\n",
                             query_ppe(), query_max_ppe(), query_spell_caster_level());
        }
    }
    
    /* Palladium Stats */
    result += "╠══════════════════════════════════════════════════════╣\n";
    result += "║  ATTRIBUTES                                          ║\n";
    result += sprintf("║  IQ: %-3d   ME: %-3d   MA: %-3d   PS: %-3d              ║\n",
                     query_stat("iq"), query_stat("me"), 
                     query_stat("ma"), query_stat("ps"));
    result += sprintf("║  PP: %-3d   PE: %-3d   PB: %-3d   SPD: %-3d             ║\n",
                     query_stat("pp"), query_stat("pe"),
                     query_stat("pb"), query_stat("spd"));
    
    /* Language */
    result += "╠══════════════════════════════════════════════════════╣\n";
    result += sprintf("║  Speaking: %-20s                   ║\n", 
                     query_primary_language());
    
    result += "╚══════════════════════════════════════════════════════╝\n";
    
    result += "\nUse 'languages' to see known languages.\n";
    result += "Use 'skills' to see your skills.\n";
    
    if (query_psionic_category() != "none") {
        result += "Use 'manifest' to see your psionic powers.\n";
    }
    if (query_spell_caster_level() > 0) {
        result += "Use 'cast' to see your spells.\n";
    }
    
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

/*
 * Execute a command from a file in /cmds/
 */
string execute_cmd_file(string verb, string args) {
    string path = "/cmds/" + verb;
    object cmd_ob = find_object(path);
    
    if (!cmd_ob) {
        cmd_ob = load_object(path);
    }
    
    if (!cmd_ob) {
        return "Command not found: " + verb + "\n";
    }
    
    /* Try different calling conventions */
    int result;
    
    if (function_exists("cmd", cmd_ob)) {
        result = cmd_ob->cmd(args);
    } else if (function_exists("main", cmd_ob)) {
        result = cmd_ob->main(args);
    } else {
        return "Command has no entry point: " + verb + "\n";
    }
    
    return "";  /* Command handles its own output via tell_object/write */
}

// Save/restore (for file I/O system)
mapping save_data() {
    return ([
        "name": name,
        "password_hash": password_hash,
        "privilege_level": privilege_level,
        "title": title,
        "level": level,
        "experience": experience,
        "last_login": last_login,
        "total_login_time": total_login_time,
        
        /* Palladium stats */
        "stats": query_all_stats(),
        "hp": query_hp(),
        "max_hp": query_max_hp(),
        "sdc": query_sdc(),
        "max_sdc": query_max_sdc(),
        "mdc": query_mdc(),
        "max_mdc": query_max_mdc(),
        
        /* Race/OCC */
        "race_id": query_race_id(),
        "occ_id": query_occ_id(),
        "race_name": query_race_name(),
        "race_article": query_race_article(),
        
        /* Languages */
        "languages": query_languages(),
        "primary_language": query_primary_language(),
        
        /* Skills */
        "skills": query_all_skills(),
        "primary_skills": query_primary_skills(),
        "secondary_skills": query_secondary_skills(),
        
        /* Psionics */
        "isp": query_isp(),
        "max_isp": query_max_isp(),
        "psionic_powers": query_psionic_powers(),
        "psionic_category": query_psionic_category(),
        
        /* Magic */
        "ppe": query_ppe(),
        "max_ppe": query_max_ppe(),
        "spells_known": query_spells(),
        "spell_caster_level": query_spell_caster_level(),
    ]);
}

void restore_data(mapping data) {
    name = data["name"];
    password_hash = data["password_hash"];
    privilege_level = data["privilege_level"];
    title = data["title"];
    level = data["level"] || 1;
    experience = data["experience"] || 0;
    last_login = data["last_login"];
    total_login_time = data["total_login_time"];
    
    // Attach wiztool if wizard/admin
    if (privilege_level >= 1) {
        attach_wiztool();
    }
    
    /* Palladium stats */
    if (data["stats"]) set_all_stats(data["stats"]);
    if (data["hp"]) set_hp(data["hp"]);
    if (data["max_hp"]) set_max_hp(data["max_hp"]);
    if (data["sdc"]) set_sdc(data["sdc"]);
    if (data["max_sdc"]) set_max_sdc(data["max_sdc"]);
    if (data["mdc"]) set_mdc(data["mdc"]);
    if (data["max_mdc"]) set_max_mdc(data["max_mdc"]);
    
    /* Race/OCC */
    if (data["race_id"]) set_race_id(data["race_id"]);
    if (data["occ_id"]) set_occ_id(data["occ_id"]);
    if (data["race_name"]) set_race_name(data["race_name"]);
    if (data["race_article"]) set_race_article(data["race_article"]);
    
    /* Languages */
    if (data["languages"]) set_languages(data["languages"]);
    if (data["primary_language"]) set_primary_language(data["primary_language"]);
    
    /* Skills */
    if (data["skills"]) set_skills(data["skills"]);
    if (data["primary_skills"]) set_primary_skills(data["primary_skills"]);
    if (data["secondary_skills"]) set_secondary_skills(data["secondary_skills"]);
    
    /* Psionics */
    if (data["isp"]) set_isp(data["isp"]);
    if (data["max_isp"]) set_max_isp(data["max_isp"]);
    if (data["psionic_powers"]) set_psionic_powers(data["psionic_powers"]);
    if (data["psionic_category"]) set_psionic_category(data["psionic_category"]);
    
    /* Magic */
    if (data["ppe"]) set_ppe(data["ppe"]);
    if (data["max_ppe"]) set_max_ppe(data["max_ppe"]);
    if (data["spells_known"]) set_spells(data["spells_known"]);
    if (data["spell_caster_level"]) set_spell_caster_level(data["spell_caster_level"]);
    
    set_name(name);
    set_real_name(name);
    set_short(name + " the " + title);
}

// Cleanup on removal
void remove() {
    if (wiztool) {
        detach_wiztool();
    }
    ::remove();
}
