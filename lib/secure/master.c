// AMLP Master Object

#include <globals.h>

void create() {
    write("Master object initialized\n");
}

object connect() {
    object login_ob;
    mixed err;
    
    err = catch(login_ob = new(LOGIN_OB));
    if (err) {
        write("Failed to create login object.\n");
        write(err);
        return 0;
    }
    return login_ob;
}

mixed compile_object(string filename) {
    // Return the object to associate with this filename
    return filename;
}

void log_error(string file, string message) {
    string logfile;
    logfile = DIR_LOG + "/errors";
    write_file(logfile, file + ": " + message + "\n");
}

string query_version() {
    return "AMLP-MUD 1.0";
}

string query_mud_name() {
    return "AMLP-MUD";
}

string get_root_uid() {
    return ROOT_UID;
}

string creator_file(string file) {
    return ROOT_UID;
}

int valid_read(string file, object ob) {
    return 1;  // Allow all reads for now
}

int valid_write(string file, object ob) {
    return 1;  // Allow all writes for now
}

void crash(string error, object command_giver, object current_object) {
    log_error("CRASH", error);
}

void epilog(object ob) {
    // Called after object creation
}

void prolog(string file) {
    // Called before object compilation
}
