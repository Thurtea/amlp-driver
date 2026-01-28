/*
 * /lib/psionics/super/mind_block.lpc - Mind Block
 *
 * Mental defense power
 */

inherit "/std/psionic";

void create() {
    ::create();
    
    set_power_id("mind_block");
    set_power_name("Mind Block");
    set_power_desc(
        "The psychic can erect a mental barrier that prevents other "
        "psychics from reading their thoughts, sensing their emotions, "
        "or psionically detecting them. While the mind block is active, "
        "the character cannot use their own psionic powers. The block "
        "can be maintained indefinitely at no additional ISP cost."
    );
    set_power_category("super");
    
    set_isp_cost(4);
    set_duration_type(1);    /* Per minute - but can be held indefinitely */
    set_base_duration(0);    /* Special: maintained until dropped */
    set_range_type(0);       /* Self */
    set_base_range(0);
}

int apply_effect(object user, object target) {
    /* Check if already blocking */
    if (user->query_property("mind_block_active")) {
        user->remove_property("mind_block_active");
        tell_object(user, "You lower your mental defenses.\n");
        return 1;
    }
    
    user->set_property("mind_block_active", 1);
    tell_object(user,
        "You erect a powerful mental barrier, shielding your thoughts.\n"
        "While maintaining the block, you cannot use other psionic powers.\n");
    
    return 1;
}
