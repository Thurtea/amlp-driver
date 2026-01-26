// Documentation Room - Technical information

#include <globals.h>

inherit DIR_STD + "/room";

void create() {
    ::create();
    
    set_id("documentation");
    set_short("The Documentation Chamber");
    set_long(
        "A quiet room lined with reference materials and architectural diagrams.\n\n" +
        
        "MUDLIB STRUCTURE:\n" +
        "  /lib/secure/master.c     - Driver callbacks and security\n" +
        "  /lib/single/simul_efun.c - Simulated efuns (utility functions)\n" +
        "  /lib/clone/login.c       - Connection handler\n" +
        "  /lib/clone/user.c        - Player object\n" +
        "  /lib/std/object.c        - Base for all objects\n" +
        "  /lib/std/room.c          - Base for all rooms\n" +
        "  /lib/std/living.c        - Base for NPCs/players\n" +
        "  /lib/cmds/*.c            - Player commands\n\n" +
        
        "AVAILABLE COMMANDS:\n" +
        "  look    - Examine your surroundings\n" +
        "  say     - Speak to others in the room\n" +
        "  go      - Move through exits (go <direction>)\n" +
        "  who     - See who is online\n" +
        "  quit    - Disconnect from the MUD\n" +
        "  shutdown - Stop the server (admin only)\n\n" +
        
        "ADDING NEW CONTENT:\n" +
        "  1. Create rooms in /lib/domains/<your_domain>/room/\n" +
        "  2. Inherit from " + DIR_STD + "/room.c\n" +
        "  3. Define short/long descriptions\n" +
        "  4. Add exits with add_exit(direction, destination_path)\n" +
        "  5. Update your start room or create entry points\n\n" +
        
        "ADDING NEW COMMANDS:\n" +
        "  1. Create file in /lib/cmds/<command_name>.c\n" +
        "  2. Include <globals.h>\n" +
        "  3. Define: int main(string arg)\n" +
        "  4. Use previous_object() to get the player\n" +
        "  5. Return 1 on success\n\n" +
        
        "EXTENDING THE MUDLIB:\n" +
        "  - Add new std/ inheritable objects (item.c, monster.c, etc)\n" +
        "  - Create daemon objects for game systems\n" +
        "  - Expand simul_efun.c with utility functions\n" +
        "  - Modify master.c for security and hooks\n" +
        "  - Build your world in domains/\n"
    );
    
    add_exit("west", DIR_DOMAINS + "/start/room/welcome");
}
