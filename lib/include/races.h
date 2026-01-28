// /lib/include/races.h
// Race system definitions

#ifndef __RACES_H__
#define __RACES_H__

// Race categories
#define RACE_CAT_HUMAN       "human"
#define RACE_CAT_DEMIHUMAN   "demihuman"
#define RACE_CAT_SUPERNATURAL "supernatural"
#define RACE_CAT_DBEE        "dimensional_being"
#define RACE_CAT_MUTANT      "mutant"
#define RACE_CAT_DRAGON      "dragon"
#define RACE_CAT_FAERIE      "faerie"
#define RACE_CAT_GIANT       "giant"
#define RACE_CAT_UNDEAD      "undead"
#define RACE_CAT_PSIONIC     "psionic"
#define RACE_CAT_SHIFTER     "shifter"

// Race file paths
#define RACE_DIR         "/races/"
#define RACE_DAEMON      "/daemon/races"

// Alignment types
#define ALIGN_PRINCIPLED    "principled"
#define ALIGN_SCRUPULOUS    "scrupulous"
#define ALIGN_UNPRINCIPLED  "unprincipled"
#define ALIGN_ANARCHIST     "anarchist"
#define ALIGN_MISCREANT     "miscreant"
#define ALIGN_ABERRANT      "aberrant"
#define ALIGN_DIABOLIC      "diabolic"

// Alignment categories
#define ALIGN_GOOD       ({ "principled", "scrupulous" })
#define ALIGN_SELFISH    ({ "unprincipled", "anarchist" })
#define ALIGN_EVIL       ({ "miscreant", "aberrant", "diabolic" })

// Base race path
#define BASE_RACE        "/std/race"

// Available races array (core implementations)
#define CORE_RACES ({ \
    "human", "elf", "dwarf", "ogre", "dog_boy", \
    "orc", "troll", "goblin", "atlantean", "psi_stalker" \
})

// All races (for full implementation)
#define ALL_RACES ({ \
    "algor_frost_giant", "atlantean", "basilisk", "bearman", "bogie", \
    "brownie", "burster", "changeling", "common_faerie", "common_pixie", \
    "conservator", "coyle", "dog_boy", "dwarf", "elf", \
    "equinoid", "fire_dragon", "frost_pixie", "gargoyle", "goblin", \
    "great_horned_dragon", "green_wood_faerie", "gurgoyle", "hawrke_duhk", \
    "hawrk_ka", "human", "ice_dragon", "jotan", "kankoran", \
    "mind_melter", "night_elf_faerie", "nimro_fire_giant", "ogre", "orc", \
    "pogtal", "psi_stalker", "rahu_man", "ratling", "secondary_vampire", \
    "silver_bells_faerie", "thunder_lizard_dragon", "titan", "tree_sprite", \
    "troll", "water_sprite", "werebear", "weretiger", "werewolf", \
    "wild_psi_stalker", "wild_vampire" \
})

#endif
