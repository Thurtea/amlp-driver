// /lib/secure/master.c
// Master Object - Core LPC driver interface

#include <globals.h>

// State
private mapping loaded_daemons;

void create() {
    loaded_daemons = ([]);
    
    // Initialize the MUD
    write("Master object initializing...\n");
    
    // Step 1: Load simul_efun first (provides additional efuns)
    load_simul_efun();
    
    // Step 2: Initialize core daemons
    init_daemons();
    
    // Step 3: Set up directories
    init_directories();

    /* Test: attempt to load a small test clone to exercise compile/load/clone
     * This helps validate that clone_object/load_object and the compiler
     * + program_loader path work at runtime. Remove when no longer needed. */
    catch {
        object t = load_object("/test/clone_test");
        if (t) {
            write("[MASTER TEST] Loaded test clone: " + file_name(t) + "\n");
        } else {
            write("[MASTER TEST] Failed to load test clone.\n");
        }
    };
    
    write("Master object initialized successfully.\n");
}

// Load simulated efuns
private void load_simul_efun() {
    object simul;
    
    catch {
        simul = load_object("/secure/simul_efun");
    };
    
    if (simul) {
        write("Simul_efun loaded.\n");
    } else {
        write("WARNING: Simul_efun failed to load.\n");
    }
}

// Initialize core daemons
private void init_daemons() {
    string *daemon_list = ({
        "/daemon/command",
        "/daemon/login",
        "/daemon/chargen",
        "/daemon/skills",
        "/daemon/languages",
        "/daemon/psionics",
        "/daemon/spells",
    });
    
    foreach (string daemon in daemon_list) {
        object d;
        
        catch {
            d = load_object(daemon);
        };
        
        if (d) {
            loaded_daemons[daemon] = d;
            write("  Loaded: " + daemon + "\n");
        } else {
            write("  WARNING: Failed to load " + daemon + "\n");
        }
    }
}

// Create necessary directories
private void init_directories() {
    string *dirs = ({
        "/save",
        "/save/players",
        "/log",
    });
    
    foreach (string dir in dirs) {
        if (file_size(dir) != -2) {
            mkdir(dir);
        }
    }
}

// === DRIVER APPLIES (REQUIRED) ===

// Called when a user connects
object connect() {
    object login;
    
    catch {
        login = clone_object("/clone/login");
    };
    
    if (!login) {
        write("ERROR: Failed to clone login object!\n");
        return 0;
    }
    
    return login;
}

// Handle compilation/runtime errors
void flag_error(string err) {
    string logfile = "/log/errors.log";
    
    write("COMPILATION ERROR:\n" + err + "\n");
    
    // Log to file
    write_file(logfile, ctime(time()) + "\n" + err + "\n\n");
}

// Get root UID (for security system)
string get_root_uid() {
    return "Root";
}

// Get backbone UID
string get_bb_uid() {
    return "Backbone";
}

// Return creator of a file (for security)
string creator_file(string file) {
    string *parts;
    
    if (!file || file == "") return "Root";
    
    // Remove leading slash
    if (file[0] == /) {
        file = file[1..];
    }
    
    parts = explode(file, "/");
    
    // Check if its
