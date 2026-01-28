// The Void - Testing room

#include <globals.h>

inherit DIR_STD + "/room";

void create() {
    ::create();
    
    set_id("void");
    set_short("The Void");
    set_long(
        "You are floating in an endless void of possibility.\n" +
        "This empty space represents the blank canvas upon which you will\n" +
        "build your MUD. There is nothing here yet - but there will be.\n"
    );
    
    add_exit("welcome", DIR_DOMAINS + "/start/room/welcome");
}
