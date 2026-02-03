/*
 * /lib/skills/physical/swimming.lpc - Swimming skill
 */

inherit "/std/skill";

void create() {
    ::create();
    
    set_skill_id("swimming");
    set_skill_name("Swimming");
    set_skill_desc(
        "The ability to swim and stay afloat. Characters without this skill "
        "can only dog paddle and may drown in rough water. Includes "
        "knowledge of currents, diving, and water safety."
    );
    set_skill_category("physical");
    
    set_base_percentage(50);
    set_level_bonus(5);
    set_max_percentage(98);
    
    set_stat_requirements(([  ]));
    set_related_skills(({ "scuba" }));
    set_related_bonuses(({ 5 }));
    
    set_training_time(2);
    set_is_primary_only(0);
}
