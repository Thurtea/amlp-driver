/*
 * /lib/skills/weapons/wp_knife.lpc - Weapon Proficiency: Knife
 */

inherit "/std/skill";

void create() {
    ::create();
    
    set_skill_id("wp_knife");
    set_skill_name("W.P. Knife");
    set_skill_desc(
        "Training in the use of knives, daggers, and similar small bladed "
        "weapons. Includes combat techniques, throwing, and maintenance. "
        "Provides +1 to strike at level 1, and additional bonuses as "
        "the skill improves."
    );
    set_skill_category("weapon");
    
    set_base_percentage(0);  /* WP skills work differently */
    set_level_bonus(0);
    set_max_percentage(0);
    
    set_stat_requirements(([  ]));
    
    set_training_time(4);
    set_is_primary_only(0);
}

/* WP skills provide strike/parry bonuses instead of percentage */
int query_strike_bonus(int level) {
    if (level < 1) return 0;
    return 1 + (level / 3);  /* +1 at 1, +2 at 3, +3 at 6, etc. */
}

int query_parry_bonus(int level) {
    if (level < 1) return 0;
    return 1 + (level / 4);  /* +1 at 1, +2 at 4, +3 at 8, etc. */
}

int query_throw_bonus(int level) {
    if (level < 1) return 0;
    return level / 2;  /* +1 at 2, +2 at 4, etc. */
}
