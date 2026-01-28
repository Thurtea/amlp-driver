// /lib/occs/cyber_knight.lpc
// Cyber-Knight O.C.C. - Techno-Warrior Champion
//
// Cyber-Knights are an order of techno-warriors dedicated to justice and
// protecting the innocent. They wield psionic psi-swords and follow a
// strict code of honor similar to ancient knights.

inherit "/std/occ";

void create() {
    ::create();
    
    set_occ_name("Cyber-Knight");
    set_occ_id("cyber_knight");
    set_occ_category(OCC_CAT_MILITARY);
    set_alignment_requirement("not_evil");
    
    set_occ_description(
        "Cyber-Knights are an elite order of techno-warriors dedicated to righting "
        "wrongs and protecting the innocent across Rifts Earth. Founded by Lord "
        "Coake, the Cyber-Knights combine psionic powers, advanced cybernetic "
        "enhancement, and rigorous martial training with a strict code of honor "
        "reminiscent of the knights of old. Their signature ability is the psi-sword "
        "- a blade of pure psychic energy that can cut through mega-damage materials. "
        "Cyber-Knights travel the land as wandering champions, opposing tyranny, "
        "evil, and injustice wherever they find it. Many operate independently, "
        "while others serve with the order or assist communities in need."
    );
    
    // Stat requirements - must be capable
    set_stat_requirements(([
        "me": 12,   // Mental Endurance 12+
        "ma": 10,   // Mental Affinity 10+
        "pe": 10,   // Physical Endurance 10+
        "pp": 10    // Physical Prowess 10+
    ]));
    
    // Stat bonuses from training
    set_stat_bonuses(([
        "me": 1,    // +1 Mental Endurance
        "pe": 1,    // +1 Physical Endurance
        "ps": 1     // +1 Physical Strength
    ]));
    
    // Cyber-Knights level faster in combat
    set_xp_table(XP_LEVELS);
    
    // Base abilities
    set_base_abilities(({
        "psi_sword",           // Signature psychic blade
        "psi_shield",          // Psychic defense
        "cyber_armor",         // Cyber-Knight living armor
        "resist_horror",       // Resistance to horror factor
        "sense_evil"           // Detect evil intentions
    }));
    
    // Abilities gained at levels
    set_occ_abilities(([
        2: ({ "zen_combat" }),
        4: ({ "enhanced_psi_sword" }),
        6: ({ "heroic_resolve" }),
        8: ({ "psi_sword_mastery" }),
        10: ({ "champion_aura" }),
        12: ({ "supreme_psi_sword" }),
        15: ({ "legendary_knight" })
    ]));
    
    // Combat - excellent
    set_base_attacks(4);
    set_attacks_per_level(1, 2);  // +1 attack every 2 levels
    set_hand_to_hand_type("martial_arts");
    
    // Combat progression
    set_combat_progression(([
        1: ([ "strike": 2, "parry": 3, "dodge": 2 ]),
        3: ([ "roll": 2, "initiative": 1 ]),
        5: ([ "strike": 1, "parry": 1 ]),
        7: ([ "dodge": 1, "disarm": 2 ]),
        9: ([ "strike": 1, "initiative": 1 ]),
        12: ([ "parry": 1, "dodge": 1, "death_blow": 1 ])
    ]));
    
    // OCC Skills - combat and honor focused
    set_occ_skills(({
        "language_american",
        "literacy_american",
        "language_dragonese",  // Common supernatural language
        "lore_demons_monsters",
        "lore_faeries_creatures",
        "land_navigation",
        "wilderness_survival",
        "horsemanship_general",
        "climbing",
        "swimming",
        "wp_sword",
        "wp_knife",
        "wp_shield",
        "wp_energy_pistol",
        "htoh_martial_arts"
    }));
    
    // OCC related skills
    set_occ_related_skills(({
        "first_aid", "paramedic", "holistic_medicine",
        "pilot_motorcycle", "pilot_automobile", "pilot_hovercycle",
        "pilot_robots_basic", "horsemanship_exotic",
        "tracking", "detect_ambush", "detect_concealment", "intelligence",
        "history", "anthropology", "lore_magic",
        "language_*", "literacy_*"  // Any languages
    }));
    set_occ_skill_count(6);
    
    // Elective skills - honor bound limits
    set_elective_categories(({
        SKILL_CAT_COMMUNICATION, SKILL_CAT_DOMESTIC,
        SKILL_CAT_MEDICAL, SKILL_CAT_PHYSICAL,
        SKILL_CAT_SCIENCE, SKILL_CAT_TECHNICAL,
        SKILL_CAT_WEAPON, SKILL_CAT_WILDERNESS
    }));
    set_elective_count(4);
    set_secondary_count(6);
    
    // Skill bonuses
    set_skill_bonuses(([
        "physical": 10,
        "weapon_proficiencies": 10,
        "wilderness": 5
    ]));
    
    // Psionic abilities - psi-sword focus
    set_base_ppe(0);
    set_ppe_per_level(0);
    set_base_isp(40);         // Good ISP pool
    set_isp_per_level(10);    // +10 per level
    set_magic_type("none");
    set_psionic_type("major");
    
    // Excellent HP and SDC from training
    set_base_hp(0);  // PE + 1d6
    set_hp_per_level(0);  // 1d6
    set_base_sdc(50);
    set_sdc_per_level(2);
    set_mdc_class(0);
    
    // Standard equipment - quality gear
    set_standard_equipment(({
        "/obj/item/backpack",
        "/obj/item/canteen",
        "/obj/item/first_aid_kit",
        "/obj/item/flashlight",
        "/obj/item/rope",
        "/obj/clothing/travelling_clothes",
        "/obj/item/rations_2weeks",
        "/obj/item/tinted_goggles"
    }));
    
    set_optional_gear(({
        "/obj/vehicle/horse",
        "/obj/vehicle/motorcycle",
        "/obj/armor/crusader_armor"
    }));
    
    set_starting_credits(200, 600);
    
    set_starting_weapons(({
        "/obj/weapon/vibro_sword",    // Quality weapon
        "/obj/weapon/knife",
        "/obj/weapon/energy_pistol"
    }));
    
    set_starting_armor(({
        "/obj/armor/cyber_knight_armor"  // Living cyber-armor
    }));
    
    // Race restrictions - must be humanoid, long-lived helps
    set_race_requirements(({ }));
    set_race_exclusions(({ 
        "ogre",           // Too brutish for the code
        "brodkil",
        "gargoyle"
    }));
    
    // Living armor replaces most cybernetics
    set_cybernetics_allowed(1);  // Limited
    set_max_cybernetic_slots(2);
    set_starting_cybernetics(({
        "cyber_knight_armor_link"  // Connection to living armor
    }));
    
    // Special powers
    add_special_power("psi_sword", 1);
    add_special_power("psi_shield", 1);
    add_special_power("sense_evil", 1);
    add_special_power("resist_horror", 1);
    
    set_vehicle_type("none");  // May have horse or cycle
    set_special_training("cyber_knight_code");
    
    // Background
    set_background(
        "Cyber-Knights are carefully selected from promising individuals who "
        "demonstrate courage, honor, and potential. They undergo years of "
        "rigorous training in combat, psionic development, and ethics. The "
        "Cyber-Knight Code demands they protect the innocent, fight injustice, "
        "show mercy to the defeated, and maintain personal honor at all times."
    );
    set_organizations(({ "cyber_knight_order" }));
    set_enemies(({ "coalition_states", "demons", "evil_supernatural" }));
}

// Cyber-Knight Code bonuses
int query_code_bonus() {
    return 2;  // +2 to save vs horror factor when upholding code
}

// Psi-Sword damage increases with level
string query_psi_sword_damage(int level) {
    if (level < 4) return "1d6 MD";
    else if (level < 7) return "2d6 MD";
    else if (level < 10) return "3d6 MD";
    else if (level < 13) return "4d6 MD";
    else return "5d6 MD";
}

// Cyber-Knights inspire others
int query_inspire_bonus() {
    return 1;  // +1 to allies' combat rolls when fighting alongside
}
