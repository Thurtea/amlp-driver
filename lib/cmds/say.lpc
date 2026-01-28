/*
 * /lib/cmds/say.lpc - Speak to others in the room
 *
 * Supports language system:
 * - Text is garbled based on listener's comprehension
 * - Speaker's name shown based on introduction system
 */

#include <globals.h>

#define LANG_DAEMON "/daemon/languages"

int main(string arg) {
    object *inv;
    object room, user, lang_d;
    string speaker_lang;
    int i;
    
    user = previous_object();
    if (!arg || arg == "") {
        tell_object(user, "Say what?\n");
        return 1;
    }
    
    room = user->query_environment();
    if (!room) {
        tell_object(user, "You are nowhere.\n");
        return 1;
    }
    
    /* Get speaker's language */
    speaker_lang = user->query_primary_language();
    if (!speaker_lang) speaker_lang = "american";
    
    /* Load languages daemon */
    lang_d = find_object(LANG_DAEMON);
    if (!lang_d) {
        lang_d = load_object(LANG_DAEMON);
    }
    
    /* Tell everyone in the room */
    inv = all_inventory(room);
    for (i = 0; i < sizeof(inv); i++) {
        if (inv[i] != user && living(inv[i])) {
            /* Get what name the listener sees */
            string speaker_name = user->query_introduction_name(inv[i]);
            
            /* Garble text based on listener's comprehension */
            string heard_text = arg;
            string lang_note = "";
            
            if (lang_d) {
                heard_text = lang_d->garble_for_listener(
                    user, inv[i], speaker_lang, arg);
                
                /* Add language indicator for non-American */
                if (speaker_lang != "american") {
                    lang_note = " (in " + speaker_lang + ")";
                }
            }
            
            tell_object(inv[i], 
                speaker_name + " says" + lang_note + ": " + heard_text + "\n");
        }
    }
    
    /* Tell the speaker what they said */
    string lang_note = "";
    if (speaker_lang != "american") {
        lang_note = " (in " + speaker_lang + ")";
    }
    tell_object(user, "You say" + lang_note + ": " + arg + "\n");
    
    return 1;
}

string query_help() {
    return
"SAY - Speak to others in the room\n\n"
"Usage:\n"
"  say <message>          Speak to everyone in the room\n"
"  '<message>             Short form of say\n\n"
"You speak in your current primary language. Listeners who\n"
"don't understand that language will hear garbled text.\n\n"
"People who haven't been introduced to you will see your\n"
"race description instead of your name.\n\n"
"See also: LANGUAGE, LANGUAGES, INTRODUCE\n";
}
