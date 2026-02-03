// /lib/occs/vagabond.lpc
// Vagabond O.C.C. - Jack of All Trades
//
// Vagabonds are the common folk of Rifts Earth - survivors who have learned
// to adapt and get by with whatever skills they can pick up. They are the
// most versatile class, able to learn a wide range of skills.

inherit "/std/occ";

void create() {
    ::create();
    
    set_occ_name("Vagabond");
    set_occ_id("vagabond");
    set_occ_category(OCC_CAT_ADVENTURER);
    set_alignment_requirement("any");
    
    set_occ_description(
        "Vagabonds are the everyday people of Rifts Earth - survivors who have "
        "learned to get by through adaptability and street smarts rather than "
        "specialized training. They might be drifters, displaced refugees, or "
        "just ordinary folks who never had the opportunity for formal education "
        "or military training. What Vagabonds lack in specialized expertise, they "
        "make up for in versatility. They pick up skills wherever they can, becoming "
        "jacks of all trades. Many Vagabonds eventually specialize and transition "
        "to other professions, but others embrace their flexible lifestyle."
    );
    
    // No stat requirements - open to anyone
    set_stat_requirements(([
        // None required
    ]));
    
    // No stat bonuses - average class
    set_stat_bonuses(([
        // None
    ]));
    
    // Standard level progression
    set_xp_table(XP_LEVELS);
    
    // Base abilities - survival and adaptability
    set_base_abilities(({
        "streetwise",
        "scrounge"
    }));
    
    // Abilities gained at levels
    set_occ_abilities(([
        3: ({ "resist_fatigue" }),
        6: ({ "quick_learner" }),
        9: ({ "jack_of_trades" }),
        12: ({ "street_smart" })
    ]));
    
    // Combat - basic
    set_base_attacks(2);
    set_attacks_per_level(1, 4);  // +1 attack every 4 levels
    set_hand_to_hand_type("basic");
    
    // OCC Skills - practical survival skills
    set_occ_skills(({
        "language_american",     // Native language at 98%
        "mathematics_basic",
        "pilot_automobile",
        "streetwise",
        "first_aid"
    }));
    
    // Many OCC related options
    set_occ_related_skills(({
        "cook", "sew", "general_repair", "automotive_mechanics",
        "computer_operation", "radio_basic", "pilot_motorcycle",
        "swimming", "climbing", "running", "wilderness_survival",
        "land_navigation", "hunting", "track_animals",
        "barter", "gambling", "pick_pockets", "palming",
        "wp_knife", "wp_blunt", "wp_handgun"
    }));
    set_occ_skill_count(8);  // Pick 8 from OCC related
    
    // Wide elective access - Vagabond's strength
    set_elective_categories(({}));  // All categories allowed
    set_elective_count(6);
    set_secondary_count(10);  // More secondary skills than most
    
    // Skill bonuses - modest
    set_skill_bonuses(([
        "domestic": 5,
        "technical": 5,
        "rogue": 5
    ]));
    
    // No magic or psionics by default
    set_base_ppe(0);
    set_ppe_per_level(0);
    set_base_isp(0);
    set_isp_per_level(0);
    set_magic_type("none");
    set_psionic_type("none");
    
    // SDC based
    set_base_hp(0);  // Standard: PE + 1d6
    set_hp_per_level(0);  // Standard: 1d6
    set_base_sdc(20);
    set_sdc_per_level(0);
    set_mdc_class(0);
    
    // Basic starting equipment
    set_standard_equipment(({
        "/obj/item/backpack",
        "/obj/item/canteen",
        "/obj/item/flashlight",
        "/obj/item/pocket_knife",
        "/obj/item/rope",
        "/obj/clothing/basic_clothes",
        "/obj/clothing/jacket"
    }));
    
    set_optional_gear(({
        "/obj/item/first_aid_kit",
        "/obj/item/sleeping_bag",
        "/obj/weapon/baseball_bat",
        "/obj/weapon/knife"
    }));
    
    set_starting_credits(100, 400);
    
    set_starting_weapons(({
        "/obj/weapon/knife"
    }));
    
    set_starting_armor(({
        // None - too poor for armor
    }));
    
    // No race restrictions
    set_race_requirements(({ }));
    set_race_exclusions(({ }));
    
    // Limited cybernetics
    set_cybernetics_allowed(1);
    set_max_cybernetic_slots(2);
    set_starting_cybernetics(({ }));
    
    // No special powers
    set_special_powers(({ }));
    set_vehicle_type("none");
    set_special_training("");
    
    // Background
    set_background(
        "Vagabonds come from all walks of life - displaced farmers, orphaned "
        "children who grew up on the streets, former slaves, or simply people "
        "who never found a stable profession. Some are wanderers by choice, "
        "others are forced into vagrancy by circumstance."
    );
    set_organizations(({ }));
    set_enemies(({ }));
}

// Vagabonds can learn skills faster
int query_learning_bonus() {
    return 5;  // +5% to learning new skills
}

// Vagabonds can adapt to situations
int query_adaptability_bonus() {
    return 10;  // +10% when attempting unfamiliar tasks
}
