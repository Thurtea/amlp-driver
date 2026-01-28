// /lib/occs/operator.lpc
// Operator O.C.C. - Mechanic and Tech Expert
//
// Operators are the mechanics, engineers, and tech wizards of Rifts Earth.
// They keep vehicles running, repair equipment, and can build amazing
// devices from salvage.

inherit "/std/occ";

void create() {
    ::create();
    
    set_occ_name("Operator");
    set_occ_id("operator");
    set_occ_category(OCC_CAT_SCHOLAR);
    set_alignment_requirement("any");
    
    set_occ_description(
        "Operators are the unsung heroes of Rifts Earth - the mechanics, engineers, "
        "and technicians who keep civilization running. While others fight monsters "
        "and cast spells, Operators repair vehicles, maintain power armor, fix "
        "weapons, and build new devices from salvage. A good Operator is worth their "
        "weight in gold to any community or adventuring party. They combine "
        "theoretical knowledge with practical skill, able to diagnose problems, "
        "improvise solutions, and create wonders from scrap. Many Operators run "
        "their own shops or travel with mercenary companies."
    );
    
    // Stat requirements - need intelligence and dexterity
    set_stat_requirements(([
        "iq": 10,   // Intelligence 10+
        "pp": 8     // Physical Prowess 8+
    ]));
    
    // Stat bonuses
    set_stat_bonuses(([
        "iq": 1     // +1 Intelligence from study
    ]));
    
    // Standard level progression
    set_xp_table(XP_LEVELS);
    
    // Base abilities
    set_base_abilities(({
        "jury_rig",           // Quick temporary repairs
        "salvage_expert",     // Find useful parts in junk
        "tool_proficiency"    // Expert with tools
    }));
    
    // Abilities gained at levels
    set_occ_abilities(([
        3: ({ "speed_repair" }),
        5: ({ "build_device" }),
        7: ({ "hotwire_systems" }),
        9: ({ "master_mechanic" }),
        12: ({ "prototype_builder" })
    ]));
    
    // Combat - basic, Operators are not fighters
    set_base_attacks(2);
    set_attacks_per_level(1, 4);
    set_hand_to_hand_type("basic");
    
    // OCC Skills - technical focus
    set_occ_skills(({
        "language_american",
        "literacy_american",
        "mathematics_basic",
        "mathematics_advanced",
        "computer_operation",
        "computer_repair",
        "electrical_engineer",
        "mechanical_engineer",
        "automotive_mechanics",
        "weapons_engineer",
        "robot_mechanics",
        "read_sensory_equipment",
        "pilot_automobile"
    }));
    
    // OCC related skills - more technical options
    set_occ_related_skills(({
        "aircraft_mechanics", "robot_electronics", "robot_mechanics",
        "locksmith", "surveillance", "tv_video", "radio_basic", "radio_scramblers",
        "chemistry", "chemistry_analytical", "physics",
        "pilot_*",  // Any piloting skill
        "sensory_equipment", "optic_systems", "weapon_systems",
        "bioware_mechanics", "cybernetic_medicine",
        "wp_energy_pistol", "wp_energy_rifle", "wp_knife"
    }));
    set_occ_skill_count(8);
    
    // Elective skills - limited combat options
    set_elective_categories(({
        SKILL_CAT_COMMUNICATION, SKILL_CAT_DOMESTIC,
        SKILL_CAT_ELECTRICAL, SKILL_CAT_MECHANICAL,
        SKILL_CAT_PILOT, SKILL_CAT_SCIENCE,
        SKILL_CAT_TECHNICAL
    }));
    set_elective_count(5);
    set_secondary_count(8);
    
    // Excellent skill bonuses for technical work
    set_skill_bonuses(([
        "electrical": 20,
        "mechanical": 20,
        "pilot": 10,
        "science": 10,
        "technical": 15
    ]));
    
    // No magic or psionics
    set_base_ppe(0);
    set_ppe_per_level(0);
    set_base_isp(0);
    set_isp_per_level(0);
    set_magic_type("none");
    set_psionic_type("none");
    
    // Average combat stats
    set_base_hp(0);
    set_hp_per_level(0);
    set_base_sdc(20);
    set_sdc_per_level(0);
    set_mdc_class(0);
    
    // Good equipment - tools are essential
    set_standard_equipment(({
        "/obj/item/backpack",
        "/obj/item/canteen",
        "/obj/item/flashlight",
        "/obj/item/toolkit_basic",
        "/obj/item/toolkit_electrical",
        "/obj/item/multimeter",
        "/obj/item/soldering_iron",
        "/obj/clothing/work_clothes",
        "/obj/item/safety_goggles"
    }));
    
    set_optional_gear(({
        "/obj/item/toolkit_advanced",
        "/obj/item/welding_kit",
        "/obj/item/diagnostic_computer",
        "/obj/vehicle/motorcycle",
        "/obj/vehicle/truck"
    }));
    
    set_starting_credits(500, 2000);  // Operators can earn well
    
    set_starting_weapons(({
        "/obj/weapon/knife",
        "/obj/weapon/energy_pistol"
    }));
    
    set_starting_armor(({
        "/obj/armor/light_body_armor"
    }));
    
    // No race restrictions
    set_race_requirements(({ }));
    set_race_exclusions(({
        "ogre"  // Fine motor skill issues
    }));
    
    // Cybernetics compatible with tech work
    set_cybernetics_allowed(2);  // Full
    set_max_cybernetic_slots(6);
    set_starting_cybernetics(({ }));
    
    // No special powers
    set_special_powers(({ }));
    set_vehicle_type("none");  // But often has one
    set_special_training("");
    
    // Background
    set_background(
        "Operators often learned their trade from family or through apprenticeship. "
        "Some attended surviving technical schools, while others are self-taught "
        "geniuses who figured out how things work by taking them apart. Most "
        "Operators have a specialty - vehicles, weapons, robots, or electronics."
    );
    set_organizations(({ }));
    set_enemies(({ }));
}

// Operators get repair bonuses
int query_repair_bonus() {
    return 15;  // +15% to all repair skills
}

// Operators can jury-rig repairs
int query_jury_rig_bonus() {
    return 20;  // +20% to temporary repairs
}

// Operators can identify tech
int query_identify_tech_bonus() {
    return 25;  // +25% to identify unknown technology
}

// Operators earn more for their services
int query_service_rate_multiplier() {
    return 2;  // 2x standard rate for skilled work
}
