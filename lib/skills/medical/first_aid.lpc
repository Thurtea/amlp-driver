/*
 * /lib/skills/medical/first_aid.lpc - First Aid skill
 */

inherit "/std/skill";

void create() {
    ::create();
    
    set_skill_id("first_aid");
    set_skill_name("First Aid");
    set_skill_desc(
        "Basic emergency medical treatment including cleaning wounds, "
        "applying bandages, treating shock, and stabilizing patients. "
        "A successful roll can heal 1d6 S.D.C. or 1d4 Hit Points, or "
        "stabilize a dying patient."
    );
    set_skill_category("medical");
    
    set_base_percentage(45);
    set_level_bonus(5);
    set_max_percentage(98);
    
    set_stat_requirements(([  ]));
    
    set_training_time(4);
    set_is_primary_only(0);
}

/* First aid healing amounts */
int query_sdc_heal() {
    return random(6) + 1;  /* 1d6 */
}

int query_hp_heal() {
    return random(4) + 1;  /* 1d4 */
}
