// /lib/races/elf.lpc
// Elf Race - Mystical beings from another dimension
//
// Elves came to Rifts Earth through dimensional rifts, bringing ancient
// magic and culture. They are graceful, long-lived, and naturally attuned
// to magical energies.

inherit "/std/race";

void create() {
    ::create();
    
    set_race_name("Elf");
    set_race_id("elf");
    set_race_plural("Elves");
    set_race_category(RACE_CAT_DEMIHUMAN);
    
    set_race_description(
        "Elves are beings of grace and magic who came to Rifts Earth through "
        "dimensional portals. They are tall, slender, and beautiful by human "
        "standards, with pointed ears and an ethereal quality. Most elves possess "
        "an innate connection to magic and the supernatural. They are long-lived, "
        "often surviving for thousands of years, which gives them patience and "
        "a different perspective on time. Many elves are drawn to the practice of "
        "magic, and they make exceptional spell casters."
    );
    
    // Physical characteristics
    set_size_category(SIZE_MEDIUM);
    set_height_range(66, 84);   // 5'6" to 7'0" - taller and slender
    set_weight_range(100, 180); // Lighter than humans
    set_lifespan(2000);         // Very long-lived
    set_body_types(({ "slender", "lithe", "graceful" }));
    
    // Stat modifiers - agile and intelligent, but less sturdy
    set_stat_modifiers(([
        "pp": 2,    // +2 Physical Prowess (agility)
        "pb": 2,    // +2 Physical Beauty
        "iq": 1,    // +1 Intelligence
        "pe": -1    // -1 Physical Endurance
    ]));
    
    // Combat bonuses - graceful fighters
    set_combat_bonuses(([
        "initiative": 1,
        "dodge": 1
    ]));
    
    // SDC creature, slightly less sturdy
    set_natural_sdc(-5);  // -5 to racial SDC
    set_mdc_creature(0);
    
    // Natural resistances
    set_resistances(([
        "charm": 20,    // 20% resistance to charm/enchantment
        "sleep": 50     // 50% resistance to sleep magic
    ]));
    set_immunities(({ }));
    set_vulnerabilities(({ "cold_iron" }));  // Vulnerable to cold iron
    
    // Racial abilities
    add_racial_ability("nightvision", 1);      // Can see in dark up to 60'
    add_racial_ability("detect_magic", 1);     // Innate magic sense
    add_racial_ability("enhanced_hearing", 1); // Superior hearing
    
    // Natural magical affinity
    set_psionic_potential(0);
    set_magic_potential(10);  // +10 base PPE
    
    // OCC preferences - magic-oriented
    set_allowed_occs(({ }));   // All allowed
    set_forbidden_occs(({ 
        "juicer",           // Chemical enhancement rejected
        "crazy",            // Brain implants incompatible
        "full_conversion_borg"  // Lose magical nature
    }));
    set_recommended_occs(({ 
        "ley_line_walker", "mystic", "warlock", 
        "temporal_wizard", "mind_melter"
    }));
    
    // Tend toward good or selfish alignments
    set_allowed_alignments(({ }));  // All technically allowed
    
    // Starting resources
    set_starting_equipment(({
        "/obj/clothing/elven_clothes",
        "/obj/item/backpack",
        "/obj/item/canteen",
        "/obj/weapon/dagger"
    }));
    set_starting_credits(150);
    
    // Faction relationships
    set_default_factions(({ "elven_kingdoms" }));
    set_hostile_factions(({ "demon_lords" }));
    
    // Cultural background
    set_culture("Ancient Elven Tradition");
    set_homeland("Various Dimensions (Primarily Palladium World)");
    set_languages(({ "elven", "dragonese", "american" }));
    set_primary_language("elven");
}

// Elves have enhanced senses
int query_perception_bonus() {
    return 10;  // +10% to perception skills
}

// Elves have a natural aptitude for magic
int query_magic_skill_bonus() {
    return 15;  // +15% to magic-related skills
}
