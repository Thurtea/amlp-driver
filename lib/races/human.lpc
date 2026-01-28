// /lib/races/human.lpc
// Human Race - The baseline race for Rifts
//
// Humans are the most common race on Rifts Earth. They are adaptable,
// versatile, and can pursue virtually any O.C.C. without restriction.

inherit "/std/race";

void create() {
    ::create();
    
    set_race_name("Human");
    set_race_id("human");
    set_race_plural("Humans");
    set_race_category(RACE_CAT_HUMAN);
    
    set_race_description(
        "Humans remain the dominant species on Rifts Earth, despite the cataclysm "
        "and the arrival of countless supernatural beings. Their greatest strength "
        "lies in their adaptability, ingenuity, and determination. While they lack "
        "the natural abilities of many other races, humans make up for it with "
        "technology, magic, and sheer willpower. They can be found in every "
        "corner of the world, from the Coalition States to the wilderness, "
        "pursuing every conceivable occupation."
    );
    
    // Physical characteristics - standard human range
    set_size_category(SIZE_MEDIUM);
    set_height_range(60, 78);   // 5'0" to 6'6"
    set_weight_range(100, 250);
    set_lifespan(80);
    set_body_types(({ "thin", "average", "athletic", "muscular", "heavy" }));
    
    // Humans have no stat modifiers - they are the baseline
    set_stat_modifiers(([
        // No modifiers - baseline race
    ]));
    
    // No special combat bonuses
    set_combat_bonuses(([
        // None
    ]));
    
    // Standard SDC creature
    set_natural_sdc(0);
    set_mdc_creature(0);
    
    // No special resistances or immunities
    set_resistances(([
        // None
    ]));
    set_immunities(({ }));
    set_vulnerabilities(({ }));
    
    // No racial abilities
    set_racial_abilities(({ }));
    
    // Minor psionic potential (most humans are latent)
    set_psionic_potential(0);  // May develop with OCC
    set_magic_potential(0);    // May develop with OCC
    
    // Humans can select ANY OCC
    set_allowed_occs(({ }));   // Empty = all allowed
    set_forbidden_occs(({ }));
    set_recommended_occs(({ 
        "vagabond", "rogue_scholar", "wilderness_scout", 
        "operator", "city_rat", "body_fixer"
    }));
    
    // All alignments allowed
    set_allowed_alignments(({ }));  // Empty = all allowed
    
    // Starting resources - average
    set_starting_equipment(({
        "/obj/clothing/basic_clothes",
        "/obj/item/backpack",
        "/obj/item/canteen"
    }));
    set_starting_credits(100);
    
    // Neutral faction standing
    set_default_factions(({ }));
    set_hostile_factions(({ }));
    
    // Cultural background
    set_culture("Varied");
    set_homeland("Earth (Various Regions)");
    set_languages(({ "american", "spanish", "euro" }));
    set_primary_language("american");
}

// Humans get a bonus to skill selection
int query_skill_bonus() {
    return 5;  // +5% to all skills (adaptability)
}

// Humans can learn any skill category
string* query_skill_restrictions() {
    return ({ });  // No restrictions
}
