/*
 * /lib/psionics/physical/telekinesis.lpc - Telekinesis
 *
 * Classic mind-over-matter power
 */

inherit "/std/psionic";

void create() {
    ::create();
    
    set_power_id("telekinesis");
    set_power_name("Telekinesis");
    set_power_desc(
        "The ability to move objects with the power of the mind. The "
        "psychic can lift and manipulate objects up to 60 lbs at level 1, "
        "+10 lbs per additional level. Objects can be moved at a rate of "
        "up to 10 feet per melee round (about walking speed). Fine "
        "manipulation (like untying knots) requires intense concentration."
    );
    set_power_category("physical");
    
    set_isp_cost(3);         /* varies by weight */
    set_duration_type(1);    /* Per minute */
    set_base_duration(2);
    set_range_type(2);       /* Ranged */
    set_base_range(60);      /* 60 feet */
}

/*
 * Calculate max weight based on level
 */
int query_max_weight(int level) {
    return 60 + ((level - 1) * 10);
}

int apply_effect(object user, object target) {
    if (!target) {
        tell_object(user, "You focus your telekinetic power, ready to move something.\n");
        return 1;
    }
    
    tell_object(user, 
        "You focus your mind on " + target->query_short() + 
        " and feel your telekinetic grip take hold.\n");
    
    object env = environment(user);
    if (env) {
        object *inv = all_inventory(env);
        foreach (object ob : inv) {
            if (ob != user && living(ob)) {
                string target_name = target->query_short();
                if (!target_name) target_name = "something";
                tell_object(ob,
                    user->query_introduction_name(ob) + 
                    " concentrates intently and " + target_name +
                    " begins to float.\n");
            }
        }
    }
    
    return 1;
}
