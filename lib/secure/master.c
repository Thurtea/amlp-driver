// lib/secure/master.c - Master Object with Player Creation

void create() {
    write("Master object loaded.\n");
}

// Called when server starts
void initialize() {
    write("Initializing server systems...\n");
    
    // Load base object system
    load_object("/std/object");
    load_object("/std/living");
    load_object("/std/player");
    
    write("Server initialization complete.\n");
}

// Clone a new object
object clone_object(string path) {
    object ob;
    
    // Load object file and create instance
    catch {
        ob = load_object(path);
        if (ob) {
            ob = clone ob;  // Create instance
            ob->create();   // Call create() on new instance
        }
    };
    
    return ob;
}

// Find player by name
object find_player(string name) {
    object *players = users();
    foreach(object player : players) {
        if (player->query_name() == name) {
            return player;
        }
    }
    return 0;
}

// Shutdown server
void shutdown(int delay) {
    if (delay > 0) {
        call_out("do_shutdown", delay);
    } else {
        do_shutdown();
    }
}

void do_shutdown() {
    write("SERVER SHUTTING DOWN\n");
    // Server will handle actual shutdown
}
