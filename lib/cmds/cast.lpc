/*
 * /lib/cmds/cast.lpc - Cast a spell
 * 
 * Usage:
 *   cast                    - List your spells
 *   cast <spell>            - Cast a spell on yourself
 *   cast <spell> <target>   - Cast a spell on a target
 *   cast info <spell>       - Get info about a spell
 */

#define SPELLS_DAEMON "/daemon/spells"

int cmd(string args) {
    object spells_d = find_object(SPELLS_DAEMON);
    if (!spells_d) {
        spells_d = load_object(SPELLS_DAEMON);
    }
    
    /* Check if character is a spell caster */
    int caster_level = this_player()->query_spell_caster_level();
    if (caster_level <= 0) {
        write("You don't know how to cast spells.\n");
        return 1;
    }
    
    if (!args || args == "") {
        /* List spells */
        if (spells_d) {
            write(spells_d->format_spell_list(this_player()));
        } else {
            mapping my_spells = this_player()->query_spells();
            write("Your Spells:\n");
            foreach (string id, int level : my_spells) {
                write("  " + id + " (level " + level + ")\n");
            }
            write("P.P.E.: " + this_player()->query_ppe() + "/" + 
                  this_player()->query_max_ppe() + "\n");
        }
        return 1;
    }
    
    /* Check for info request */
    if (sizeof(args) > 5 && args[0..4] == "info ") {
        string spell_id = lower_case(args[5..]);
        if (spells_d) {
            write(spells_d->query_spell_info(spell_id));
        } else {
            write("Spells daemon not available.\n");
        }
        return 1;
    }
    
    /* Parse spell and optional target */
    string spell_id, target_name;
    object target;
    
    if (sscanf(args, "%s %s", spell_id, target_name) != 2) {
        spell_id = args;
        target_name = "";
    }
    
    spell_id = lower_case(spell_id);
    
    /* Check if player knows this spell */
    if (!this_player()->knows_spell(spell_id)) {
        write("You don't know that spell.\n");
        return 1;
    }
    
    /* Check PPE */
    if (spells_d) {
        object spell = spells_d->query_spell(spell_id);
        if (spell) {
            int cost = spell->query_ppe_cost();
            if (this_player()->query_ppe() < cost) {
                write("You don't have enough P.P.E. You need " + cost + 
                      " but only have " + this_player()->query_ppe() + ".\n");
                return 1;
            }
            
            /* Check spell level vs caster level */
            int spell_level = spell->query_spell_level();
            if (spell_level > caster_level) {
                write("That spell is too advanced for you (level " + 
                      spell_level + ", you are level " + caster_level + ").\n");
                return 1;
            }
        }
    }
    
    /* Find target if specified */
    if (target_name && target_name != "") {
        object env = environment(this_player());
        if (env) {
            target = present(target_name, env);
        }
        if (!target) {
            write("You don't see '" + target_name + "' here.\n");
            return 1;
        }
    } else {
        target = this_player();  /* Default to self */
    }
    
    /* Cast the spell */
    int result;
    if (spells_d) {
        result = spells_d->cast_spell(spell_id, this_player(), target);
    } else {
        write("Spells daemon not available.\n");
        return 1;
    }
    
    return 1;
}

string query_help() {
    return
"CAST - Cast magical spells\n\n"
"Usage:\n"
"  cast                    List your spells and P.P.E.\n"
"  cast <spell>            Cast a spell (on yourself by default)\n"
"  cast <spell> <target>   Cast a spell on a target\n"
"  cast info <spell>       Get detailed info about a spell\n\n"
"Spells cost P.P.E. (Potential Psychic Energy) to cast.\n"
"P.P.E. regenerates over time, and can also be drawn from\n"
"ley lines, nexus points, and other sources.\n\n"
"Spell levels range from 1 to 15. You can only cast spells\n"
"of your caster level or lower.\n";
}
