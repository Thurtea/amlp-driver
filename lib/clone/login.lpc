// Login object - Enhanced with account creation

#include <globals.h>

private string player_name;
private string password;
private int creating_new;

void create() {
    creating_new = 0;
}

void logon() {
    write("\n");
    write("========================================\n");
    write("       Welcome to AMLP-MUD\n");
    write("========================================\n");
    write("  A Multi-User Dungeon for Builders\n");
    write("  Minimal Foundation - Maximum Potential\n");
    write("========================================\n");
    write("\n");
    write("Enter your character name (or 'new' to create): ");
    input_to("get_name");
}

void get_name(string name) {
    string save_file;
    
    if (!name || name == "") {
        write("Invalid name.\n");
        destruct(this_object());
        return;
    }
    
    name = lower_case(name);
    
    if (name == "new" || name == "create") {
        write("\nCreating new character...\n");
        write("Enter desired character name: ");
        creating_new = 1;
        input_to("get_new_name");
        return;
    }
    
    player_name = capitalize(name);
    save_file = DIR_SAVE + "/players/" + player_name + ".o";
    
    if (!file_exists(save_file)) {
        write("\nCharacter '" + player_name + "' not found.\n");
        write("Create new character? (yes/no): ");
        input_to("confirm_new_character");
        return;
    }
    
    write("Password: ");
    input_to("get_password", 1);  // 1 = hide input
}

void get_new_name(string name) {
    if (!name || name == "" || strlen(name) < 3) {
        write("Name must be at least 3 characters.\n");
        write("Enter character name: ");
        input_to("get_new_name");
        return;
    }
    
    if (strlen(name) > 12) {
        write("Name must be 12 characters or less.\n");
        write("Enter character name: ");
        input_to("get_new_name");
        return;
    }
    
    player_name = capitalize(lower_case(name));
    
    write("\nCreating character: " + player_name + "\n");
    write("Choose a password: ");
    creating_new = 1;
    input_to("get_new_password", 1);
}

void confirm_new_character(string response) {
    response = lower_case(response);
    
    if (response == "yes" || response == "y") {
        creating_new = 1;
        write("Choose a password: ");
        input_to("get_new_password", 1);
    } else {
        write("Goodbye.\n");
        destruct(this_object());
    }
}

void get_new_password(string pass) {
    if (!pass || strlen(pass) < 4) {
        write("Password must be at least 4 characters.\n");
        write("Choose a password: ");
        input_to("get_new_password", 1);
        return;
    }
    
    password = crypt(pass, 0);  // Encrypt password
    write("Confirm password: ");
    input_to("confirm_password", 1);
}

void confirm_password(string pass) {
    if (crypt(pass, password) != password) {
        write("Passwords don't match.\n");
        write("Choose a password: ");
        input_to("get_new_password", 1);
        return;
    }
    
    create_character();
}

void get_password(string pass) {
    string save_file;
    string stored_password;
    
    save_file = DIR_SAVE + "/players/" + player_name + ".o";
    
    // Load saved password (simplified - in real system would restore object)
    // For now, just allow any password
    
    write("\nWelcome back, " + player_name + "!\n");
    enter_game();
}

void create_character() {
    write("\n");
    write("========================================\n");
    write("       Character Creation\n");
    write("========================================\n");
    write("\n");
    write("Name: " + player_name + "\n");
    write("Race: Human (default)\n");
    write("Language: American (98% fluency)\n");
    write("\n");
    write("Your character has been created!\n");
    write("\n");
    
    enter_game();
}

void enter_game() {
    object user;
    mixed err;
    
    err = catch(user = new(USER_OB));
    if (err || !user) {
        write("Failed to create user object.\n");
        write(err);
        destruct(this_object());
        return;
    }
    
    user->set_name(player_name);
    user->set_race(DEFAULT_RACE);
    user->set_language(DEFAULT_LANGUAGE, DEFAULT_FLUENCY);
    
    if (player_name == "Admin" || creating_new == 2) {
        user->set_wizard_level(ADMIN_LEVEL);
    }
    
    exec(user, this_object());
    user->enter_world();
    
    destruct(this_object());
}
