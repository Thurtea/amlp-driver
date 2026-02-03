/*
 * /lib/cmds/language.lpc - Switch your primary speaking language
 * 
 * Usage:
 *   language              - Show current language
 *   language <lang>       - Switch to speaking that language
 */

int cmd(string args) {
    string primary = this_player()->query_primary_language();
    
    if (!args || args == "") {
        /* Show current language */
        write("You are currently speaking " + primary + ".\n");
        write("Use 'languages' to see all languages you know.\n");
        write("Use 'language <name>' to switch languages.\n");
        return 1;
    }
    
    args = lower_case(args);
    
    /* Check if they know this language */
    int prof = this_player()->query_language_proficiency(args);
    
    if (prof <= 0) {
        write("You don't know how to speak " + args + ".\n");
        return 1;
    }
    
    if (args == primary) {
        write("You're already speaking " + args + ".\n");
        return 1;
    }
    
    this_player()->set_primary_language(args);
    
    write("You will now speak in " + args + ".\n");
    
    /* Show proficiency warning if low */
    if (prof < 50) {
        write("Warning: Your proficiency is only " + prof + 
              "%, you may garble words.\n");
    }
    
    return 1;
}

string query_help() {
    return
"LANGUAGE - Switch your speaking language\n\n"
"Usage:\n"
"  language              Show what language you're speaking\n"
"  language <lang>       Switch to speaking that language\n\n"
"When you speak, listeners will hear you in the language you\n"
"are speaking. If they don't understand it well, the text will\n"
"appear garbled to them.\n\n"
"See also: LANGUAGES, SAY\n";
}
