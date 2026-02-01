#include "combat.h"
#include "skills.h"
#include "session_internal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <strings.h>

// External function declarations
extern void send_to_player(PlayerSession *sess, const char *message);

// ============================================================================
// GLOBAL COMBAT STATE
// ============================================================================

static CombatRound *active_combats = NULL;  // List of active combat rounds
static bool combat_initialized = false;

// ============================================================================
// INITIALIZATION
// ============================================================================

void combat_init(void) {
    if (combat_initialized) {
        return;
    }
    
    srand(time(NULL));  // Seed random number generator
    active_combats = NULL;
    combat_initialized = true;
    
    printf("[COMBAT] Combat system initialized\n");
}

// ============================================================================
// UTILITY FUNCTIONS
// ============================================================================

int combat_roll_dice(int num_dice, int sides) {
    int total = 0;
    for (int i = 0; i < num_dice; i++) {
        total += (rand() % sides) + 1;
    }
    return total;
}

int combat_d20(void) {
    return combat_roll_dice(1, 20);
}

// ============================================================================
// PARTICIPANT MANAGEMENT
// ============================================================================

CombatParticipant* combat_create_participant(PlayerSession *sess, Character *ch) {
    if (!ch) return NULL;
    
    CombatParticipant *p = malloc(sizeof(CombatParticipant));
    if (!p) return NULL;
    
    p->name = strdup(sess && sess->username ? sess->username : "Unknown");
    p->is_player = (sess != NULL);
    p->session = sess;
    p->character = ch;
    
    p->initiative = 0;
    p->actions_remaining = 1;
    p->is_defending = false;
    p->parries_remaining = 2;  // Base 2 parries per round
    
    p->target = NULL;
    p->next = NULL;
    
    return p;
}

void combat_free_participant(CombatParticipant *p) {
    if (!p) return;
    
    if (p->name) {
        free(p->name);
    }
    free(p);
}

void combat_add_participant(CombatRound *combat, CombatParticipant *p) {
    if (!combat || !p) return;
    
    p->next = combat->participants;
    combat->participants = p;
    combat->num_participants++;
}

void combat_remove_participant(CombatRound *combat, CombatParticipant *p) {
    if (!combat || !p) return;
    
    // Remove from linked list
    if (combat->participants == p) {
        combat->participants = p->next;
    } else {
        CombatParticipant *curr = combat->participants;
        while (curr && curr->next != p) {
            curr = curr->next;
        }
        if (curr) {
            curr->next = p->next;
        }
    }
    
    combat->num_participants--;
    
    // If this was the current participant, advance turn
    if (combat->current == p) {
        combat_next_turn(combat);
    }
}

CombatParticipant* combat_find_participant(CombatRound *combat, const char *name) {
    if (!combat || !name) return NULL;
    
    for (CombatParticipant *p = combat->participants; p; p = p->next) {
        if (strcasecmp(p->name, name) == 0) {
            return p;
        }
    }
    return NULL;
}

// ============================================================================
// COMBAT ROUND MANAGEMENT
// ============================================================================

CombatRound* combat_start(CombatParticipant *initiator, CombatParticipant *target) {
    if (!initiator || !target) return NULL;
    
    CombatRound *combat = malloc(sizeof(CombatRound));
    if (!combat) return NULL;
    
    combat->state = COMBAT_INITIATIVE;
    combat->participants = NULL;
    combat->current = NULL;
    combat->round_number = 1;
    combat->num_participants = 0;
    
    // Add participants
    combat_add_participant(combat, initiator);
    combat_add_participant(combat, target);
    
    // Link to global active combats list
    // (For Phase 3, we'll only support one combat at a time)
    active_combats = combat;
    
    // Roll initiative
    combat_roll_initiative(combat);
    
    // Broadcast start message
    char msg[256];
    snprintf(msg, sizeof(msg), "\n\033[1;33m>>> COMBAT INITIATED! %s vs %s <<<\033[0m\n\n", 
             initiator->name, target->name);
    combat_broadcast(combat, msg);
    
    combat->state = COMBAT_ACTIVE;
    combat_display_initiative(combat);
    
    return combat;
}

void combat_end(CombatRound *combat) {
    if (!combat) return;
    
    combat_broadcast(combat, "\n\033[1;33m>>> COMBAT ENDED <<<\033[0m\n\n");
    
    // Free all participants
    CombatParticipant *p = combat->participants;
    while (p) {
        CombatParticipant *next = p->next;
        combat_free_participant(p);
        p = next;
    }
    
    // Remove from active combats
    if (active_combats == combat) {
        active_combats = NULL;
    }
    
    combat->state = COMBAT_ENDED;
    free(combat);
}

CombatRound* combat_get_active(PlayerSession *sess) {
    if (!sess) return NULL;
    
    // For Phase 3, we only support one combat at a time
    if (active_combats) {
        // Check if this session is in the active combat
        for (CombatParticipant *p = active_combats->participants; p; p = p->next) {
            if (p->session == sess) {
                return active_combats;
            }
        }
    }
    
    return NULL;
}

// ============================================================================
// INITIATIVE SYSTEM
// ============================================================================

void combat_roll_initiative(CombatRound *combat) {
    if (!combat) return;
    
    // Roll initiative for all participants (SPD + 1d20)
    for (CombatParticipant *p = combat->participants; p; p = p->next) {
        if (p->character) {
            int spd = p->character->stats.spd;
            int roll = combat_d20();
            p->initiative = spd + roll;
            
            // Bonus from Hand to Hand skill
            SkillDef *hth_skill = skill_get_by_name("Hand to Hand");
            if (hth_skill && p->character->num_skills > 0) {
                for (int i = 0; i < p->character->num_skills; i++) {
                    if (p->character->skills[i].skill_id == 0) {  // Hand to Hand is ID 0
                        int hth_bonus = p->character->skills[i].percentage / 20;
                        p->initiative += hth_bonus;
                        break;
                    }
                }
            }
        }
    }
    
    // Sort participants by initiative (highest first)
    // Simple bubble sort for small lists
    bool swapped;
    do {
        swapped = false;
        CombatParticipant **prev = &combat->participants;
        CombatParticipant *curr = combat->participants;
        
        while (curr && curr->next) {
            if (curr->initiative < curr->next->initiative) {
                // Swap
                CombatParticipant *temp = curr->next;
                curr->next = temp->next;
                temp->next = curr;
                *prev = temp;
                swapped = true;
            }
            prev = &(*prev)->next;
            curr = *prev;
        }
    } while (swapped);
    
    // Set current to first participant
    combat->current = combat->participants;
}

void combat_next_turn(CombatRound *combat) {
    if (!combat) return;
    
    // Move to next participant
    if (combat->current) {
        combat->current = combat->current->next;
    }
    
    // If we've cycled through everyone, start a new round
    if (!combat->current) {
        combat->round_number++;
        
        // Reset actions and parries for all participants
        for (CombatParticipant *p = combat->participants; p; p = p->next) {
            p->actions_remaining = 1;
            p->parries_remaining = 2;  // Base 2 parries
            p->is_defending = false;
        }
        
        // Roll new initiative
        combat_roll_initiative(combat);
        combat_display_initiative(combat);
    }
    
    // Announce whose turn it is
    if (combat->current) {
        char msg[256];
        snprintf(msg, sizeof(msg), "\n\033[1;36m>>> %s's turn <<<\033[0m\n", 
                 combat->current->name);
        combat_broadcast(combat, msg);
    }
}

void combat_display_initiative(CombatRound *combat) {
    if (!combat) return;
    
    char msg[512];
    snprintf(msg, sizeof(msg), "\n\033[1;33mROUND %d INITIATIVE ORDER:\033[0m\n", 
             combat->round_number);
    combat_broadcast(combat, msg);
    
    int position = 1;
    for (CombatParticipant *p = combat->participants; p; p = p->next) {
        snprintf(msg, sizeof(msg), "  %d. %s (Initiative: %d)\n", 
                 position++, p->name, p->initiative);
        combat_broadcast(combat, msg);
    }
    combat_broadcast(combat, "\n");
}

// ============================================================================
// ATTACK SYSTEM
// ============================================================================

int combat_calculate_strike_bonus(CombatParticipant *p, bool is_melee) {
    if (!p || !p->character) return 0;
    
    int bonus = 0;
    
    // Hand to Hand bonus (always applies)
    for (int i = 0; i < p->character->num_skills; i++) {
        if (p->character->skills[i].skill_id == 0) {  // Hand to Hand
            bonus += p->character->skills[i].percentage / 20;
            break;
        }
    }
    
    // Weapon Proficiency bonus (if applicable)
    if (!is_melee) {
        // Check for WP Rifle or WP Pistol (skill IDs 8 and 9)
        for (int i = 0; i < p->character->num_skills; i++) {
            if (p->character->skills[i].skill_id == 8 || 
                p->character->skills[i].skill_id == 9) {
                bonus += p->character->skills[i].percentage / 20;
                break;
            }
        }
    } else {
        // Check for WP Sword (skill ID 7)
        for (int i = 0; i < p->character->num_skills; i++) {
            if (p->character->skills[i].skill_id == 7) {
                bonus += p->character->skills[i].percentage / 20;
                break;
            }
        }
    }
    
    // PP bonus for strike
    if (p->character->stats.pp > 10) {
        bonus += (p->character->stats.pp - 10) / 5;
    }
    
    return bonus;
}

int combat_calculate_damage(int weapon_dice, int weapon_sides, int ps_bonus) {
    int damage = combat_roll_dice(weapon_dice, weapon_sides);
    damage += ps_bonus;
    return (damage > 0) ? damage : 1;  // Minimum 1 damage
}

DamageResult combat_attack_melee(CombatParticipant *attacker, CombatParticipant *defender) {
    DamageResult result = {0, false, false, 0, 0};
    
    if (!attacker || !defender) return result;
    
    // Roll to hit (1d20 + strike bonus)
    int attack_roll = combat_d20();
    int strike_bonus = combat_calculate_strike_bonus(attacker, true);
    int total_strike = attack_roll + strike_bonus;
    
    // Check for critical hit
    if (attack_roll == 20) {
        result.is_critical = true;
    }
    
    // Check for fumble
    if (attack_roll == 1) {
        char msg[256];
        snprintf(msg, sizeof(msg), "\033[1;31m%s fumbles the attack!\033[0m\n", 
                 attacker->name);
        combat_send_to_participant(attacker, msg);
        combat_send_to_participant(defender, msg);
        return result;
    }
    
    // Target number is 8 (base difficulty)
    int target_number = 8;
    
    // Check if defender parries
    if (defender->parries_remaining > 0 && !defender->is_defending) {
        if (combat_defend_parry(defender, total_strike)) {
            return result;  // Attack parried, no damage
        }
    }
    
    // Check if hit
    if (total_strike < target_number) {
        char msg[256];
        snprintf(msg, sizeof(msg), "%s swings at %s but misses! (Rolled %d+%d=%d vs %d)\n",
                 attacker->name, defender->name, attack_roll, strike_bonus, total_strike, target_number);
        combat_send_to_participant(attacker, msg);
        combat_send_to_participant(defender, msg);
        return result;
    }
    
    // Calculate damage (2d6 base melee + PS bonus)
    int ps_bonus = 0;
    if (attacker->character && attacker->character->stats.ps > 15) {
        ps_bonus = (attacker->character->stats.ps - 15) / 5 + 1;
    }
    
    result.damage = combat_calculate_damage(2, 6, ps_bonus);
    
    // Double damage on critical hit
    if (result.is_critical) {
        result.damage *= 2;
    }
    
    // Apply damage
    combat_apply_damage(defender, &result);
    
    // Send combat messages
    char msg[256];
    if (result.is_critical) {
        snprintf(msg, sizeof(msg), 
                 "\033[1;33mCRITICAL HIT!\033[0m %s strikes %s for \033[1;31m%d damage\033[0m!\n",
                 attacker->name, defender->name, result.damage);
    } else {
        snprintf(msg, sizeof(msg), 
                 "%s strikes %s for \033[1;31m%d damage\033[0m! (Rolled %d+%d=%d)\n",
                 attacker->name, defender->name, result.damage, attack_roll, strike_bonus, total_strike);
    }
    combat_send_to_participant(attacker, msg);
    combat_send_to_participant(defender, msg);
    
    return result;
}

DamageResult combat_attack_ranged(CombatParticipant *attacker, CombatParticipant *defender) {
    DamageResult result = {0, false, false, 0, 0};
    
    if (!attacker || !defender) return result;
    
    // Roll to hit (1d20 + WP bonus)
    int attack_roll = combat_d20();
    int strike_bonus = combat_calculate_strike_bonus(attacker, false);
    int total_strike = attack_roll + strike_bonus;
    
    // Check for critical hit
    if (attack_roll == 20) {
        result.is_critical = true;
    }
    
    // Check for fumble
    if (attack_roll == 1) {
        char msg[256];
        snprintf(msg, sizeof(msg), "\033[1;31m%s fumbles the shot!\033[0m\n", 
                 attacker->name);
        combat_send_to_participant(attacker, msg);
        combat_send_to_participant(defender, msg);
        return result;
    }
    
    // Target number is 8 (base difficulty)
    int target_number = 8;
    
    // Ranged attacks can only be dodged, not parried
    if (defender->is_defending) {
        if (combat_defend_dodge(defender, total_strike)) {
            return result;  // Attack dodged, no damage
        }
    }
    
    // Check if hit
    if (total_strike < target_number) {
        char msg[256];
        snprintf(msg, sizeof(msg), "%s shoots at %s but misses! (Rolled %d+%d=%d vs %d)\n",
                 attacker->name, defender->name, attack_roll, strike_bonus, total_strike, target_number);
        combat_send_to_participant(attacker, msg);
        combat_send_to_participant(defender, msg);
        return result;
    }
    
    // Calculate damage (3d6 base ranged, no PS bonus)
    result.damage = combat_calculate_damage(3, 6, 0);
    
    // Double damage on critical hit
    if (result.is_critical) {
        result.damage *= 2;
    }
    
    // Apply damage
    combat_apply_damage(defender, &result);
    
    // Send combat messages
    char msg[256];
    if (result.is_critical) {
        snprintf(msg, sizeof(msg), 
                 "\033[1;33mCRITICAL HIT!\033[0m %s shoots %s for \033[1;31m%d damage\033[0m!\n",
                 attacker->name, defender->name, result.damage);
    } else {
        snprintf(msg, sizeof(msg), 
                 "%s shoots %s for \033[1;31m%d damage\033[0m! (Rolled %d+%d=%d)\n",
                 attacker->name, defender->name, result.damage, attack_roll, strike_bonus, total_strike);
    }
    combat_send_to_participant(attacker, msg);
    combat_send_to_participant(defender, msg);
    
    return result;
}

// ============================================================================
// DEFENSE SYSTEM
// ============================================================================

int combat_calculate_parry_bonus(CombatParticipant *p) {
    if (!p || !p->character) return 0;
    
    int bonus = 0;
    
    // Hand to Hand bonus
    for (int i = 0; i < p->character->num_skills; i++) {
        if (p->character->skills[i].skill_id == 0) {  // Hand to Hand
            bonus += p->character->skills[i].percentage / 20;
            break;
        }
    }
    
    // PP bonus for parry
    if (p->character->stats.pp > 10) {
        bonus += (p->character->stats.pp - 10) / 5;
    }
    
    return bonus;
}

int combat_calculate_dodge_bonus(CombatParticipant *p) {
    if (!p || !p->character) return 0;
    
    int bonus = 0;
    
    // Acrobatics bonus (skill ID 1)
    for (int i = 0; i < p->character->num_skills; i++) {
        if (p->character->skills[i].skill_id == 1) {  // Acrobatics
            bonus += p->character->skills[i].percentage / 20;
            break;
        }
    }
    
    // Hand to Hand bonus
    for (int i = 0; i < p->character->num_skills; i++) {
        if (p->character->skills[i].skill_id == 0) {  // Hand to Hand
            bonus += p->character->skills[i].percentage / 20;
            break;
        }
    }
    
    // PP bonus for dodge
    if (p->character->stats.pp > 10) {
        bonus += (p->character->stats.pp - 10) / 5;
    }
    
    return bonus;
}

bool combat_defend_parry(CombatParticipant *defender, int attack_roll) {
    if (!defender || defender->parries_remaining <= 0) return false;
    
    int parry_roll = combat_d20();
    int parry_bonus = combat_calculate_parry_bonus(defender);
    int total_parry = parry_roll + parry_bonus;
    
    defender->parries_remaining--;
    
    if (total_parry >= attack_roll) {
        char msg[256];
        snprintf(msg, sizeof(msg), "\033[1;32m%s parries the attack!\033[0m (Rolled %d+%d=%d vs %d)\n",
                 defender->name, parry_roll, parry_bonus, total_parry, attack_roll);
        combat_send_to_participant(defender, msg);
        return true;
    }
    
    return false;
}

bool combat_defend_dodge(CombatParticipant *defender, int attack_roll) {
    if (!defender || !defender->is_defending) return false;
    
    int dodge_roll = combat_d20();
    int dodge_bonus = combat_calculate_dodge_bonus(defender);
    int total_dodge = dodge_roll + dodge_bonus;
    
    if (total_dodge >= attack_roll) {
        char msg[256];
        snprintf(msg, sizeof(msg), "\033[1;32m%s dodges the attack!\033[0m (Rolled %d+%d=%d vs %d)\n",
                 defender->name, dodge_roll, dodge_bonus, total_dodge, attack_roll);
        combat_send_to_participant(defender, msg);
        return true;
    }
    
    return false;
}

// ============================================================================
// DAMAGE & DEATH
// ============================================================================

void combat_apply_damage(CombatParticipant *target, DamageResult *dmg) {
    if (!target || !dmg || !target->character) return;
    
    int damage_remaining = dmg->damage;
    
    // Apply to SDC first
    if (target->character->sdc > 0) {
        if (damage_remaining >= target->character->sdc) {
            dmg->sdc_damage = target->character->sdc;
            damage_remaining -= target->character->sdc;
            target->character->sdc = 0;
        } else {
            dmg->sdc_damage = damage_remaining;
            target->character->sdc -= damage_remaining;
            damage_remaining = 0;
        }
    }
    
    // If SDC is depleted, apply remaining damage to HP
    if (damage_remaining > 0) {
        if (damage_remaining >= target->character->hp) {
            dmg->hp_damage = target->character->hp;
            target->character->hp = 0;
        } else {
            dmg->hp_damage = damage_remaining;
            target->character->hp -= damage_remaining;
        }
    }
    
    // Check if target is killed
    if (combat_check_death(target)) {
        dmg->is_kill = true;
    }
}

bool combat_check_death(CombatParticipant *p) {
    if (!p || !p->character) return false;
    
    if (p->character->hp <= 0) {
        char msg[256];
        snprintf(msg, sizeof(msg), 
                 "\n\033[1;31m>>> %s has been defeated! <<<\033[0m\n\n",
                 p->name);
        combat_send_to_participant(p, msg);
        
        // Broadcast to combat
        CombatRound *combat = combat_get_active(p->session);
        if (combat) {
            combat_broadcast(combat, msg);
        }
        
        return true;
    }
    
    return false;
}

void combat_award_experience(CombatParticipant *winner, CombatParticipant *loser) {
    if (!winner || !loser || !winner->character) return;
    
    // Basic XP award (will be expanded in later phases)
    int xp = 50;  // Base XP for defeating an enemy
    
    // Add character experience field in future phase
    char msg[256];
    snprintf(msg, sizeof(msg), "\033[1;33mYou gained %d experience points!\033[0m\n", xp);
    combat_send_to_participant(winner, msg);
}

// ============================================================================
// MESSAGING
// ============================================================================

void combat_broadcast(CombatRound *combat, const char *message) {
    if (!combat || !message) return;
    
    for (CombatParticipant *p = combat->participants; p; p = p->next) {
        combat_send_to_participant(p, message);
    }
}

void combat_send_to_participant(CombatParticipant *p, const char *message) {
    if (!p || !message) return;
    
    if (p->is_player && p->session) {
        send_to_player(p->session, message);
    }
}
