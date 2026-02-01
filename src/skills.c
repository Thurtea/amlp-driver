/* skills.c - Rifts RPG Skill System Implementation */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "skills.h"

/* External declarations */
extern void send_to_player(PlayerSession *session, const char *format, ...);
extern const RaceOCCInfo ALL_OCCS[];  /* Defined in chargen.c */

/* ========== SKILL DATABASE ========== */

/* All available skills - 15 core skills */
static const SkillDef SKILL_DATABASE[] = {
    /* Physical Skills (0-2) */
    {
        .name = "Hand to Hand - Basic",
        .category = "Physical",
        .description = "Basic martial arts and unarmed combat",
        .base_percentage = 40,
        .modifier_stat = 'P'  /* PP: Physical Power */
    },
    {
        .name = "Acrobatics",
        .category = "Physical",
        .description = "Dodge, tumble, balance, parkour",
        .base_percentage = 30,
        .modifier_stat = 'P'  /* PP: Physical Power */
    },
    {
        .name = "Swimming",
        .category = "Physical",
        .description = "Aquatic movement and survival",
        .base_percentage = 35,
        .modifier_stat = 'P'  /* PP: Physical Power */
    },
    
    /* Technical Skills (3-6) */
    {
        .name = "Computer Operations",
        .category = "Technical",
        .description = "Operating computers, hacking, data access",
        .base_percentage = 45,
        .modifier_stat = 'I'  /* IQ: Intellect */
    },
    {
        .name = "Mechanics",
        .category = "Technical",
        .description = "Vehicle and robot repair/maintenance",
        .base_percentage = 40,
        .modifier_stat = 'I'  /* IQ: Intellect */
    },
    {
        .name = "Electronics",
        .category = "Technical",
        .description = "Device creation, repair, modification",
        .base_percentage = 40,
        .modifier_stat = 'I'  /* IQ: Intellect */
    },
    {
        .name = "Literacy",
        .category = "Technical",
        .description = "Reading, writing, language comprehension",
        .base_percentage = 50,
        .modifier_stat = 'I'  /* IQ: Intellect */
    },
    
    /* Weapon Proficiencies (7-9) */
    {
        .name = "WP Sword",
        .category = "Weapon",
        .description = "Proficiency with swords and bladed melee weapons",
        .base_percentage = 50,
        .modifier_stat = 'P'  /* PP: Physical Power */
    },
    {
        .name = "WP Rifle",
        .category = "Weapon",
        .description = "Proficiency with energy rifles and heavy guns",
        .base_percentage = 50,
        .modifier_stat = 'P'  /* PP: Physical Power */
    },
    {
        .name = "WP Pistol",
        .category = "Weapon",
        .description = "Proficiency with energy pistols and sidearms",
        .base_percentage = 45,
        .modifier_stat = 'P'  /* PP: Physical Power */
    },
    
    /* Medical Skills (10-11) */
    {
        .name = "First Aid",
        .category = "Medical",
        .description = "Basic healing and injury treatment",
        .base_percentage = 40,
        .modifier_stat = 'M'  /* MA: Mental Affinity */
    },
    {
        .name = "Paramedic",
        .category = "Medical",
        .description = "Advanced healing and critical care",
        .base_percentage = 35,
        .modifier_stat = 'M'  /* MA: Mental Affinity */
    },
    
    /* Wilderness Skills (12-13) */
    {
        .name = "Survival",
        .category = "Wilderness",
        .description = "Wilderness survival and scavenging",
        .base_percentage = 35,
        .modifier_stat = 'E'  /* ME: Mental Endurance */
    },
    {
        .name = "Tracking",
        .category = "Wilderness",
        .description = "Tracking and hunting targets",
        .base_percentage = 40,
        .modifier_stat = 'E'  /* ME: Mental Endurance */
    },
    
    /* Magical/Psionic Foundation (14-15) */
    {
        .name = "Magic - Novice",
        .category = "Magical",
        .description = "Foundation for magical spellcasting",
        .base_percentage = 50,
        .modifier_stat = 'E'  /* ME: Mental Endurance */
    },
    {
        .name = "Psionics - Novice",
        .category = "Psionic",
        .description = "Foundation for psionic powers",
        .base_percentage = 50,
        .modifier_stat = 'E'  /* ME: Mental Endurance */
    }
};

/* Total skills in database */
#define TOTAL_SKILLS (sizeof(SKILL_DATABASE) / sizeof(SkillDef))

/* ========== OCC SKILL PACKAGES ========== */

/* Helper macro for skill packages */
#define SKILL_PACKAGE(name, skills_array, isp, ppe) \
    { .skill_ids = skills_array, .num_skills = sizeof(skills_array)/sizeof(int), .starting_isp = isp, .starting_ppe = ppe }

/* Skill ID mapping for readability */
#define HAND_TO_HAND 0
#define ACROBATICS 1
#define SWIMMING 2
#define COMPUTER_OPS 3
#define MECHANICS 4
#define ELECTRONICS 5
#define LITERACY 6
#define WP_SWORD 7
#define WP_RIFLE 8
#define WP_PISTOL 9
#define FIRST_AID 10
#define PARAMEDIC 11
#define SURVIVAL 12
#define TRACKING 13
#define MAGIC_NOVICE 14
#define PSIONICS_NOVICE 15

/* OCC Skill Packages for all 35 OCCs */
static OCCSkillPackage OCC_SKILL_PACKAGES[] = {
    /* 0: Cyber-Knight - Combat + Psionics + Tech */
    {
        .skill_ids = {HAND_TO_HAND, WP_SWORD, WP_PISTOL, COMPUTER_OPS, PSIONICS_NOVICE},
        .skill_percentages = {65, 70, 55, 45, 40},
        .num_skills = 5,
        .starting_isp = 20,  /* 3d6 psi points */
        .starting_ppe = 5
    },
    
    /* 1: Ley Line Walker - Magic + Knowledge */
    {
        .skill_ids = {HAND_TO_HAND, MAGIC_NOVICE, LITERACY, SURVIVAL, WP_PISTOL},
        .skill_percentages = {30, 80, 70, 45, 35},
        .num_skills = 5,
        .starting_isp = 5,
        .starting_ppe = 40  /* Heavy PPE */
    },
    
    /* 2: Rogue Scientist - Technical Master */
    {
        .skill_ids = {COMPUTER_OPS, ELECTRONICS, MECHANICS, LITERACY, HAND_TO_HAND},
        .skill_percentages = {75, 70, 65, 80, 35},
        .num_skills = 5,
        .starting_isp = 5,
        .starting_ppe = 5
    },
    
    /* 3: Techno-Wizard - Magic + Technology */
    {
        .skill_ids = {MAGIC_NOVICE, COMPUTER_OPS, ELECTRONICS, LITERACY, WP_PISTOL},
        .skill_percentages = {70, 60, 65, 75, 40},
        .num_skills = 5,
        .starting_isp = 5,
        .starting_ppe = 35
    },
    
    /* 4: Battle Magus - Combat + Magic */
    {
        .skill_ids = {HAND_TO_HAND, MAGIC_NOVICE, WP_SWORD, LITERACY, ACROBATICS},
        .skill_percentages = {60, 70, 70, 60, 45},
        .num_skills = 5,
        .starting_isp = 5,
        .starting_ppe = 30
    },
    
    /* 5: Biomancer - Medical + Magic */
    {
        .skill_ids = {MAGIC_NOVICE, PARAMEDIC, LITERACY, FIRST_AID, SURVIVAL},
        .skill_percentages = {65, 50, 70, 60, 40},
        .num_skills = 5,
        .starting_isp = 5,
        .starting_ppe = 35
    },
    
    /* 6: Body Fixer - Medical + Technical */
    {
        .skill_ids = {PARAMEDIC, ELECTRONICS, MECHANICS, FIRST_AID, COMPUTER_OPS},
        .skill_percentages = {70, 60, 55, 70, 45},
        .num_skills = 5,
        .starting_isp = 5,
        .starting_ppe = 5
    },
    
    /* 7: Burster - Psychic Warrior */
    {
        .skill_ids = {PSIONICS_NOVICE, HAND_TO_HAND, ACROBATICS, WP_PISTOL, LITERACY},
        .skill_percentages = {70, 60, 50, 50, 40},
        .num_skills = 5,
        .starting_isp = 35,  /* Heavy ISP */
        .starting_ppe = 5
    },
    
    /* 8: City Rat - Urban Rogue */
    {
        .skill_ids = {HAND_TO_HAND, TRACKING, ACROBATICS, COMPUTER_OPS, SURVIVAL},
        .skill_percentages = {50, 55, 60, 50, 40},
        .num_skills = 5,
        .starting_isp = 5,
        .starting_ppe = 5
    },
    
    /* 9: Cosmo-Knight - Cosmic Knight */
    {
        .skill_ids = {HAND_TO_HAND, WP_SWORD, PSIONICS_NOVICE, LITERACY, MAGIC_NOVICE},
        .skill_percentages = {65, 70, 55, 60, 40},
        .num_skills = 5,
        .starting_isp = 25,
        .starting_ppe = 20
    },
    
    /* 10: Crazy - Augmented Warrior */
    {
        .skill_ids = {HAND_TO_HAND, WP_RIFLE, WP_SWORD, ACROBATICS, TRACKING},
        .skill_percentages = {75, 70, 65, 55, 45},
        .num_skills = 5,
        .starting_isp = 5,
        .starting_ppe = 5
    },
    
    /* 11: Dragon Hatchling RCC - Young Dragon */
    {
        .skill_ids = {HAND_TO_HAND, MAGIC_NOVICE, PSIONICS_NOVICE, LITERACY, SURVIVAL},
        .skill_percentages = {55, 50, 50, 50, 40},
        .num_skills = 5,
        .starting_isp = 20,
        .starting_ppe = 40
    },
    
    /* 12: Elemental Fusionist - Elemental Mage */
    {
        .skill_ids = {MAGIC_NOVICE, LITERACY, SURVIVAL, FIRST_AID, ACROBATICS},
        .skill_percentages = {75, 70, 50, 40, 45},
        .num_skills = 5,
        .starting_isp = 5,
        .starting_ppe = 50
    },
    
    /* 13: Full Conversion Borg - Cyborg */
    {
        .skill_ids = {MECHANICS, ELECTRONICS, COMPUTER_OPS, HAND_TO_HAND, WP_RIFLE},
        .skill_percentages = {65, 60, 50, 70, 65},
        .num_skills = 5,
        .starting_isp = 5,
        .starting_ppe = 5
    },
    
    /* 14: Headhunter - Bounty Hunter */
    {
        .skill_ids = {TRACKING, HAND_TO_HAND, WP_PISTOL, WP_RIFLE, ACROBATICS},
        .skill_percentages = {60, 55, 60, 60, 50},
        .num_skills = 5,
        .starting_isp = 5,
        .starting_ppe = 5
    },
    
    /* 15: Juicer - Chemical Warrior */
    {
        .skill_ids = {HAND_TO_HAND, WP_RIFLE, WP_SWORD, ACROBATICS, TRACKING},
        .skill_percentages = {75, 70, 70, 60, 50},
        .num_skills = 5,
        .starting_isp = 5,
        .starting_ppe = 5
    },
    
    /* 16: Line Walker - Ley Line Master */
    {
        .skill_ids = {MAGIC_NOVICE, LITERACY, SURVIVAL, PSIONICS_NOVICE, FIRST_AID},
        .skill_percentages = {70, 75, 50, 40, 40},
        .num_skills = 5,
        .starting_isp = 10,
        .starting_ppe = 45
    },
    
    /* 17: Mercenary - Professional Soldier */
    {
        .skill_ids = {HAND_TO_HAND, WP_RIFLE, WP_SWORD, TRACKING, SURVIVAL},
        .skill_percentages = {65, 70, 60, 55, 45},
        .num_skills = 5,
        .starting_isp = 5,
        .starting_ppe = 5
    },
    
    /* 18: Mind Melter - Psychic Master */
    {
        .skill_ids = {PSIONICS_NOVICE, LITERACY, ACROBATICS, FIRST_AID, COMPUTER_OPS},
        .skill_percentages = {75, 65, 50, 45, 45},
        .num_skills = 5,
        .starting_isp = 40,
        .starting_ppe = 5
    },
    
    /* 19: Mystic - Spiritual Magic User */
    {
        .skill_ids = {MAGIC_NOVICE, LITERACY, FIRST_AID, SURVIVAL, PSIONICS_NOVICE},
        .skill_percentages = {70, 70, 50, 50, 40},
        .num_skills = 5,
        .starting_isp = 15,
        .starting_ppe = 35
    },
    
    /* 20: Necromancer - Death Magic */
    {
        .skill_ids = {MAGIC_NOVICE, PSIONICS_NOVICE, LITERACY, SURVIVAL, HAND_TO_HAND},
        .skill_percentages = {75, 50, 75, 45, 35},
        .num_skills = 5,
        .starting_isp = 20,
        .starting_ppe = 40
    },
    
    /* 21: Ninja - Silent Warrior */
    {
        .skill_ids = {HAND_TO_HAND, WP_SWORD, ACROBATICS, TRACKING, PSIONICS_NOVICE},
        .skill_percentages = {70, 70, 65, 60, 40},
        .num_skills = 5,
        .starting_isp = 15,
        .starting_ppe = 5
    },
    
    /* 22: Operator - Vehicle Expert */
    {
        .skill_ids = {MECHANICS, COMPUTER_OPS, ELECTRONICS, HAND_TO_HAND, WP_PISTOL},
        .skill_percentages = {70, 60, 55, 40, 45},
        .num_skills = 5,
        .starting_isp = 5,
        .starting_ppe = 5
    },
    
    /* 23: Power Armor Pilot - Mecha Warrior */
    {
        .skill_ids = {MECHANICS, WP_RIFLE, HAND_TO_HAND, COMPUTER_OPS, ACROBATICS},
        .skill_percentages = {60, 70, 55, 50, 45},
        .num_skills = 5,
        .starting_isp = 5,
        .starting_ppe = 5
    },
    
    /* 24: Psi-Stalker - Psychic Tracker */
    {
        .skill_ids = {PSIONICS_NOVICE, TRACKING, SURVIVAL, HAND_TO_HAND, ACROBATICS},
        .skill_percentages = {60, 70, 60, 55, 50},
        .num_skills = 5,
        .starting_isp = 30,
        .starting_ppe = 5
    },
    
    /* 25: Psychic - Psionic Master */
    {
        .skill_ids = {PSIONICS_NOVICE, LITERACY, FIRST_AID, ACROBATICS, HAND_TO_HAND},
        .skill_percentages = {75, 65, 45, 50, 40},
        .num_skills = 5,
        .starting_isp = 40,
        .starting_ppe = 5
    },
    
    /* 26: Rifter - Dimensional Traveler */
    {
        .skill_ids = {MAGIC_NOVICE, LITERACY, SURVIVAL, COMPUTER_OPS, PSIONICS_NOVICE},
        .skill_percentages = {65, 70, 50, 50, 45},
        .num_skills = 5,
        .starting_isp = 20,
        .starting_ppe = 40
    },
    
    /* 27: Rogue Scholar - Knowledge Master */
    {
        .skill_ids = {LITERACY, COMPUTER_OPS, SURVIVAL, FIRST_AID, MAGIC_NOVICE},
        .skill_percentages = {80, 60, 45, 45, 40},
        .num_skills = 5,
        .starting_isp = 5,
        .starting_ppe = 20
    },
    
    /* 28: Shifter - Beast Master */
    {
        .skill_ids = {PSIONICS_NOVICE, HAND_TO_HAND, SURVIVAL, TRACKING, ACROBATICS},
        .skill_percentages = {65, 60, 60, 60, 55},
        .num_skills = 5,
        .starting_isp = 35,
        .starting_ppe = 5
    },
    
    /* 29: Temporal Wizard - Time Mage */
    {
        .skill_ids = {MAGIC_NOVICE, PSIONICS_NOVICE, LITERACY, COMPUTER_OPS, ACROBATICS},
        .skill_percentages = {75, 50, 75, 55, 45},
        .num_skills = 5,
        .starting_isp = 15,
        .starting_ppe = 45
    },
    
    /* 30: Undead Slayer - Supernatural Hunter */
    {
        .skill_ids = {HAND_TO_HAND, WP_SWORD, MAGIC_NOVICE, TRACKING, FIRST_AID},
        .skill_percentages = {65, 70, 55, 60, 50},
        .num_skills = 5,
        .starting_isp = 10,
        .starting_ppe = 30
    },
    
    /* 31: Vagabond - Free Wanderer */
    {
        .skill_ids = {SURVIVAL, HAND_TO_HAND, TRACKING, ACROBATICS, LITERACY},
        .skill_percentages = {60, 50, 55, 55, 50},
        .num_skills = 5,
        .starting_isp = 5,
        .starting_ppe = 5
    },
    
    /* 32: Warrior Monk - Disciplined Fighter */
    {
        .skill_ids = {HAND_TO_HAND, ACROBATICS, PSIONICS_NOVICE, LITERACY, FIRST_AID},
        .skill_percentages = {75, 65, 45, 55, 50},
        .num_skills = 5,
        .starting_isp = 20,
        .starting_ppe = 5
    },
    
    /* 33: Wilderness Scout - Nature Expert */
    {
        .skill_ids = {SURVIVAL, TRACKING, HAND_TO_HAND, WP_RIFLE, ACROBATICS},
        .skill_percentages = {70, 70, 50, 60, 55},
        .num_skills = 5,
        .starting_isp = 5,
        .starting_ppe = 5
    },
    
    /* 34: Zapper - Energy Blaster */
    {
        .skill_ids = {WP_PISTOL, WP_RIFLE, COMPUTER_OPS, ELECTRONICS, ACROBATICS},
        .skill_percentages = {70, 75, 50, 50, 55},
        .num_skills = 5,
        .starting_isp = 5,
        .starting_ppe = 5
    }
};

/* ========== GLOBAL EXPORTS ========== */

SkillDef ALL_SKILLS[TOTAL_SKILLS];
int NUM_SKILLS = TOTAL_SKILLS;
OCCSkillPackage OCC_PACKAGES[35];

/* ========== INITIALIZATION ========== */

void skill_init(void) {
    int i;
    
    /* Copy skill database */
    for (i = 0; i < TOTAL_SKILLS; i++) {
        ALL_SKILLS[i] = SKILL_DATABASE[i];
    }
    
    /* Copy OCC packages */
    for (i = 0; i < 35; i++) {
        OCC_PACKAGES[i] = OCC_SKILL_PACKAGES[i];
    }
    
    fprintf(stderr, "[Skills] Initialized %d skills for 35 OCCs\n", TOTAL_SKILLS);
}

/* ========== SKILL LOOKUPS ========== */

SkillDef *skill_get_by_id(int id) {
    if (id < 0 || id >= TOTAL_SKILLS) return NULL;
    return &ALL_SKILLS[id];
}

SkillDef *skill_get_by_name(const char *name) {
    int i;
    
    if (!name || !name[0]) return NULL;
    
    for (i = 0; i < TOTAL_SKILLS; i++) {
        if (strcasecmp(ALL_SKILLS[i].name, name) == 0) {
            return &ALL_SKILLS[i];
        }
    }
    
    return NULL;
}

int skill_get_id_by_name(const char *name) {
    int i;
    
    if (!name || !name[0]) return -1;
    
    for (i = 0; i < TOTAL_SKILLS; i++) {
        if (strcasecmp(ALL_SKILLS[i].name, name) == 0) {
            return i;
        }
    }
    
    return -1;
}

const char *skill_get_name(int skill_id) {
    if (skill_id < 0 || skill_id >= TOTAL_SKILLS) return "Unknown";
    return ALL_SKILLS[skill_id].name;
}

/* ========== OCC SKILL ASSIGNMENT ========== */

void occ_assign_skills(PlayerSession *sess, const char *occ_name) {
    int occ_idx = 0;
    OCCSkillPackage *package;
    int i, j;
    
    if (!sess || !occ_name) return;
    
    /* Find OCC index by name */
    for (occ_idx = 0; occ_idx < 35; occ_idx++) {
        if (strcasecmp(ALL_OCCS[occ_idx].name, occ_name) == 0) {
            break;
        }
    }
    
    if (occ_idx >= 35) {
        fprintf(stderr, "[Skills] Unknown OCC: %s\n", occ_name);
        return;
    }
    
    package = &OCC_PACKAGES[occ_idx];
    
    /* Clear existing skills */
    memset(sess->character.skills, 0, sizeof(sess->character.skills));
    
    /* Assign skills from package */
    for (i = 0; i < package->num_skills && i < MAX_PLAYER_SKILLS; i++) {
        sess->character.skills[i].skill_id = package->skill_ids[i];
        sess->character.skills[i].percentage = package->skill_percentages[i];
        sess->character.skills[i].uses = 0;
    }
    
    sess->character.num_skills = package->num_skills;
    
    /* Set PSIs/PPE (these are initial pool amounts - will be system dependent) */
    /* Temporary: just store the pool sizes */
    sess->chargen_temp_choice = package->starting_isp;  /* Reuse for ISP pool */
}

/* ========== SKILL CHECKS ========== */

int skill_check(int skill_percentage) {
    int roll = (rand() % 100) + 1;
    return (roll <= skill_percentage) ? 1 : 0;
}

/* ========== DISPLAY FUNCTIONS ========== */

void skill_display_list(PlayerSession *sess) {
    int i, skill_id;
    SkillDef *skill;
    
    if (!sess) return;
    
    send_to_player(sess, "\n\033[1;32m=== YOUR SKILLS ===\033[0m\n\n");
    
    if (sess->character.num_skills == 0) {
        send_to_player(sess, "No skills learned yet.\n");
        return;
    }
    
    for (i = 0; i < sess->character.num_skills; i++) {
        skill_id = sess->character.skills[i].skill_id;
        skill = skill_get_by_id(skill_id);
        
        if (skill) {
            send_to_player(sess, "  %-30s %3d%%\n", 
                          skill->name, 
                          sess->character.skills[i].percentage);
        }
    }
    
    send_to_player(sess, "\n");
}

