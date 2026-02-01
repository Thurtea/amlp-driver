#ifndef CHARGEN_H
#define CHARGEN_H

#include <stddef.h>

/* Forward declare from session_internal.h */
typedef struct PlayerSession PlayerSession;

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

#endif /* CHARGEN_H */
