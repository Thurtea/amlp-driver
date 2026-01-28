/*
 * /lib/cmds/manifest.lpc - Use psionic powers
 * 
 * Usage:
 *   manifest                    - List your psionic powers
 *   manifest <power>            - Use a power on yourself
 *   manifest <power> <target>   - Use a power on a target
 *   manifest info <power>       - Get info about a power
 */

#define PSI_DAEMON "/daemon/psionics"

int cmd(string args) {
    object psi_d = find_object(PSI_DAEMON);
    if (!psi_d) {
        psi_d = load_object(PSI_DAEMON);
    }
    
    /* Check if character is psionic */
    string psi_cat = this_player()->query_psionic_category();
    if (!psi_cat || psi_cat == "none") {
        write("You have no psionic abilities.\n");
        return 1;
    }
    
    /* Check for mind block */
    if (this_player()->query_property("mind_block_active") && 
        args && args != "" && lower_case(args[0..9]) != "mind_block") {
        write("You cannot use psionic powers while maintaining a mind block.\n");
        write("Use 'manifest mind_block' to lower your defenses first.\n");
        return 1;
    }
    
    if (!args || args == "") {
        /* List powers */
        if (psi_d) {
            write(psi_d->format_power_list(this_player()));
        } else {
            string *powers = this_player()->query_psionic_powers();
            write("Your Psionic Powers:\n");
            foreach (string p : powers) {
                write("  " + p + "\n");
            }
            write("ISP: " + this_player()->query_isp() + "/" + 
                  this_player()->query_max_isp() + "\n");
        }
        return 1;
    }
    
    /* Check for info request */
    if (sizeof(args) > 5 && args[0..4] == "info ") {
        string power_id = lower_case(args[5..]);
        if (psi_d) {
            write(psi_d->query_power_info(power_id));
        } else {
            write("Psionics daemon not available.\n");
        }
        return 1;
    }
    
    /* Parse power and optional target */
    string power_id, target_name;
    object target;
    
    if (sscanf(args, "%s %s", power_id, target_name) != 2) {
        power_id = args;
        target_name = "";
    }
    
    power_id = lower_case(power_id);
    
    /* Check if player knows this power */
    if (!this_player()->knows_power(power_id)) {
        write("You don't know that psionic power.\n");
        return 1;
    }
    
    /* Check ISP */
    if (psi_d) {
        object power = psi_d->query_power(power_id);
        if (power) {
            int cost = power->query_isp_cost();
            if (this_player()->query_isp() < cost) {
                write("You don't have enough ISP. You need " + cost + 
                      " but only have " + this_player()->query_isp() + ".\n");
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
    
    /* Activate the power */
    int result;
    if (psi_d) {
        result = psi_d->activate_power(power_id, this_player(), target);
    } else {
        write("Psionics daemon not available.\n");
        return 1;
    }
    
    return 1;
}

string query_help() {
    return
"MANIFEST - Use psionic powers\n\n"
"Usage:\n"
"  manifest                    List your psionic powers and ISP\n"
"  manifest <power>            Use a power (on yourself by default)\n"
"  manifest <power> <target>   Use a power on a target\n"
"  manifest info <power>       Get detailed info about a power\n\n"
"Psionic powers cost ISP (Inner Strength Points) to use.\n"
"ISP regenerates over time at a rate based on your ME stat.\n\n"
"Categories:\n"
"  Sensitive - Detection and perception powers\n"
"  Physical  - Mind over matter\n"
"  Healing   - Recovery and restoration\n"
"  Super     - Most powerful abilities\n";
}
