#ifndef CHARGEN_H
#define CHARGEN_H

#include <stddef.h>

/* Forward declare from session_internal.h */
typedef struct PlayerSession PlayerSession;

/* Forward declare skill structures */
typedef struct {
    int skill_id;
    int percentage;
    int uses;
} PlayerSkill;

/* Character generation states */
typedef enum {
    CHARGEN_RACE_SELECT,
    CHARGEN_OCC_SELECT,
    CHARGEN_STATS_ROLL,
    CHARGEN_STATS_CONFIRM,
    CHARGEN_COMPLETE
} ChargenState;

/* Character stats (Palladium RPG) */
typedef struct {
    int iq;   /* Intelligence Quotient */
    int me;   /* Mental Endurance */
    int ma;   /* Mental Affinity */
    int ps;   /* Physical Strength */
    int pp;   /* Physical Prowess */
    int pe;   /* Physical Endurance */
    int pb;   /* Physical Beauty */
    int spd;  /* Speed */
} CharacterStats;

/* Character data */
typedef struct {
    char *race;
    char *occ;
    int level;
    int xp;
    int hp;
    int max_hp;
    int sdc;
    int max_sdc;
    int isp;   /* Inner Strength Points (psionics) */
    int max_isp;
    int ppe;   /* Potential Psychic Energy (magic) */
    int max_ppe;
    CharacterStats stats;
    
    /* Skills system (Phase 2) */
    PlayerSkill skills[20];     /* Max 20 skills per character */
    int num_skills;             /* Number of skills learned */
} Character;

/* Chargen initialization */
void chargen_init(PlayerSession *sess);

/* Chargen state machine */
void chargen_process_input(PlayerSession *sess, const char *input);

/* Helper functions */
int roll_3d6(void);
int roll_1d6(void);
void chargen_roll_stats(PlayerSession *sess);
void chargen_display_stats(PlayerSession *sess);
void chargen_complete(PlayerSession *sess);

/* Commands */
void cmd_stats(PlayerSession *sess, const char *args);
void cmd_skills(PlayerSession *sess, const char *args);
void cmd_attack(PlayerSession *sess, const char *args);
void cmd_strike(PlayerSession *sess, const char *args);
void cmd_shoot(PlayerSession *sess, const char *args);
void cmd_dodge(PlayerSession *sess, const char *args);
void cmd_flee(PlayerSession *sess, const char *args);

/* Character persistence */
int save_character(PlayerSession *sess);
int load_character(PlayerSession *sess, const char *username);
int character_exists(const char *username);

#endif /* CHARGEN_H */

