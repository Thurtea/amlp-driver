/* skills.h - Rifts RPG skill system definitions */
#ifndef __SKILLS_H__
#define __SKILLS_H__

#include "chargen.h"  /* For PlayerSkill typedef */
#include "session_internal.h"  /* For PlayerSession */

/* Race/OCC info structure (mirrors chargen.c) */
typedef struct {
    const char *name;
    const char *desc;
} RaceOCCInfo;

/* Maximum skills per character */
#define MAX_PLAYER_SKILLS 20

/* Skill categories */
typedef enum {
    SKILL_PHYSICAL,
    SKILL_TECHNICAL,
    SKILL_WEAPON,
    SKILL_MEDICAL,
    SKILL_WILDERNESS,
    SKILL_MAGICAL,
    SKILL_PSIONIC,
    SKILL_SOCIAL,
    SKILL_OTHER
} SkillCategory;

/* Skill definition - static database */
typedef struct {
    const char *name;           /* Unique skill name */
    const char *category;       /* Category (Physical, Technical, etc) */
    const char *description;    /* What the skill does */
    int base_percentage;        /* Base starting % for this skill */
    char modifier_stat;         /* Stat that modifies this ('P'=PP, 'S'=PS, 'M'=MA, 'E'=ME, etc) */
} SkillDef;

/* OCC skill package - what skills an OCC gets at char creation */
typedef struct {
    int skill_ids[10];          /* Skill indices for this OCC package */
    int skill_percentages[10];  /* Skill % for each in package */
    int num_skills;             /* How many skills in this package */
    int starting_isp;           /* Starting ISP pool (from 1d6) */
    int starting_ppe;           /* Starting PPE pool (from 1d6) */
} OCCSkillPackage;

/* Exported functions */
void skill_init(void);
SkillDef *skill_get_by_id(int id);
SkillDef *skill_get_by_name(const char *name);
int skill_get_id_by_name(const char *name);

void occ_assign_skills(PlayerSession *sess, const char *occ_name);
void skill_display_list(PlayerSession *sess);
int skill_check(int skill_percentage);
const char *skill_get_name(int skill_id);

/* Global skill database (defined in skills.c) */
extern SkillDef ALL_SKILLS[];
extern int NUM_SKILLS;
extern OCCSkillPackage OCC_PACKAGES[];

#endif /* __SKILLS_H__ */
