/*
 * /lib/psionics/sensitive/sixth_sense.lpc - Sixth Sense
 *
 * Classic Palladium warning power
 */

inherit "/std/psionic";

void create() {
    ::create();
    
    set_power_id("sixth_sense");
    set_power_name("Sixth Sense");
    set_power_desc(
        "This power gives the psychic a precognitive flash of impending "
        "danger. It cannot be called upon at will, but rather, activates "
        "automatically when there is imminent danger to the psychic or "
        "someone nearby. The warning comes as a sudden flash of insight "
        "or gut feeling that something is wrong."
    );
    set_power_category("sensitive");
    
    set_isp_cost(2);
    set_duration_type(0);  /* Instant warning */
    set_base_duration(0);
    set_range_type(0);     /* Self */
    set_base_range(0);
}

/*
 * This power activates automatically, so apply_effect
 * is called by combat/trap systems
 */
int apply_effect(object user, object target) {
    tell_object(user, 
        "A sudden chill runs down your spine. Something is wrong...\n");
    return 1;
}
