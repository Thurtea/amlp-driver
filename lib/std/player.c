// Player object

inherit "/std/living";

void create(void) {
    ::create();
}

void net_dead(void) {
    say(query_name() + " has gone link-dead.\n");
}

void reconnect(void) {
    say(query_name() + " has reconnected.\n");
}

void enter_world(void) {
    object start_room;
    
    start_room = load_object("/domains/start/room/void");
    if (start_room) {
        move(start_room);
        receive_message(start_room->look());
    }
}

int process_input(string input) {
    string verb, args;
    object cmd_ob;
    
    if (!input || input == "") return 1;
    
    if (sscanf(input, "%s %s", verb, args) != 2) {
        verb = input;
        args = "";
    }
    
    // Try to execute command
    cmd_ob = load_object("/cmds/" + verb);
    if (cmd_ob) {
        return cmd_ob->execute(args, this_object());
    }
    
    receive_message("What?\n");
    return 1;
}
