#ifndef COMBAT_H
#define COMBAT_H

#include <stdbool.h>
#include "chargen.h"
#include "session_internal.h"

// ============================================================================
// COMBAT SYSTEM - Rifts RPG Turn-Based Combat
// ============================================================================

// Combat state machine
typedef enum {
    COMBAT_NONE = 0,        // Not in combat
    COMBAT_INITIATIVE,      // Rolling initiative
    COMBAT_ACTIVE,          // Taking turns
    COMBAT_ENDED            // Combat finished
} CombatState;

// Combat participant (player or NPC)
typedef struct CombatParticipant {
    char *name;                           // Name of participant
    bool is_player;                       // True if player, false if NPC
    PlayerSession *session;               // Player session (NULL for NPCs)
    Character *character;                 // Character data
    
    // Combat state
    int initiative;                       // Initiative roll (SPD + 1d20)
    int actions_remaining;                // Actions left this round
    bool is_defending;                    // Currently in defensive stance
    int parries_remaining;                // Parries left this round
    
    // Target tracking
    struct CombatParticipant *target;     // Current attack target
    
    // Linked list
    struct CombatParticipant *next;
} CombatParticipant;

// Combat round manager
typedef struct CombatRound {
    CombatState state;                    // Current combat state
    CombatParticipant *participants;      // Linked list of combatants
    CombatParticipant *current;           // Current turn participant
    int round_number;                     // Combat round counter
    int num_participants;                 // Number of active combatants
} CombatRound;

// Damage result
typedef struct {
    int damage;                           // Total damage dealt
    bool is_critical;                     // Critical hit flag
    bool is_kill;                         // Target killed flag
    int sdc_damage;                       // Damage to SDC
    int hp_damage;                        // Damage to HP
} DamageResult;

// ============================================================================
// COMBAT SYSTEM API
// ============================================================================

// Initialization
void combat_init(void);

// Combat round management
CombatRound* combat_start(CombatParticipant *initiator, CombatParticipant *target);
void combat_end(CombatRound *combat);
CombatRound* combat_get_active(PlayerSession *sess);

// Participant management
CombatParticipant* combat_create_participant(PlayerSession *sess, Character *ch);
void combat_free_participant(CombatParticipant *p);
void combat_add_participant(CombatRound *combat, CombatParticipant *p);
void combat_remove_participant(CombatRound *combat, CombatParticipant *p);
CombatParticipant* combat_find_participant(CombatRound *combat, const char *name);

// Initiative system
void combat_roll_initiative(CombatRound *combat);
void combat_next_turn(CombatRound *combat);
void combat_display_initiative(CombatRound *combat);

// Attack system
DamageResult combat_attack_melee(CombatParticipant *attacker, CombatParticipant *defender);
DamageResult combat_attack_ranged(CombatParticipant *attacker, CombatParticipant *defender);
int combat_calculate_strike_bonus(CombatParticipant *p, bool is_melee);
int combat_calculate_damage(int weapon_dice, int weapon_sides, int ps_bonus);

// Defense system
bool combat_defend_parry(CombatParticipant *defender, int attack_roll);
bool combat_defend_dodge(CombatParticipant *defender, int attack_roll);
int combat_calculate_parry_bonus(CombatParticipant *p);
int combat_calculate_dodge_bonus(CombatParticipant *p);

// Damage application
void combat_apply_damage(CombatParticipant *target, DamageResult *dmg);
bool combat_check_death(CombatParticipant *p);
void combat_award_experience(CombatParticipant *winner, CombatParticipant *loser);

// Combat messages
void combat_broadcast(CombatRound *combat, const char *message);
void combat_send_to_participant(CombatParticipant *p, const char *message);

// Utility
int combat_roll_dice(int num_dice, int sides);
int combat_d20(void);

#endif // COMBAT_H
