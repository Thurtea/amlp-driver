/*
 * /lib/cmds/forget.lpc - Forget someone's introduction
 * 
 * Usage:
 *   forget <name>        - Forget this person's name
 */

int cmd(string args) {
    object env = environment(this_player());
    
    if (!args || args == "") {
        write("Forget who?\n");
        write("Usage: forget <name>\n");
        return 1;
    }
    
    if (!env) {
        write("You're nowhere.\n");
        return 1;
    }
    
    object target = present(args, env);
    
    if (!target) {
        write("There is no '" + args + "' here.\n");
        return 1;
    }
    
    if (!living(target)) {
        write("That's not a living being.\n");
        return 1;
    }
    
    if (target == this_player()) {
        write("You can't forget yourself. Trust me, you've tried.\n");
        return 1;
    }
    
    /* Get their name before we forget */
    string their_name;
    if (target->query_introduced(this_player())) {
        their_name = target->query_real_name();
        if (!their_name) their_name = target->query_name();
    } else {
        write("You don't know who " + 
              target->query_introduction_name(this_player()) + 
              " is anyway.\n");
        return 1;
    }
    
    /* Make them forget us (we're forgetting that they introduced to us) */
    target->forget_introduction(this_player());
    
    write("You forget " + their_name + "'s name. They will appear as '" +
          target->query_race_description() + "' to you now.\n");
    
    return 1;
}

string query_help() {
    return
"FORGET - Forget someone's introduction\n\n"
"Usage:\n"
"  forget <name>        Forget someone's name\n\n"
"Use this to forget someone's name. They will need to\n"
"introduce themselves again for you to know who they are.\n"
"Useful for roleplay scenarios.\n";
}
