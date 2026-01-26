// User object - Enhanced with race, language, wizard system

#include <globals.h>

inherit DIR_STD + "/living";

private string race;
private mapping languages;
private int wizard_level;

void create() {
    ::create();
    set_living_name("user");
    race = DEFAULT_RACE;
    languages = ([ DEFAULT_LANGUAGE : DEFAULT_FLUENCY ]);
    wizard_level = PLAYER_LEVEL;
}

void set_race(string r) {
    race = r;
}

string query_race() {
    return race;
}

void set_language(string lang, int fluency) {
    if (!languages) languages = ([]);
    languages[lang] = fluency;
}

int query_language(string lang) {
    if (!languages) return 0;
    return languages[lang];
}

mapping query_all_languages() {
    return languages ? copy(languages) : ([]);
}

void set_wizard_level(int level) {
    wizard_level = level;
}

int query_wizard_level() {
    return wizard_level;
}

string query_title() {
    switch(wizard_level) {
        case ADMIN_LEVEL:
            return "[Admin] ";
        case WIZARD_LEVEL:
            return "[Wizard] ";
        default:
            return "";
    }
}

void enter_world() {
    object room;
    
    write("\n");
    if (wizard_level >= WIZARD_LEVEL) {
        write("=== ADMINISTRATOR ACCESS GRANTED ===\n");
        write("Use 'wiz' to see wizard commands.\n");
        write("\n");
    }
    write("Entering the world...\n\n");
    
    room = load_object(START_ROOM);
    if (!room) {
        write("ERROR: Start room not found!\n");
        destruct(this_object());
        return;
    }
    
    move(room);
    force_me("look");
}

void net_dead() {
    if (query_environment()) {
        tell_room(query_environment(), 
            query_title() + query_name() + " goes link-dead.\n", 
            ({this_object()}));
    }
}

void reconnect() {
    if (query_environment()) {
        tell_room(query_environment(), 
            query_title() + query_name() + " reconnects.\n", 
            ({this_object()}));
    }
}

int process_input(string input) {
    string verb, args;
    object cmd_ob;
    string cmd_path;
    
    if (!input || input == "") return 1;
    
    // Parse input
    if (sscanf(input, "%s %s", verb, args) != 2) {
        verb = input;
        args = "";
    }
    
    verb = lower_case(verb);
    
    // Try admin commands first if wizard
    if (wizard_level >= WIZARD_LEVEL) {
        cmd_path = ADMIN_CMD_PREFIX + verb;
        catch(cmd_ob = load_object(cmd_path));
        if (cmd_ob) {
            return cmd_ob->main(args);
        }
    }
    
    // Try regular commands
    cmd_path = CMD_PREFIX + verb;
    catch(cmd_ob = load_object(cmd_path));
    
    if (cmd_ob) {
        return cmd_ob->main(args);
    }
    
    write("What?\n");
    return 1;
}

void remove() {
    if (query_environment()) {
        tell_room(query_environment(), 
            query_title() + query_name() + " leaves this reality.\n", 
            ({this_object()}));
    }
    destruct(this_object());
}

void save_me() {
    string save_file;
    save_file = DIR_SAVE + "/players/" + query_name() + ".o";
    save_object(save_file);
}

void restore_me() {
    string save_file;
    save_file = DIR_SAVE + "/players/" + query_name() + ".o";
    restore_object(save_file);
}
