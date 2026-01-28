// /lib/occs/body_fixer.lpc
// Body Fixer O.C.C. - Street Medic and Healer
//
// Body Fixers are the doctors, medics, and cybernetic surgeons who keep
// people alive in the dangerous world of Rifts Earth. They combine
// traditional medicine with cutting-edge cybernetic repair.

inherit "/std/occ";

void create() {
    ::create();
    
    set_occ_name("Body Fixer");
    set_occ_id("body_fixer");
    set_occ_category(OCC_CAT_SCHOLAR);
    set_alignment_requirement("any");
    
    set_occ_description(
        "Body Fixers are the healers of Rifts Earth - part doctor, part cybernetic "
        "surgeon, and part miracle worker. In a world where injuries are common "
        "and death lurks around every corner, Body Fixers are invaluable. They "
        "combine knowledge of traditional medicine with cybernetic installation "
        "and repair, able to treat everything from gunshot wounds to malfunctioning "
        "bionics. Some work in established clinics, others are street docs who "
        "ask no questions, and many travel with adventuring groups or mercenary "
        "companies as combat medics. A skilled Body Fixer can mean the difference "
        "between life and death."
    );
    
    // Stat requirements - need intelligence and steady hands
    set_stat_requirements(([
        "iq": 12,   // Intelligence 12+
        "pp": 10    // Physical Prowess 10+ (steady hands)
    ]));
    
    // Stat bonuses
    set_stat_bonuses(([
        "iq": 1     // +1 Intelligence from medical studies
    ]));
    
    // Standard level progression
    set_xp_table(XP_LEVELS);
    
    // Base abilities
    set_base_abilities(({
        "medical_diagnosis",    // Identify injuries and conditions
        "steady_hands",         // Bonus to delicate procedures
        "triage_expert"         // Quickly assess multiple patients
    }));
    
    // Abilities gained at levels
    set_occ_abilities(([
        3: ({ "field_surgery" }),
        5: ({ "cybernetic_specialist" }),
        7: ({ "trauma_expert" }),
        9: ({ "master_surgeon" }),
        12: ({ "miracle_worker" })
    ]));
    
    // Combat - minimal, focuses on healing
    set_base_attacks(2);
    set_attacks_per_level(1, 4);
    set_hand_to_hand_type("basic");
    
    // OCC Skills - medical and technical
    set_occ_skills(({
        "language_american",
        "literacy_american",
        "mathematics_basic",
        "mathematics_advanced",
        "biology",
        "chemistry",
        "pathology",
        "first_aid",
        "paramedic",
        "surgery",
        "cybernetic_medicine",
        "brewing_pharmaceutical",
        "computer_operation",
        "read_sensory_equipment"
    }));
    
    // OCC related skills - expanded medical options
    set_occ_related_skills(({
        "chemistry_analytical", "botany", "zoology",
        "holistic_medicine", "medical_doctor", "forensics",
        "criminal_science", "psychology",
        "robot_electronics", "electrical_engineer",
        "pilot_automobile", "pilot_hovercycle",
        "wp_knife", "wp_energy_pistol"
    }));
    set_occ_skill_count(6);
    
    // Elective skills
    set_elective_categories(({
        SKILL_CAT_COMMUNICATION, SKILL_CAT_DOMESTIC,
        SKILL_CAT_MEDICAL, SKILL_CAT_PILOT,
        SKILL_CAT_SCIENCE, SKILL_CAT_TECHNICAL
    }));
    set_elective_count(4);
    set_secondary_count(8);
    
    // Excellent medical skill bonuses
    set_skill_bonuses(([
        "medical": 25,
        "science": 15,
        "technical": 10
    ]));
    
    // No magic or psionics normally
    set_base_ppe(0);
    set_ppe_per_level(0);
    set_base_isp(0);
    set_isp_per_level(0);
    set_magic_type("none");
    set_psionic_type("none");
    
    // Average combat stats
    set_base_hp(0);
    set_hp_per_level(0);
    set_base_sdc(15);
    set_sdc_per_level(0);
    set_mdc_class(0);
    
    // Medical equipment
    set_standard_equipment(({
        "/obj/item/medical_bag",
        "/obj/item/first_aid_kit",
        "/obj/item/surgical_kit",
        "/obj/item/diagnostic_scanner",
        "/obj/item/cybernetic_toolkit",
        "/obj/item/flashlight",
        "/obj/clothing/medical_scrubs",
        "/obj/item/medical_drugs"  // Assorted medicines
    }));
    
    set_optional_gear(({
        "/obj/item/portable_hospital",
        "/obj/item/cryo_chamber",
        "/obj/item/cybernetic_parts",
        "/obj/vehicle/ambulance"
    }));
    
    set_starting_credits(400, 1500);
    
    set_starting_weapons(({
        "/obj/weapon/knife",
        "/obj/weapon/scalpel"  // Also a weapon in emergencies
    }));
    
    set_starting_armor(({
        "/obj/armor/light_body_armor"
    }));
    
    // No race restrictions
    set_race_requirements(({ }));
    set_race_exclusions(({
        "ogre"  // Fine motor skill issues
    }));
    
    // Cybernetics - understands them well
    set_cybernetics_allowed(2);  // Full
    set_max_cybernetic_slots(4);
    set_starting_cybernetics(({ }));
    
    // No special powers
    set_special_powers(({ }));
    set_vehicle_type("none");
    set_special_training("");
    
    // Background
    set_background(
        "Body Fixers learn their trade through apprenticeship, surviving medical "
        "schools, or self-study from pre-Rifts medical texts. Most are driven by "
        "a genuine desire to help others, though some are in it purely for the "
        "credits. The best Body Fixers combine compassion with clinical skill, "
        "able to work under pressure and make hard decisions."
    );
    set_organizations(({ "healers_guild" }));
    set_enemies(({ }));
}

// Body Fixers get healing bonuses
int query_healing_bonus() {
    return 20;  // +20% to all healing skills
}

// Body Fixers can stabilize dying patients faster
int query_stabilize_bonus() {
    return 30;  // +30% to stabilize critical patients
}

// Body Fixers can install cybernetics
int query_cybernetic_surgery_bonus() {
    return 15;  // +15% to cybernetic installation
}

// Body Fixers can identify drugs and poisons
int query_pharmaceutical_bonus() {
    return 20;  // +20% to identify/create pharmaceuticals
}

// Patients heal faster under Body Fixer care
int query_recovery_rate_bonus() {
    return 50;  // 50% faster recovery with proper care
}
