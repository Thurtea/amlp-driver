// /lib/races/ogre.lpc
// Ogre Race - Massive humanoid brutes
//
// Ogres are large, powerful humanoids often found in the wilderness
// or employed as enforcers and heavy laborers. While often stereotyped
// as dumb brutes, many ogres are more intelligent than given credit for.

inherit "/std/race";

void create() {
    ::create();
    
    set_race_name("Ogre");
    set_race_id("ogre");
    set_race_plural("Ogres");
    set_race_category(RACE_CAT_GIANT);
    
    set_race_description(
        "Ogres are massive humanoids standing eight to twelve feet tall, with "
        "heavily muscled bodies and brutish features. They came to Rifts Earth "
        "through dimensional portals, and have established themselves in wilderness "
        "areas and as hired muscle in various settlements. While often dismissed "
        "as stupid brutes, ogres possess a cunning intelligence and strong tribal "
        "culture. They are formidable warriors, capable of wielding weapons that "
        "would be siege equipment for smaller beings. Many have found employment "
        "as mercenaries, bouncers, or manual laborers."
    );
    
    // Physical characteristics - large and powerful
    set_size_category(SIZE_LARGE);
    set_height_range(96, 144);   // 8'0" to 12'0"
    set_weight_range(500, 900);  // Massive
    set_lifespan(200);           // Longer than humans
    set_body_types(({ "massive", "hulking", "barrel-chested", "muscular" }));
    
    // Stat modifiers - strong and tough, limited intellect
    set_stat_modifiers(([
        "ps": 10,   // +10 Physical Strength (supernatural strength)
        "pe": 5,    // +5 Physical Endurance
        "iq": -2,   // -2 Intelligence
        "ma": -2,   // -2 Mental Affinity
        "pb": -3,   // -3 Physical Beauty
        "pp": -1    // -1 Physical Prowess
    ]));
    
    // Combat bonuses - brute force
    set_combat_bonuses(([
        "damage": 4,     // +4 damage bonus
        "strike": -1     // -1 strike (less precise)
    ]));
    
    // Naturally tough - high SDC
    set_natural_sdc(50);   // +50 racial SDC
    set_natural_armor(6);  // Natural AR 6 (tough hide)
    set_mdc_creature(0);   // SDC creature (not supernatural)
    
    // Natural resistances - hardy constitution
    set_resistances(([
        "cold": 25,      // 25% cold resistance
        "disease": 30,   // 30% disease resistance
        "fear": 20       // 20% fear resistance
    ]));
    set_immunities(({ }));
    set_vulnerabilities(({ }));
    
    // Racial abilities
    add_racial_ability("intimidate", 2);        // Natural intimidation
    add_racial_ability("enhanced_strength", 1); // Can lift massive weights
    add_racial_ability("thick_hide", 1);        // Natural armor
    add_racial_ability("night_vision", 1);      // Can see in dim light
    
    // Limited magical/psionic potential
    set_psionic_potential(0);
    set_magic_potential(0);
    
    // OCC restrictions - limited options
    set_allowed_occs(({ }));
    set_forbidden_occs(({ 
        "ley_line_walker",
        "mystic",
        "temporal_wizard",
        "techno_wizard",
        "rogue_scholar",
        "cyber_knight",      // Requires discipline and code
        "operator",          // Dexterity requirements
        "mind_melter"
    }));
    set_recommended_occs(({ 
        "mercenary", "vagabond", "headhunter"
    }));
    
    // Tend toward selfish or evil
    set_allowed_alignments(({ 
        ALIGN_UNPRINCIPLED, ALIGN_ANARCHIST,
        ALIGN_ABERRANT, ALIGN_MISCREANT
    }));
    
    // Starting resources - basic
    set_starting_equipment(({
        "/obj/clothing/crude_clothes",
        "/obj/item/large_sack",
        "/obj/weapon/club"
    }));
    set_starting_credits(50);  // Less resources
    
    // Faction relationships
    set_default_factions(({ }));
    set_hostile_factions(({ "coalition_states" }));  // CS classifies them as hostile
    
    // Cultural background
    set_culture("Tribal");
    set_homeland("Various Dimensions");
    set_languages(({ "goblinoid", "american" }));
    set_primary_language("goblinoid");
}

// Ogres can carry massive loads
int query_carry_capacity_multiplier() {
    return 3;  // 3x normal carrying capacity
}

// Ogres are intimidating
int query_intimidation_bonus() {
    return 30;  // +30% to intimidation
}

// Ogres have penalties to fine motor skills
int query_fine_motor_penalty() {
    return -20;  // -20% to delicate tasks
}

// Ogres can use oversized weapons
int can_use_oversized_weapons() {
    return 1;
}
