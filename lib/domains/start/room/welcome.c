// Welcome Hall - Entry point with project information

#include <globals.h>

inherit DIR_STD + "/room";

void create() {
    ::create();
    
    set_id("welcome");
    set_short("The Welcome Hall");
    set_long(
        "You stand in a minimalist hall with clean, simple architecture.\n" +
        "This is AMLP-MUD, a bare-bones LPC mudlib foundation.\n\n" +
        
        "ABOUT THIS PROJECT:\n" +
        "  AMLP-MUD is an intentionally minimal mudlib designed as a starting\n" +
        "  point for building your own LPC-based MUD. It provides only the\n" +
        "  essential components: master object, login system, basic commands,\n" +
        "  and object inheritance hierarchy.\n\n" +
        
        "PHILOSOPHY:\n" +
        "  - NO color codes (add them yourself if desired)\n" +
        "  - NO complex systems (combat, economy, quests)\n" +
        "  - NO unnecessary features\n" +
        "  - JUST the foundation to build upon\n\n" +
        
        "INSPIRED BY:\n" +
        "  This mudlib follows the philosophy of the Lil mudlib - providing\n" +
        "  hardcore do-it-yourselfers with a clean slate.\n"
    );
    
    add_exit("east", DIR_DOMAINS + "/start/room/documentation");
    add_exit("void", DIR_DOMAINS + "/start/room/void");
}
