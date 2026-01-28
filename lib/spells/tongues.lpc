/*
 * /lib/spells/tongues.lpc - Tongues spell
 *
 * The critical spell for understanding all languages!
 * Level 6 spell that grants universal language understanding.
 */

inherit "/std/spell";

void create() {
    ::create();
    
    set_spell_id("tongues");
    set_spell_name("Tongues");
    set_spell_desc(
        "One of the most valuable spells for travelers and diplomats, "
        "Tongues grants the caster the ability to understand and speak "
        "any spoken language for the duration of the spell. This includes "
        "all mortal languages, as well as the languages of supernatural "
        "beings (though demons and dragons may notice the magical nature "
        "of the communication). The spell does not grant literacy - the "
        "caster can speak and understand, but cannot read or write in "
        "languages they don't know.\n\n"
        "While under the effect of Tongues, the caster understands all "
        "speech as if spoken in their native language, and their own "
        "speech is automatically understood by all listeners."
    );
    
    set_spell_level(6);
    set_ppe_cost(12);
    
    set_duration_type(2);    /* Per caster level */
    set_base_duration(3);    /* 3 minutes per level */
    
    set_range_type(0);       /* Self */
    set_base_range(0);
    
    set_incantation("Lingua omnium, da mihi auditum!");
    set_requires_verbal(1);
    set_requires_somatic(1);
    set_components(({  }));
    set_casting_time(1);
}

int apply_effect(object caster, object target) {
    if (!caster) return 0;
    
    /* Calculate duration */
    int caster_level = caster->query_spell_caster_level();
    int duration_minutes = 3 * caster_level;
    
    /* Set the tongues property */
    caster->set_property("tongues_active", 1);
    caster->set_property("tongues_duration", duration_minutes);
    
    tell_object(caster,
        "You speak the words of power and feel your mind expand.\n"
        "A warm sensation spreads through your ears and throat.\n"
        "You can now understand and speak all languages!\n"
        "Duration: " + duration_minutes + " minutes.\n");
    
    /* Notify room */
    object env = environment(caster);
    if (env) {
        object *inv = all_inventory(env);
        foreach (object ob : inv) {
            if (ob != caster && living(ob)) {
                tell_object(ob,
                    caster->query_introduction_name(ob) +
                    " speaks arcane words and a shimmer passes over them.\n");
            }
        }
    }
    
    /* Schedule removal - duration is in minutes */
    int seconds = duration_minutes * 60;
    call_out("remove_tongues", seconds, caster);
    
    return 1;
}

/*
 * Called when the spell wears off
 */
void remove_tongues(object caster) {
    if (!caster) return;
    
    if (caster->query_property("tongues_active")) {
        caster->remove_property("tongues_active");
        caster->remove_property("tongues_duration");
        tell_object(caster, "The Tongues spell fades. You can no longer understand all languages.\n");
    }
}
