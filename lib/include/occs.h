// /lib/include/occs.h
// Occupational Character Class (OCC) definitions

#ifndef __OCCS_H__
#define __OCCS_H__

// OCC categories
#define OCC_CAT_ADVENTURER   "adventurer"
#define OCC_CAT_MILITARY     "military"
#define OCC_CAT_TECHNICAL    "technical"
#define OCC_CAT_SCHOLARLY    "scholarly"
#define OCC_CAT_MAGIC        "magic"
#define OCC_CAT_PSIONIC      "psionic"
#define OCC_CAT_AUGMENTED    "augmented"
#define OCC_CAT_PILOT        "pilot"
#define OCC_CAT_SPECIAL      "special"
#define OCC_CAT_CRIMINAL     "criminal"
#define OCC_CAT_MEDICAL      "medical"
#define OCC_CAT_COALITION    "coalition"

// OCC file paths
#define OCC_DIR          "/occs/"
#define OCC_DAEMON       "/daemon/occs"

// Base OCC path
#define BASE_OCC         "/std/occ"

// Available OCCs (core implementations)
#define CORE_OCCS ({ \
    "vagabond", "cyber_knight", "operator", "body_fixer", "juicer", \
    "wilderness_scout", "rogue_scholar", "headhunter", "city_rat", "bounty_hunter" \
})

// All OCCs (for full implementation)
#define ALL_OCCS ({ \
    "atlantean_nomad", "atlantean_slave", "body_fixer", "bounty_hunter", \
    "city_rat", "cs_grunt", "cs_ranger", "cs_military_specialist", \
    "cs_samas_pilot", "cs_technical_officer", "cyber_doc", "cyber_knight", \
    "forger", "freelance_spy", "glitter_boy_pilot", "gifted_gypsy", \
    "headhunter", "iss_peacekeeper", "iss_specter", "juicer", "knight", \
    "master_assassin", "ninja_juicer", "ntset_protector", "operator", \
    "pirate", "professional_thief", "rogue_scholar", "royal_knight", \
    "sailor", "smuggler", "special_forces", "vagabond", "wilderness_scout", \
    "ley_line_walker", "maxi_man", "sunaj_assassin" \
})

// Skill categories
#define SKILL_CAT_COMMUNICATION  "communication"
#define SKILL_CAT_DOMESTIC       "domestic"
#define SKILL_CAT_ELECTRICAL     "electrical"
#define SKILL_CAT_ESPIONAGE      "espionage"
#define SKILL_CAT_MECHANICAL     "mechanical"
#define SKILL_CAT_MEDICAL        "medical"
#define SKILL_CAT_MILITARY       "military"
#define SKILL_CAT_PHYSICAL       "physical"
#define SKILL_CAT_PILOT          "pilot"
#define SKILL_CAT_PILOT_RELATED  "pilot_related"
#define SKILL_CAT_ROGUE          "rogue"
#define SKILL_CAT_SCIENCE        "science"
#define SKILL_CAT_TECHNICAL      "technical"
#define SKILL_CAT_WEAPON         "weapon"
#define SKILL_CAT_WILDERNESS     "wilderness"

// Experience level thresholds (standard Palladium)
#define XP_LEVELS ({ \
    0,          /* Level 1 */ \
    2000,       /* Level 2 */ \
    4000,       /* Level 3 */ \
    8000,       /* Level 4 */ \
    16000,      /* Level 5 */ \
    25000,      /* Level 6 */ \
    35000,      /* Level 7 */ \
    50000,      /* Level 8 */ \
    70000,      /* Level 9 */ \
    95000,      /* Level 10 */ \
    125000,     /* Level 11 */ \
    175000,     /* Level 12 */ \
    225000,     /* Level 13 */ \
    300000,     /* Level 14 */ \
    400000      /* Level 15 */ \
})

#endif
