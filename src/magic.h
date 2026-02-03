#ifndef MAGIC_H
#define MAGIC_H

#include <stdbool.h>
#include <time.h>

/* Forward declarations */
typedef struct PlayerSession PlayerSession;
struct Character;

/* Magic Spell Schools */
typedef enum {
    SPELL_WARLOCK,
    SPELL_MYSTIC,
    SPELL_WIZARD,
    SPELL_BATTLE_MAGE,
    SPELL_RITUAL,
    SPELL_HYBRID
} SpellSchool;

/* Magic spell structure */
typedef struct {
    int id;                         /* Unique ID (0-33) */
    char *name;                     /* "Fireball" */
    char *description;
    int ppe_cost;                   /* Base cost to cast */
    int ppe_per_round;              /* 0 if instant, >0 if sustained */
    int duration_rounds;            /* 0=instant, -1=until dispelled */
    int base_damage;                /* For damage spells */
    int damage_dice;
    int damage_sides;
    bool is_mega_damage;            /* MDC vs SDC */
    int range_feet;                 /* 0=touch, X=distance, -1=self */
    int area_effect_feet;           /* 0=single target */
    SpellSchool school;
    char *level_name;               /* "Level 1", "Level 5", etc. */
    int casting_time_rounds;        /* How many rounds to cast */
    bool can_overwhelm;             /* Can spend extra PPE for power */
    bool is_ritual;                 /* Requires standing still */
    bool is_passive_bonus;          /* Permanent passive while active */
    char *keywords;                 /* Search/filter keywords */
} MagicSpell;

/* Known spell structure - what player character knows */
typedef struct {
    int spell_id;                   /* ID of spell in template array */
    int rank;                       /* 1-10 mastery level */
    int total_casts;                /* How many times cast (for advancement) */
    int last_cast;                  /* Timestamp of last casting */
} KnownSpell;

/* Character magic abilities */
typedef struct {
    KnownSpell *spells;             /* Array of known spells */
    int spell_count;                /* How many spells known */
    int ppe_current;                /* Current PPE */
    int ppe_max;                    /* Maximum PPE pool */
    int ppe_base;                   /* Base PPE from attributes */
    int ppe_recovery_rate;          /* PPE per round naturally */
    bool is_casting;                /* Currently casting a spell */
    int casting_spell_id;           /* Spell being cast */
    int casting_rounds_remaining;   /* Rounds until cast completes */
    bool is_meditating;             /* In meditation mode */
    int meditation_rounds_active;   /* Rounds of meditation remaining */
} MagicAbilities;

/* =============== MAGIC API =============== */

/* Initialization */
void magic_init(void);

/* Spell lookup and functions */
MagicSpell* magic_find_spell_by_id(int spell_id);
MagicSpell* magic_find_spell_by_name(const char *name);
KnownSpell* magic_find_known_spell(struct Character *ch, int spell_id);

/* Ability management */
void magic_init_abilities(struct Character *ch);
void magic_free_abilities(struct Character *ch);
void magic_learn_spell(struct Character *ch, int spell_id);
void magic_add_starting_spells(struct Character *ch, const char *occ_name);

/* PPE management */
int magic_get_ppe_max(struct Character *ch);
int magic_get_ppe_current(struct Character *ch);
void magic_set_ppe_current(struct Character *ch, int amount);
void magic_recover_ppe(struct Character *ch, int amount);
void magic_spend_ppe(struct Character *ch, int amount);
bool magic_can_cast_spell(struct Character *ch, int spell_id);

/* Spell casting */
bool magic_start_casting(PlayerSession *sess, struct Character *ch,
                         int spell_id, const char *target_name);
bool magic_continue_casting(struct Character *ch);
bool magic_complete_cast(PlayerSession *sess, struct Character *ch);
void magic_interrupt_cast(struct Character *ch);

/* Display */
void magic_display_spells(PlayerSession *sess);
void magic_display_ppe(PlayerSession *sess);

/* Spell advancement */
void magic_record_spell_cast(struct Character *ch, int spell_id);
void magic_check_spell_rank_advance(struct Character *ch, int spell_id);

/* Ticks */
void magic_spell_tick(struct Character *ch);
void magic_meditate_tick(struct Character *ch);

/* =============== SPELL TEMPLATES ARRAY =============== */
extern MagicSpell MAGIC_SPELLS[34];
extern int MAGIC_SPELL_COUNT;

#endif /* MAGIC_H */
