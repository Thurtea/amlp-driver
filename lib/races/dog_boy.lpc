// /lib/races/dog_boy.lpc
// Dog Boy Race - Coalition Mutant Canines
//
// Dog Boys are genetically engineered mutant canines created by the Coalition
// States. They possess enhanced senses, psionic abilities, and unwavering
// loyalty. Many serve as scouts and supernatural hunters for the CS military.

inherit "/std/race";

void create() {
    ::create();
    
    set_race_name("Dog Boy");
    set_race_id("dog_boy");
    set_race_plural("Dog Boys");
    set_race_category(RACE_CAT_MUTANT);
    
    set_race_description(
        "Dog Boys are genetically engineered mutant canines created in Coalition "
        "States laboratories. Standing upright like humans but covered in fur and "
        "possessing canine features, they are designed to be loyal soldiers and "
        "supernatural hunters. Their enhanced senses - particularly smell and "
        "hearing - make them excellent trackers. Most importantly, Dog Boys "
        "possess natural psionic abilities that allow them to sense the supernatural "
        "and psychic phenomena. While fiercely loyal to their handlers, some Dog "
        "Boys have escaped or been freed, living as rogues seeking their own path."
    );
    
    // Physical characteristics
    set_size_category(SIZE_MEDIUM);
    set_height_range(60, 78);   // Similar to humans
    set_weight_range(150, 250); // Slightly heavier (muscle)
    set_lifespan(40);           // Shorter than human lifespan
    set_body_types(({ "lean", "athletic", "muscular", "powerful" }));
    
    // Stat modifiers - enhanced senses and physical, average mental
    set_stat_modifiers(([
        "ps": 2,    // +2 Physical Strength
        "pp": 2,    // +2 Physical Prowess (agility)
        "pe": 2,    // +2 Physical Endurance
        "spd": 3,   // +3 Speed (fast runners)
        "ma": -1,   // -1 Mental Affinity (intimidating to some)
        "pb": -2    // -2 Physical Beauty (by human standards)
    ]));
    
    // Combat bonuses - trained military combatants
    set_combat_bonuses(([
        "initiative": 2,    // Enhanced senses = faster reactions
        "strike": 1,
        "dodge": 1
    ]));
    
    // Naturally tough
    set_natural_sdc(25);   // +25 racial SDC
    set_natural_armor(0);
    set_mdc_creature(0);
    
    // Natural resistances
    set_resistances(([
        "horror_factor": 40,  // 40% resistance to horror
        "disease": 20         // 20% disease resistance
    ]));
    set_immunities(({ }));
    set_vulnerabilities(({ }));
    
    // Racial abilities - enhanced senses and psionic detection
    add_racial_ability("enhanced_smell", 3);        // Superior sense of smell
    add_racial_ability("enhanced_hearing", 2);      // Superior hearing
    add_racial_ability("nightvision", 1);           // Can see in darkness
    add_racial_ability("tracking_scent", 2);        // Track by smell
    add_racial_ability("sense_supernatural", 2);    // Psionic sense of supernatural
    add_racial_ability("sense_psychic", 2);         // Detect psionic activity
    add_racial_ability("sense_magic", 1);           // Detect magic use
    add_racial_ability("recognize_scent", 2);       // Remember scents perfectly
    add_racial_ability("bite_attack", 1);           // Natural bite (2d4 damage)
    add_racial_ability("claw_attack", 1);           // Natural claws (1d6 damage)
    
    // Natural psionic abilities (sensitive)
    set_psionic_potential(30);  // Base ISP of 30
    set_magic_potential(0);     // No magical affinity
    
    // OCC restrictions
    set_allowed_occs(({ }));
    set_forbidden_occs(({ 
        "ley_line_walker",   // No magic aptitude
        "mystic",
        "warlock",
        "shifter",
        "temporal_wizard",
        "techno_wizard"
    }));
    set_recommended_occs(({ 
        "psi_stalker",       // Natural fit for supernatural hunting
        "dog_pack",          // Dog Boy specific OCC
        "wilderness_scout",
        "vagabond",
        "headhunter"
    }));
    
    // Usually principled or scrupulous (loyal and honorable)
    set_allowed_alignments(({ }));  // All technically allowed
    
    // Starting resources - depends on background
    set_starting_equipment(({
        "/obj/clothing/basic_clothes",
        "/obj/item/backpack",
        "/obj/item/canteen"
    }));
    set_starting_credits(100);
    
    // Faction relationships
    set_default_factions(({ }));  // Depends on background (CS or Rogue)
    set_hostile_factions(({ }));
    
    // Cultural background
    set_culture("Coalition Engineering/Military or Rogue Pack");
    set_homeland("Coalition States (Chi-Town, Iron Heart, or Lone Star)");
    set_languages(({ "american" }));  // CS language primarily
    set_primary_language("american");
}

// Dog Boys have superior tracking abilities
int query_tracking_bonus() {
    return 40;  // +40% to tracking by scent
}

// Dog Boys can identify creatures by scent
int query_identify_scent_bonus() {
    return 60;  // +60% to recognize/identify scents
}

// Dog Boys sense the supernatural within range
int query_sense_supernatural_range() {
    return 100;  // 100 feet base range
}

// Dog Boys sense psionic use within range
int query_sense_psionic_range() {
    return 50;  // 50 feet base range
}

// Dog Boys sense magic within range
int query_sense_magic_range() {
    return 30;  // 30 feet base range
}

// Dog Boys have natural weapons
mapping query_natural_attacks() {
    return ([
        "bite": ([ "damage": "2d4", "type": "physical" ]),
        "claw": ([ "damage": "1d6", "type": "physical" ])
    ]);
}

// Dog Boys run faster on all fours
int query_all_fours_speed_bonus() {
    return 10;  // +10 to speed when running on all fours
}

// Dog Boys are pack-oriented
int query_pack_tactics_bonus() {
    return 2;  // +2 to strike/dodge when fighting with allies
}

// Dog Boys can communicate basic concepts through body language
int query_pack_communication() {
    return 1;  // Can communicate with other Dog Boys/canines silently
}
