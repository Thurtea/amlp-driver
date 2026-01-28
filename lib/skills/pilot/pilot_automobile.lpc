/*
 * /lib/skills/pilot/pilot_automobile.lpc - Automobile piloting
 */

inherit "/std/skill";

void create() {
    ::create();
    
    set_skill_id("pilot_automobile");
    set_skill_name("Pilot Automobile");
    set_skill_desc(
        "The ability to operate standard ground vehicles including cars, "
        "trucks, and motorcycles. Includes knowledge of traffic laws, "
        "emergency maneuvers, and basic maintenance. Most people have "
        "this skill even in the post-apocalyptic world."
    );
    set_skill_category("pilot");
    
    set_base_percentage(60);
    set_level_bonus(2);
    set_max_percentage(98);
    
    set_stat_requirements(([  ]));
    
    /* Automotive mechanics helps */
    set_related_skills(({ "automotive_mechanics" }));
    set_related_bonuses(({ 5 }));
    
    set_training_time(2);
    set_is_primary_only(0);
}
