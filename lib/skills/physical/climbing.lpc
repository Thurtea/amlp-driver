/*
 * /lib/skills/physical/climbing.lpc - Climbing skill
 */

inherit "/std/skill";

void create() {
    ::create();
    
    set_skill_id("climbing");
    set_skill_name("Climbing");
    set_skill_desc(
        "The ability to scale walls, cliffs, trees, and other surfaces. "
        "Includes knowledge of proper handholds, ropes, and safety gear. "
        "A failed roll may result in a fall."
    );
    set_skill_category("physical");
    
    set_base_percentage(40);
    set_level_bonus(5);
    set_max_percentage(98);
    
    /* No special requirements */
    set_stat_requirements(([  ]));
    
    /* Rappelling gives a bonus */
    set_related_skills(({ "rappelling" }));
    set_related_bonuses(({ 5 }));
    
    set_training_time(2);
    set_is_primary_only(0);
}
