// /lib/occs/juicer.lpc
// Juicer O.C.C. - Chemical Super-Soldier
//
// Juicers are augmented super-soldiers enhanced by a chemical drug
// delivery system. They possess superhuman speed, strength, and reflexes,
// but the chemicals burn out their bodies, limiting them to 6-8 years of life.

inherit "/std/occ";

void create() {
    ::create();
    
    set_occ_name("Juicer");
    set_occ_id("juicer");
    set_occ_category(OCC_CAT_MILITARY);
    set_alignment_requirement("any");
    
    set_occ_description(
        "Juicers are chemical super-soldiers, humans enhanced by a Bio-Comp "
        "drug harness that continuously pumps performance-enhancing chemicals "
        "into their bodies. The result is superhuman strength, speed, and reflexes "
        "- but at a terrible price. The chemicals burn out the Juicer's body, giving "
        "them only five to eight years to live after conversion. This death sentence "
        "attracts those with nothing to lose - the desperate, the thrill-seekers, "
        "and those seeking one last chance at glory. Juicers live fast and die young, "
        "often seeking death in battle rather than the slow deterioration that awaits. "
        "Some seek a cure, though none has been found. Others simply embrace the ride."
    );
    
    // Stat requirements - must be physically capable
    set_stat_requirements(([
        "pe": 12,   // Physical Endurance 12+
        "ps": 10,   // Physical Strength 10+
        "pp": 10    // Physical Prowess 10+
    ]));
    
    // MASSIVE stat bonuses from chemical enhancement
    set_stat_bonuses(([
        "ps": 10,   // +10 Physical Strength (supernatural)
        "pp": 6,    // +6 Physical Prowess
        "pe": 8,    // +8 Physical Endurance
        "spd": 10,  // +10 Speed (superhuman)
        "me": -2    // -2 Mental Endurance (chemical instability)
    ]));
    
    // Juicers level fast (short lifespan)
    set_xp_table(XP_LEVELS);
    
    // Base abilities - chemical enhancements
    set_base_abilities(({
        "enhanced_strength",     // Supernatural PS
        "enhanced_speed",        // Superhuman SPD
        "enhanced_endurance",    // Superhuman PE
        "rapid_healing",         // Heal 4x faster
        "drug_immunity",         // Immune to drugs/alcohol
        "increased_reaction",    // +6 initiative
        "auto_parry",            // Automatic parry without penalty
        "heightened_senses"      // Enhanced perception
    }));
    
    // Abilities gained at levels
    set_occ_abilities(([
        2: ({ "combat_frenzy" }),
        3: ({ "resist_pain" }),
        4: ({ "adrenaline_surge" }),
        5: ({ "lightning_reflexes" }),
        6: ({ "juicer_rage" }),
        7: ({ "death_wish" })
    ]));
    
    // Combat - EXCEPTIONAL
    set_base_attacks(6);
    set_attacks_per_level(1, 1);  // +1 attack EVERY level
    set_hand_to_hand_type("martial_arts");
    
    // Combat progression - rapid advancement
    set_combat_progression(([
        1: ([ "strike": 3, "parry": 3, "dodge": 4, "initiative": 6, "roll": 3 ]),
        2: ([ "strike": 1, "damage": 2 ]),
        3: ([ "dodge": 1, "pull_punch": 2 ]),
        4: ([ "strike": 1, "parry": 1 ]),
        5: ([ "initiative": 2, "disarm": 2 ]),
        6: ([ "strike": 1, "dodge": 1, "death_blow": 1 ]),
        7: ([ "paired_weapons": 1, "knock_out": 1 ])
    ]));
    
    // OCC Skills - combat focused
    set_occ_skills(({
        "language_american",
        "athletics",
        "boxing",
        "climbing",
        "running",
        "swimming",
        "acrobatics",
        "pilot_automobile",
        "pilot_motorcycle",
        "wp_knife",
        "wp_sword",
        "wp_energy_pistol",
        "wp_energy_rifle",
        "htoh_martial_arts"
    }));
    
    // OCC related skills - physical and combat
    set_occ_related_skills(({
        "gymnastics", "wrestling", "prowl",
        "detect_ambush", "detect_concealment", "tracking",
        "wilderness_survival", "land_navigation",
        "pilot_*",  // Any piloting skill
        "wp_*"      // Any weapon proficiency
    }));
    set_occ_skill_count(6);
    
    // Elective skills - limited to physical
    set_elective_categories(({
        SKILL_CAT_PHYSICAL, SKILL_CAT_PILOT,
        SKILL_CAT_WEAPON, SKILL_CAT_WILDERNESS
    }));
    set_elective_count(4);
    set_secondary_count(6);
    
    // Physical skill bonuses
    set_skill_bonuses(([
        "physical": 15,
        "pilot": 10,
        "weapon_proficiencies": 10
    ]));
    
    // No magic potential (chemicals interfere)
    set_base_ppe(0);
    set_ppe_per_level(0);
    set_base_isp(0);
    set_isp_per_level(0);
    set_magic_type("none");
    set_psionic_type("none");
    
    // EXCEPTIONAL HP and SDC from conditioning
    set_base_hp(0);  // PE + 1d6 (but PE is enhanced)
    set_hp_per_level(0);  // 2d6 per level
    set_base_sdc(80);     // High base SDC
    set_sdc_per_level(4); // +4 per level
    set_mdc_class(0);
    
    // Standard combat equipment
    set_standard_equipment(({
        "/obj/item/backpack",
        "/obj/item/canteen",
        "/obj/item/first_aid_kit",
        "/obj/item/flashlight",
        "/obj/clothing/travelling_clothes",
        "/obj/item/bio_comp_harness"  // The drug delivery system
    }));
    
    set_optional_gear(({
        "/obj/armor/juicer_armor",
        "/obj/vehicle/motorcycle",
        "/obj/weapon/vibro_sword"
    }));
    
    set_starting_credits(200, 800);
    
    set_starting_weapons(({
        "/obj/weapon/vibro_knife",
        "/obj/weapon/energy_pistol",
        "/obj/weapon/energy_rifle"
    }));
    
    set_starting_armor(({
        "/obj/armor/juicer_light_armor"
    }));
    
    // Race restrictions - humans and humanoids only
    set_race_requirements(({
        "human", "elf", "dwarf"
    }));
    set_race_exclusions(({
        "ogre", "troll", "dragon",
        "demon", "god", "supernatural"
    }));
    
    // Limited cybernetics - bio-comp is primary system
    set_cybernetics_allowed(1);  // Limited
    set_max_cybernetic_slots(2);
    set_starting_cybernetics(({
        "bio_comp_harness",          // Drug delivery system
        "bio_comp_reflex_chip"       // Reflex enhancement
    }));
    
    // Special powers from chemical enhancement
    add_special_power("enhanced_strength", 1);
    add_special_power("enhanced_speed", 1);
    add_special_power("rapid_healing", 1);
    add_special_power("auto_parry", 1);
    
    set_vehicle_type("none");  // Often has motorcycle
    set_special_training("juicer_combat");
    
    // Background
    set_background(
        "Juicers come from all walks of life, but most share one thing: desperation. "
        "Some are former soldiers seeking an edge, others are criminals looking for "
        "power, and some are simply people with nothing left to lose. The conversion "
        "process is expensive and dangerous, offered by black market clinics and "
        "some legitimate organizations. Once converted, Juicers must maintain their "
        "chemical supply or face painful withdrawal and death."
    );
    set_organizations(({ "juicer_uprising" }));
    set_enemies(({ "coalition_states" }));
}

// Juicers have superhuman attributes
int query_is_superhuman() {
    return 1;
}

// Juicers heal faster
int query_healing_rate() {
    return 4;  // 4x normal healing rate
}

// Juicers have limited lifespan
int query_years_remaining(int years_active) {
    int base_life = 7;  // Average 7 years
    return base_life - years_active;
}

// Juicers are immune to drugs
int query_drug_immunity() {
    return 1;  // Complete immunity
}

// Juicers can enter combat frenzy
mapping query_frenzy_bonuses() {
    return ([
        "strike": 2,
        "damage": 4,
        "attacks": 2,
        "save_vs_pain": 10
    ]);
}

// Juicers take penalties as chemicals wear off
mapping query_burnout_penalties(int years_active) {
    if (years_active < 4) return ([]);
    if (years_active < 6) return ([ "me": -1, "pe": -1 ]);
    if (years_active < 7) return ([ "me": -2, "pe": -2, "ps": -1 ]);
    return ([ "me": -4, "pe": -4, "ps": -2, "spd": -2 ]);  // Late stage burnout
}

// Check if Juicer is in Last Call (final year)
int query_is_last_call(int years_active) {
    return years_active >= 6;
}
