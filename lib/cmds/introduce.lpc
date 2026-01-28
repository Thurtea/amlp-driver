/*
 * /lib/cmds/introduce.lpc - Introduce yourself to others
 * 
 * Usage:
 *   introduce              - Introduce to everyone in room
 *   introduce <name>       - Introduce to a specific person
 */

int cmd(string args) {
    object env = environment(this_player());
    
    if (!env) {
        write("You're nowhere to introduce yourself.\n");
        return 1;
    }
    
    string my_name = this_player()->query_real_name();
    if (!my_name || my_name == "") {
        my_name = this_player()->query_name();
    }
    
    if (!args || args == "") {
        /* Introduce to everyone in room */
        object *contents = all_inventory(env);
        int count = 0;
        
        foreach (object ob : contents) {
            if (ob != this_player() && living(ob)) {
                this_player()->introduce_to(ob);
                count++;
            }
        }
        
        if (count == 0) {
            write("There's no one here to introduce yourself to.\n");
            return 1;
        }
        
        write("You introduce yourself as " + my_name + ".\n");
        say(this_player()->query_introduction_name(this_player()) + 
            " introduces themselves as " + my_name + ".\n");
        return 1;
    }
    
    /* Introduce to a specific person */
    object target = present(args, env);
    
    if (!target) {
        write("There is no '" + args + "' here.\n");
        return 1;
    }
    
    if (!living(target)) {
        write("You can't introduce yourself to that.\n");
        return 1;
    }
    
    if (target == this_player()) {
        write("You already know yourself.\n");
        return 1;
    }
    
    /* Check if already introduced */
    if (this_player()->query_introduced(target)) {
        string their_name = target->query_real_name();
        if (!their_name) their_name = target->query_name();
        write(their_name + " already knows who you are.\n");
        return 1;
    }
    
    this_player()->introduce_to(target);
    
    write("You introduce yourself to " + 
          target->query_introduction_name(this_player()) + 
          " as " + my_name + ".\n");
    
    tell_object(target, this_player()->query_introduction_name(target) +
                " introduces themselves to you as " + my_name + ".\n");
    
    return 1;
}

string query_help() {
    return
"INTRODUCE - Introduce yourself to others\n\n"
"Usage:\n"
"  introduce              Introduce yourself to everyone in the room\n"
"  introduce <name>       Introduce yourself to a specific person\n\n"
"Until you introduce yourself, others will only see your race\n"
"(e.g., 'a human' or 'an elf') instead of your name.\n";
}
