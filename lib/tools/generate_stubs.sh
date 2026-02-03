#!/bin/bash
# Script to generate stub race and OCC files

cd /home/thurtea/amlp-driver/lib

# Races that need stubs (excluding already created: human, elf, dwarf, ogre, dog_boy)
RACES=(
    "halfling" "gnome" "hobgoblin" "goblin" "orc" "troll"
    "wolfen" "minotaur" "changeling" "simvan" "psi_stalker" "mutant_animal" "cat_girl"
    "burster" "mind_bleeder" "mind_melter" "psi_ghost" "psi_healer"
    "dragon_hatchling" "adult_dragon" "ancient_dragon"
    "brodkil" "gargoyle" "vampire" "werewolf" "demon" "deevil" "nightbane" "godling" "thorny_dragon"
    "noli" "eandroth" "promethean" "trimadore" "uteni"
    "true_atlantean" "splugorth_minion" "splynn_slave" "cosmo_knight"
    "quick_flex" "mega_juicer" "titan_juicer" "dragon_juicer"
)

# OCCs that need stubs (excluding already created: vagabond, cyber_knight, operator, body_fixer, juicer)
OCCS=(
    "wilderness_scout" "city_rat" "rogue_scholar" "rogue_scientist"
    "mercenary" "headhunter" "special_forces" "robot_pilot" "power_armor_pilot"
    "ley_line_walker" "mystic" "warlock" "shifter" "temporal_wizard" "techno_wizard" "necromancer"
    "mind_melter" "burster" "psi_stalker" "psi_healer"
    "crazy" "full_conversion_borg" "dragon_hatchling_rcc" "cosmo_knight"
    "line_walker" "battle_magus" "elemental_fusionist"
    "sea_titan" "biomancer" "stone_master"
)

echo "Creating race stubs..."
for race in "${RACES[@]}"; do
    if [ ! -f "races/${race}.lpc" ]; then
        # Convert race_name to Title Case
        display_name=$(echo "$race" | sed 's/_/ /g' | awk '{for(i=1;i<=NF;i++) $i=toupper(substr($i,1,1)) tolower(substr($i,2))}1')
        
        cat > "races/${race}.lpc" << EOF
// /lib/races/${race}.lpc
// ${display_name} Race - Placeholder Implementation
//
// This is a stub file. Implement the full race details.

inherit "/std/race";

void create() {
    ::create();
    
    set_race_name("${display_name}");
    set_race_id("${race}");
    set_race_plural("${display_name}s");
    set_race_category(RACE_CAT_DEMIHUMAN);
    
    set_race_description(
        "TODO: Add full race description for ${display_name}."
    );
    
    // TODO: Configure physical characteristics
    set_size_category(SIZE_MEDIUM);
    set_height_range(60, 72);
    set_weight_range(120, 200);
    set_lifespan(80);
    
    // TODO: Configure stat modifiers
    set_stat_modifiers(([
        // Example: "ps": 2
    ]));
    
    // TODO: Configure racial abilities
    set_racial_abilities(({ }));
    
    // TODO: Configure OCC restrictions
    set_allowed_occs(({ }));
    set_forbidden_occs(({ }));
    set_recommended_occs(({ }));
    
    // TODO: Configure starting resources
    set_starting_equipment(({ }));
    set_starting_credits(100);
    
    set_languages(({ "american" }));
    set_primary_language("american");
}
EOF
        echo "  Created races/${race}.lpc"
    fi
done

echo ""
echo "Creating OCC stubs..."
for occ in "${OCCS[@]}"; do
    if [ ! -f "occs/${occ}.lpc" ]; then
        # Convert occ_name to Title Case
        display_name=$(echo "$occ" | sed 's/_/ /g' | awk '{for(i=1;i<=NF;i++) $i=toupper(substr($i,1,1)) tolower(substr($i,2))}1')
        
        cat > "occs/${occ}.lpc" << EOF
// /lib/occs/${occ}.lpc
// ${display_name} O.C.C. - Placeholder Implementation
//
// This is a stub file. Implement the full OCC details.

inherit "/std/occ";

void create() {
    ::create();
    
    set_occ_name("${display_name}");
    set_occ_id("${occ}");
    set_occ_category(OCC_CAT_ADVENTURER);
    set_alignment_requirement("any");
    
    set_occ_description(
        "TODO: Add full O.C.C. description for ${display_name}."
    );
    
    // TODO: Configure stat requirements
    set_stat_requirements(([
        // Example: "iq": 10
    ]));
    
    // TODO: Configure stat bonuses
    set_stat_bonuses(([
        // Example: "ps": 2
    ]));
    
    // Combat configuration
    set_base_attacks(2);
    set_attacks_per_level(1, 3);
    set_hand_to_hand_type("basic");
    
    // TODO: Configure OCC skills
    set_occ_skills(({
        "language_american"
    }));
    
    set_occ_related_skills(({ }));
    set_occ_skill_count(6);
    set_elective_count(4);
    set_secondary_count(6);
    
    // Magic/Psionics (if applicable)
    set_base_ppe(0);
    set_ppe_per_level(0);
    set_base_isp(0);
    set_isp_per_level(0);
    set_magic_type("none");
    set_psionic_type("none");
    
    // HP/SDC
    set_base_sdc(20);
    set_sdc_per_level(0);
    
    // TODO: Configure starting equipment
    set_standard_equipment(({ }));
    set_starting_credits(100, 500);
    set_starting_weapons(({ }));
    set_starting_armor(({ }));
    
    // Race restrictions
    set_race_requirements(({ }));
    set_race_exclusions(({ }));
    
    // Cybernetics
    set_cybernetics_allowed(1);
    set_max_cybernetic_slots(4);
}
EOF
        echo "  Created occs/${occ}.lpc"
    fi
done

echo ""
echo "Stub generation complete!"
echo "Total race files: $(ls -1 races/*.lpc 2>/dev/null | wc -l)"
echo "Total OCC files: $(ls -1 occs/*.lpc 2>/dev/null | wc -l)"
