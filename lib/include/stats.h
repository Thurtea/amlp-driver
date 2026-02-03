// /lib/include/stats.h
// Stat definitions for the Rifts/Palladium character system

#ifndef __STATS_H__
#define __STATS_H__

// Primary Attributes (Palladium System)
#define STAT_IQ          "iq"           // Intelligence Quotient
#define STAT_ME          "me"           // Mental Endurance
#define STAT_MA          "ma"           // Mental Affinity
#define STAT_PS          "ps"           // Physical Strength
#define STAT_PP          "pp"           // Physical Prowess
#define STAT_PE          "pe"           // Physical Endurance
#define STAT_PB          "pb"           // Physical Beauty
#define STAT_SPD         "spd"          // Speed

// Secondary Attributes (derived)
#define STAT_HP          "hp"           // Hit Points
#define STAT_SDC         "sdc"          // Structural Damage Capacity
#define STAT_MDC         "mdc"          // Mega-Damage Capacity
#define STAT_ISP         "isp"          // Inner Strength Points (psionics)
#define STAT_PPE         "ppe"          // Potential Psychic Energy (magic)

// Combat Stats
#define STAT_STRIKE      "strike"       // Combat strike bonus
#define STAT_PARRY       "parry"        // Combat parry bonus
#define STAT_DODGE       "dodge"        // Combat dodge bonus
#define STAT_ROLL        "roll"         // Roll with impact
#define STAT_PULL        "pull"         // Pull punch
#define STAT_INITIATIVE  "initiative"   // Initiative bonus
#define STAT_ATTACKS     "attacks"      // Attacks per melee

// Saving Throws
#define SAVE_VS_MAGIC    "save_magic"
#define SAVE_VS_POISON   "save_poison"
#define SAVE_VS_PSIONICS "save_psionics"
#define SAVE_VS_HORROR   "save_horror"
#define SAVE_VS_INSANITY "save_insanity"
#define SAVE_VS_DISEASE  "save_disease"

// Size categories
#define SIZE_TINY        1              // Under 1 foot
#define SIZE_SMALL       2              // 1-3 feet
#define SIZE_MEDIUM      3              // 4-7 feet (human range)
#define SIZE_LARGE       4              // 8-12 feet
#define SIZE_HUGE        5              // 13-20 feet
#define SIZE_GIANT       6              // 21-40 feet
#define SIZE_COLOSSAL    7              // 41+ feet

// Stat array indices (for quick access)
#define STAT_NAMES ({ "iq", "me", "ma", "ps", "pp", "pe", "pb", "spd" })

// Default base stats
#define DEFAULT_STAT_VALUE 10
#define MIN_STAT_VALUE     1
#define MAX_STAT_VALUE     30
#define EXCEPTIONAL_THRESHOLD 16
#define SUPERHUMAN_THRESHOLD  24

// Dice rolling helpers
#define ROLL_3D6         "3d6"          // Standard stat roll
#define ROLL_4D6_DROP    "4d6d1"        // 4d6, drop lowest
#define ROLL_1D6         "1d6"          // Single d6

#endif
