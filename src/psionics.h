#ifndef PSIONICS_H
#define PSIONICS_H

#include <stdbool.h>
#include <time.h>

/* Forward declarations */
typedef struct PlayerSession PlayerSession;
struct Character;

/* Psionic Power Categories */
typedef enum {
    PSION_SUPER,
    PSION_HEALING,
    PSION_PHYSICAL,
    PSION_SENSITIVE,
    PSION_TELEPATHY
} PsionicCategory;

/* Psionic power structure */
typedef struct {
    int id;                         /* Unique ID (0-24) */
    char *name;                     /* "Mind Block" */
    char *description;
    int isp_cost;                   /* Cost to activate */
    int isp_cost_per_round;         /* 0 if instant, >0 if sustained */
    int duration_rounds;            /* 0=instant, -1=until dispelled */
    int base_damage;                /* For damage powers */
    int damage_dice;
    int damage_sides;
    bool is_mega_damage;            /* MDC vs SDC */
    int range_feet;                 /* 0=touch, X=distance */
    int area_effect_feet;           /* 0=single target */
    PsionicCategory category;
    bool is_combat_usable;          /* Can use in combat */
    bool is_passive;                /* Passive always-on ability */
    char *keywords;                 /* Search/filter keywords */
} PsionicPower;

/* Known power structure - what player character knows */
typedef struct {
    int power_id;                   /* ID of power in template array */
    int rank;                       /* 1-10 mastery level */
    int total_uses;                 /* How many times used (for advancement) */
    int last_activated;             /* Timestamp of last use */
} KnownPower;

/* Character psionic abilities */
typedef struct {
    KnownPower *powers;             /* Array of known powers */
    int power_count;                /* How many powers known */
    int isp_current;                /* Current ISP */
    int isp_max;                    /* Maximum ISP pool */
    int isp_base;                   /* Base ISP from attributes */
    int isp_recovery_rate;          /* ISP per round naturally */
    bool is_meditating;             /* In meditation mode */
    int meditation_rounds_active;   /* Rounds of meditation remaining */
} PsionicAbilities;

/* =============== PSIONICS API =============== */

/* Initialization */
void psionics_init(void);

/* Power lookup and functions */
PsionicPower* psionics_find_power_by_id(int power_id);
PsionicPower* psionics_find_power_by_name(const char *name);
KnownPower* psionics_find_known_power(struct Character *ch, int power_id);

/* Ability management */
void psionics_init_abilities(struct Character *ch);
void psionics_free_abilities(struct Character *ch);
void psionics_learn_power(struct Character *ch, int power_id);
void psionics_add_starting_powers(struct Character *ch, const char *occ_name);

/* ISP management */
int psionics_get_isp_max(struct Character *ch);
int psionics_get_isp_current(struct Character *ch);
void psionics_set_isp_current(struct Character *ch, int amount);
void psionics_recover_isp(struct Character *ch, int amount);
void psionics_spend_isp(struct Character *ch, int amount);
bool psionics_can_use_power(struct Character *ch, int power_id);

/* Power activation */
bool psionics_activate_power(PlayerSession *sess, struct Character *ch, 
                             int power_id, const char *target_name);
void psionics_power_tick(struct Character *ch);
void psionics_meditate_tick(struct Character *ch);

/* Display */
void psionics_display_powers(PlayerSession *sess);
void psionics_display_isp(PlayerSession *sess);

/* Power advancement */
void psionics_record_power_use(struct Character *ch, int power_id);
void psionics_check_power_rank_advance(struct Character *ch, int power_id);

/* =============== POWER TEMPLATES ARRAY =============== */
extern PsionicPower PSION_POWERS[25];
extern int PSIONICS_POWER_COUNT;

#endif /* PSIONICS_H */
