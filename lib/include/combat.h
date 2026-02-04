// /lib/include/combat.h
// Combat System Constants, Archetypes, and Attack Verbs
// Phase 2 - Real-Time Verb-Based Combat System

#ifndef __COMBAT_H__
#define __COMBAT_H__

// =============================================================================
// COMBAT ARCHETYPES
// =============================================================================

#define ARCHETYPE_DRAGON            "dragon"
#define ARCHETYPE_HUMANOID_UNARMED  "humanoid_unarmed"
#define ARCHETYPE_ARMED_MELEE       "armed_melee"
#define ARCHETYPE_ARMED_RANGED      "armed_ranged"
#define ARCHETYPE_CYBORG            "cyborg"
#define ARCHETYPE_SUPERNATURAL      "supernatural"
#define ARCHETYPE_ANIMAL            "animal"
#define ARCHETYPE_MONSTER           "monster"

// =============================================================================
// COMBAT CONSTANTS
// =============================================================================

// Combat round timing
#define COMBAT_ROUND_DELAY          4        // Seconds between combat rounds

// Defense types
#define DEFENSE_NONE                0
#define DEFENSE_PARRY               1
#define DEFENSE_DODGE               2
#define DEFENSE_AUTO_PARRY          3
#define DEFENSE_AUTO_DODGE          4

// Attack types
#define ATTACK_MELEE               0
#define ATTACK_RANGED              1
#define ATTACK_NATURAL             2
#define ATTACK_PSIONIC             3
#define ATTACK_MAGIC               4

// Hit results
#define HIT_MISS                   0
#define HIT_NORMAL                 1
#define HIT_CRITICAL               2
#define HIT_FUMBLE                 3

// Defense results
#define DEF_FAILED                 0
#define DEF_PARRIED                1
#define DEF_DODGED                 2
#define DEF_BLOCKED                3

// Base combat numbers
#define BASE_STRIKE_TARGET         8         // Base difficulty to hit
#define BASE_PARRIES_PER_ROUND     2         // Default parries
#define BASE_ATTACKS_PER_ROUND     1         // Default attacks
#define CRITICAL_HIT_MULTIPLIER    2         // Critical damage multiplier

// =============================================================================
// ATTACK VERB LIBRARIES
// =============================================================================

// Dragon natural attacks
#define DRAGON_VERBS ({ \
    "bites", \
   "claws", \
    "rakes with claws", \
    "tail whips", \
    "slashes with talons", \
    "strikes with massive claws", \
    "snaps at", \
    "mauls" \
})

// Humanoid unarmed combat
#define UNARMED_VERBS ({ \
    "punches", \
    "kicks", \
    "strikes", \
    "jabs", \
    "uppercuts", \
    "roundhouse kicks", \
    "elbows", \
    "headbutts", \
    "knee strikes" \
})

// Armed melee - Blade weapons (swords, knives, etc.)
#define BLADE_VERBS ({ \
    "slashes", \
    "stabs", \
    "thrusts", \
    "cuts", \
    "slices", \
    "carves into", \
    "impales", \
    "hacks at" \
})

// Armed melee - Blunt weapons (clubs, hammers, etc.)
#define BLUNT_VERBS ({ \
    "swings at", \
    "bashes", \
    "smashes", \
    "clubs", \
    "hammers", \
    "crushes", \
    "pounds", \
    "batters" \
})

// Armed melee - Polearms (spears, halberds, etc.)
#define POLEARM_VERBS ({ \
    "thrusts at", \
    "stabs", \
    "jabs", \
    "skewers", \
    "pierces", \
    "lances" \
})

// Armed ranged - Energy weapons
#define ENERGY_VERBS ({ \
    "shoots", \
    "fires at", \
    "blasts", \
    "zaps", \
    "energy blasts", \
    "unleashes a beam at", \
    "vaporizes" \
})

// Armed ranged - Projectile weapons (guns, bows)
#define PROJECTILE_VERBS ({ \
    "shoots", \
    "fires at", \
    "launches at", \
    "peppers", \
    "riddled with bullets", \
    "unloads into" \
})

// Cyborg/robot built-in weapons
#define CYBORG_VERBS ({ \
    "fires at", \
    "shoots", \
    "blasts", \
    "slams into", \
    "hydraulic punches", \
    "laser blasts" \
})

// Supernatural/magic natural attacks
#define SUPERNATURAL_VERBS ({ \
    "blasts", \
    "strikes", \
    "smites", \
    "channels energy at", \
    "unleashes power at", \
    "tears into" \
})

// Animal natural attacks
#define ANIMAL_VERBS ({ \
    "bites", \
    "claws", \
    "mauls", \
    "gores", \
    "tramples", \
    "pounces on" \
})

// Monster/creature attacks
#define MONSTER_VERBS ({ \
    "savages", \
    "rends", \
    "tears into", \
    "eviscerates", \
    "shreds", \
    "devours" \
})

// =============================================================================
// DEFENSE VERB LIBRARIES
// =============================================================================

#define PARRY_VERBS ({ \
    "parries", \
    "blocks", \
    "deflects", \
    "turns aside", \
    "intercepts" \
})

#define DODGE_VERBS ({ \
    "dodges", \
    "evades", \
    "sidesteps", \
    "ducks under", \
    "rolls away from", \
    "leaps aside from" \
})

// =============================================================================
// COMBAT MESSAGE TEMPLATES
// =============================================================================

// Critical hit messages
#define CRITICAL_MSGS ({ \
    "CRITICAL HIT!", \
    "DEVASTATING BLOW!", \
    "MASSIVE STRIKE!", \
    "BRUTAL HIT!" \
})

// Fumble messages
#define FUMBLE_MSGS ({ \
    "fumbles", \
    "loses balance", \
    "trips over themselves", \
    "swings wildly and misses" \
})

// Miss messages  
#define MISS_MSGS ({ \
    "but misses completely", \
    "but the attack goes wide", \
    "but fails to connect", \
    "but the strike is off-target" \
})

// =============================================================================
// HELPER FUNCTIONS (to be implemented in combat daemon)
// =============================================================================

// string select_attack_verb(string archetype, object weapon)
// string select_defense_verb(int defense_type)
// string format_combat_message(object attacker, object defender, string verb, int result)

// =============================================================================
// NATURAL WEAPON DEFINITIONS
// =============================================================================

// Structure for natural weapons (claws, bite, etc.)
// Used by races to define their natural attack capabilities
//
// Example usage in race file:
// add_natural_weapon("bite", 2, 6, "The dragon's powerful jaws")
// add_natural_weapon("claw", 1, 8, "Razor-sharp talons")

// =============================================================================
// DAMAGE TYPES
// =============================================================================

#define DAMAGE_SDC                 0         // Structural Damage Capacity
#define DAMAGE_MDC                 1         // Mega-Damage Capacity
#define DAMAGE_PSIONIC             2         // Psionic/mental damage
#define DAMAGE_MAGIC               3         // Magical damage

// =============================================================================
// COMBAT STATUS FLAGS
// =============================================================================

#define STATUS_NORMAL              0
#define STATUS_STUNNED             1
#define STATUS_PRONE               2
#define STATUS_GRAPPLED            3
#define STATUS_DISARMED            4
#define STATUS_FLEEING             5

#endif // __COMBAT_H__
