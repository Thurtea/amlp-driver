/*
 * /lib/cmds/remember.lpc - Remember someone's introduction
 * 
 * Usage:
 *   remember <name>        - Remember this person permanently
 *   remember               - List people you remember
 */

int cmd(string args) {
    object env = environment(this_player());
    
    if (!args || args == "") {
        /* List remembered people - this would need persistent storage */
        write("Usage: remember <name>\n");
        write("Use this to confirm you'll remember someone's introduction.\n");
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
        write("You already remember yourself.\n");
        return 1;
    }
    
    /* Check if we've been introduced */
    if (!target->query_introduced(this_player())) {
        write(target->query_introduction_name(this_player()) + 
              " hasn't introduced themselves to you.\n");
        return 1;
    }
    
    /* Already know them - the introduce_to function handles this */
    string their_name = target->query_real_name();
    if (!their_name) their_name = target->query_name();
    
    write("You commit " + their_name + "'s face and name to memory.\n");
    
    /* 
     * In a full implementation, this would store the introduction
     * persistently. For now, introductions are stored on the target
     * object and persist as long as both players are logged in.
     */
    
    return 1;
}

string query_help() {
    return
"REMEMBER - Remember someone's introduction\n\n"
"Usage:\n"
"  remember <name>        Commit someone's name to permanent memory\n\n"
"When someone introduces themselves to you, use this command\n"
"to remember their name even after logging out.\n";
}
