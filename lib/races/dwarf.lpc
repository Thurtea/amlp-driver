// /lib/races/dwarf.lpc
// Dwarf Race - Stout dimensional travelers
//
// Dwarves are sturdy, stocky beings who came to Rifts Earth through
// dimensional rifts. They are master craftsmen and formidable warriors,
// with a natural resistance to magic and poison.

inherit "/std/race";

void create() {
    ::create();
    
    set_race_name("Dwarf");
    set_race_id("dwarf");
    set_race_plural("Dwarves");
    set_race_category(RACE_CAT_DEMIHUMAN);
    
    set_race_description(
        "Dwarves are stout, powerful beings who arrived on Rifts Earth through the "
        "dimensional rifts. Standing between four and five feet tall, they are as "
        "wide as they are strong, with muscular builds and incredible endurance. "
        "Dwarves are renowned for their craftsmanship, particularly in metalwork "
        "and engineering. They have a natural resistance to magic and poison, and "
        "are fierce warriors when provoked. While gruff and stubborn, dwarves are "
        "loyal to those who earn their respect."
    );
    
    // Physical characteristics - short and stocky
    set_size_category(SIZE_SMALL);
    set_height_range(48, 54);   // 4'0" to 4'6"
    set_weight_range(140, 200); // Heavy for their height
    set_lifespan(500);          // Long-lived but not as much as elves
    set_body_types(({ "stocky", "barrel-chested", "stout", "muscular" }));
    
    // Stat modifiers - strong and tough, but slower
    set_stat_modifiers(([
        "ps": 2,    // +2 Physical Strength
        "pe": 3,    // +3 Physical Endurance
        "pb": -1,   // -1 Physical Beauty (by human standards)
        "spd": -2   // -2 Speed (short legs)
    ]));
    
    // Combat bonuses
    set_combat_bonuses(([
        "parry": 1,
        "roll": 1
    ]));
    
    // Naturally tough - bonus SDC
    set_natural_sdc(20);  // +20 racial SDC
    set_mdc_creature(0);
    
    // Strong natural resistances
    set_resistances(([
        "magic": 30,     // 30% magic resistance
        "poison": 50,    // 50% poison resistance
        "disease": 25    // 25% disease resistance
    ]));
    set_immunities(({ }));
    set_vulnerabilities(({ }));
    
    // Racial abilities
    add_racial_ability("nightvision", 1);         // Can see in dark (underground living)
    add_racial_ability("detect_gold", 1);         // Innate sense for precious metals
    add_racial_ability("stonework_sense", 1);     // Detect shifting stone, traps
    add_racial_ability("appraise_metalwork", 1);  // Judge quality of metal items
    
    // Low magical affinity (natural resistance blocks it)
    set_psionic_potential(0);
    set_magic_potential(0);  // Cannot develop magic easily
    
    // OCC preferences - practical and martial
    set_allowed_occs(({ }));
    set_forbidden_occs(({ 
        "ley_line_walker",   // Magic resistance prevents it
        "mystic",
        "warlock",
        "shifter",
        "temporal_wizard"    // Most pure magic OCCs
    }));
    set_recommended_occs(({ 
        "operator", "mercenary", "vagabond",
        "headhunter", "techno_wizard"  // Can use tech-magic hybrid
    }));
    
    // Dwarves follow their codes strictly
    set_allowed_alignments(({ }));  // All allowed
    
    // Starting resources - craftsman's gear
    set_starting_equipment(({
        "/obj/clothing/dwarven_clothes",
        "/obj/item/backpack",
        "/obj/item/canteen",
        "/obj/item/toolkit",
        "/obj/weapon/hand_axe"
    }));
    set_starting_credits(200);
    
    // Faction relationships
    set_default_factions(({ "dwarven_clans" }));
    set_hostile_factions(({ "giant_kin" }));
    
    // Cultural background
    set_culture("Dwarven Clan Tradition");
    set_homeland("Various Dimensions (Primarily Palladium World)");
    set_languages(({ "dwarven", "goblinoid", "american" }));
    set_primary_language("dwarven");
}

// Dwarves have bonuses to technical and craft skills
int query_technical_bonus() {
    return 15;  // +15% to technical skills
}

int query_craft_bonus() {
    return 20;  // +20% to crafting/metalwork skills
}

// Dwarves can recognize quality
int query_appraisal_bonus() {
    return 25;  // +25% to appraisal of metal and stonework
}
