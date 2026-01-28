/*
 * /lib/psionics/healing/bio_regeneration.lpc - Bio-Regeneration
 *
 * Classic Palladium self-healing power
 */

inherit "/std/psionic";

void create() {
    ::create();
    
    set_power_id("bio_regeneration");
    set_power_name("Bio-Regeneration");
    set_power_desc(
        "The psychic can channel their inner strength to accelerate their "
        "body's natural healing process. The character enters a trance-like "
        "state and regenerates 2d6 hit points and 3d6 S.D.C. The process "
        "takes about 10 minutes and leaves the character somewhat fatigued."
    );
    set_power_category("healing");
    
    set_isp_cost(6);
    set_duration_type(1);   /* Minutes */
    set_base_duration(10);
    set_range_type(0);      /* Self */
    set_base_range(0);
}

int apply_effect(object user, object target) {
    /* Calculate healing */
    int hp_heal = random(6) + random(6) + 2;   /* 2d6 */
    int sdc_heal = random(6) + random(6) + random(6) + 3;  /* 3d6 */
    
    int old_hp = user->query_hp();
    int old_sdc = user->query_sdc();
    
    user->heal(hp_heal);
    user->set_sdc(user->query_sdc() + sdc_heal);
    
    int new_hp = user->query_hp();
    int new_sdc = user->query_sdc();
    
    if (new_sdc > user->query_max_sdc()) {
        user->set_sdc(user->query_max_sdc());
        new_sdc = user->query_max_sdc();
    }
    
    tell_object(user,
        "You enter a healing trance...\n"
        "Warmth spreads through your body as your wounds begin to close.\n"
        "You heal " + (new_hp - old_hp) + " HP and " + 
        (new_sdc - old_sdc) + " S.D.C.\n");
    
    /* Notify room */
    object env = environment(user);
    if (env) {
        object *inv = all_inventory(env);
        foreach (object ob : inv) {
            if (ob != user && living(ob)) {
                tell_object(ob, 
                    user->query_introduction_name(ob) + 
                    " enters a trance, their wounds slowly healing.\n");
            }
        }
    }
    
    return 1;
}
