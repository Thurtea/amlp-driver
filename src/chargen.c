#include "chargen.h"
#include "session_internal.h"
#include "room.h"
#include "skills.h"
#include "combat.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdint.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>

/* External function from session.c */
extern void send_to_player(PlayerSession *session, const char *format, ...);

/* ========== COMPLETE RACE AND OCC DATABASE ========== */


/* ALL 51 RIFTS EARTH RACES */
const RaceOCCInfo ALL_RACES[] = {
    {"Human", "Baseline race, adaptable and determined"},
    {"Elf", "Graceful and magical, attuned to nature"},
    {"Dwarf", "Stout and resilient, master craftsmen"},
    {"Dragon Hatchling", "Young but powerful, magical beings"},
    {"Adult Dragon", "Massive draconic being of immense power"},
    {"Ancient Dragon", "Millenia-old dragon of legendary might"},
    {"Brod kill", "Demon-cursed mutant with supernatural strength"},
    {"Burster", "Psychic with pyrokinetic abilities"},
    {"Cat Girl", "Feline humanoid, agile and curious"},
    {"Changeling", "Shapeshifting fae creature"},
    {"Cosmo-Knight", "Cosmic guardian with stellar powers"},
    {"D-Bee", "Dimensional being from another reality"},
    {"Deevil", "Lesser demon from dark dimensions"},
    {"Demon", "Powerful supernatural evil entity"},
    {"Dog Boy", "Canine mutant bred by Coalition"},
    {"Dragon Juicer", "Dragon blood-enhanced super soldier"},
    {"Eandroth", "Insectoid alien warrior race"},
    {"Gargoyle", "Stone-skinned supernatural creature"},
   {"Gnome", "Small magical being, tech-savvy"},
    {"Goblin", "Small cunning supernatural creature"},
    {"Godling", "Offspring of divine beings"},
    {"Halfling", "Small folk, lucky and brave"},
    {"Hobgoblin", "Larger, fierce goblinoid"},
    {"Mega-Juicer", "Ultra-enhanced combat juicer"},
    {"Mind Bleeder", "Psychic vampire, drains ISP"},
    {"Mind Melter", "Master psychic, multiple disciplines"},
    {"Minion", "Bio-wizard creation, enslaved"},
    {"Minotaur", "Bull-headed warrior of great strength"},
    {"Mutant Animal", "Uplifted animal with human intelligence"},
    {"Nightbane", "Shape-shifter between human/monster"},
    {"Noli", "Four-armed alien symbiote race"},
    {"Ogre", "Large brutish humanoid"},
    {"Orc", "Savage warrior race"},
    {"Promethean", "Artificial life, seeking humanity"},
    {"Psi-Ghost", "Psychic entity, telekinetic mastery"},
    {"Psi-Healer", "Psychic specialized in healing"},
    {"Psi-Stalker", "Psychic hunter, magic-sensing mutant"},
    {"Quick-Flex", "Incredibly fast alien species"},
    {"Simvan", "Monster-riding nomadic warrior"},
    {"Splugorth", "Ancient evil intelligence"},
    {"Splugorth Minion", "Enslaved warrior of Splugorth"},
    {"Splynn Slave", "Enslaved from dimensional market"},
    {"Thorny Dragon", "Spiked dragon variant"},
    {"Titan Juicer", "Massive juicer, extended lifespan"},
    {"Trimadore", "Crystalline energy being"},
    {"Troll", "Regenerating savage humanoid"},
    {"True Atlantean", "Dimensional traveler, tattoo magic"},
    {"Uteni", "Fur-covered peaceful alien"},
    {"Vampire", "Undead blood drinker"},
    {"Werewolf", "Shapeshifting wolf-human"},
    {"Wolfen", "Noble lupine warrior race"}
};

/* ALL 35 RIFTS EARTH OCCs */
const RaceOCCInfo ALL_OCCS[] = {
    {"Cyber-Knight", "Techno-warrior with psionic powers"},
    {"Ley Line Walker", "Master of magical energies"},
    {"Rogue Scientist", "Tech expert and inventor"},
    {"Techno-Wizard", "Blend of magic and technology"},
    {"Battle Magus", "Combat mage, magic and weapons"},
    {"Biomancer", "Life magic specialist"},
    {"Body Fixer", "Cybernetic doctor and surgeon"},
    {"Burster", "Pyrokinetic psychic warrior"},
    {"City Rat", "Urban survivor, street smart"},
    {"Cosmo-Knight", "Cosmic power armor knight"},
    {"Crazy", "Augmented insane super soldier"},
    {"Dragon Hatchling RCC", "Young dragon racial class"},
    {"Elemental Fusionist", "Combines elemental magic"},
    {"Full Conversion Borg", "Complete cyborg conversion"},
    {"Headhunter", "Bounty hunter, armor specialist"},
    {"Juicer", "Chemical-enhanced super soldier"},
    {"Line Walker", "Ley line manipulator"},
    {"Mercenary", "Professional soldier for hire"},
    {"Mind Melter", "Master psychic disciplines"},
    {"Mystic", "Spiritual magic user"},
    {"Necromancer", "Death magic practitioner"},
    {"Operator", "Mechanical genius, vehicle expert"},
    {"Power Armor Pilot", "Elite mech pilot"},
    {"Psi-Healer", "Psychic healing specialist"},
    {"Psi-Stalker", "Anti-magic psionic hunter"},
    {"Robot Pilot", "Giant robot operator"},
    {"Rogue Scholar", "Knowledge seeker, multi-skilled"},
    {"Sea Titan", "Ocean-based warrior"},
    {"Shifter", "Dimensional magic specialist"},
    {"Special Forces", "Elite military operative"},
    {"Stone Master", "Earth and stone magic user"},
    {"Temporal Wizard", "Time magic specialist"},
    {"Vagabond", "Jack-of-all-trades wanderer"},
    {"Warlock", "Elemental pact magic user"},
    {"Wilderness Scout", "Tracker and survivalist"}
};

#define NUM_RACES (sizeof(ALL_RACES) / sizeof(RaceOCCInfo))
#define NUM_OCCS (sizeof(ALL_OCCS) / sizeof(RaceOCCInfo))
#define ITEMS_PER_PAGE 10

/* Dice rolling */
int roll_3d6(void) {
    static int seeded = 0;
    if (!seeded) {
        srand(time(NULL));
        seeded = 1;
    }
    return (rand() % 6 + 1) + (rand() % 6 + 1) + (rand() % 6 + 1);
}

int roll_1d6(void) {
    static int seeded = 0;
    if (!seeded) {
        srand(time(NULL));
        seeded = 1;
    }
    return (rand() % 6 + 1);
}

/* Init car generation */
void chargen_init(PlayerSession *sess) {
    if (!sess) return;
    
    sess->chargen_state = CHARGEN_RACE_SELECT;
    sess->chargen_page = 0;
    sess->chargen_temp_choice = 0;
    
    send_to_player(sess, "\n");
    send_to_player(sess, "╔═══════════════════════════════════════╗\n");
    send_to_player(sess, "║      CHARACTER CREATION SYSTEM       ║\n");
    send_to_player(sess, "║        Rifts Earth - 109 P.A.        ║\n");
    send_to_player(sess, "╚═══════════════════════════════════════╝\n");
    send_to_player(sess, "\n");
    
    /* Display first page of races */
    int total_pages = (NUM_RACES + ITEMS_PER_PAGE - 1) / ITEMS_PER_PAGE;
    send_to_player(sess, "=== SELECT YOUR RACE (Page %d/%d) ===\n\n", 
                   sess->chargen_page + 1, total_pages);
    
    int start = sess->chargen_page * ITEMS_PER_PAGE;
    int end = start + ITEMS_PER_PAGE;
    if (end > NUM_RACES) end = NUM_RACES;
    
    for (int i = start; i < end; i++) {
        send_to_player(sess, "  %2d. %s - %s\n", 
                      i + 1, ALL_RACES[i].name, ALL_RACES[i].desc);
    }
    
    send_to_player(sess, "\n");
    if (sess->chargen_page > 0) {
        send_to_player(sess, "  Type 'p' for previous page\n");
    }
    if (end < NUM_RACES) {
        send_to_player(sess, "  Type 'n' for next page\n");
    }
    send_to_player(sess, "\nEnter choice (1-%d): ", NUM_RACES);
}

/* Roll character stats */
void chargen_roll_stats(PlayerSession *sess) {
    if (!sess) return;
    
    Character *ch = &sess->character;
    
    ch->stats.iq = roll_3d6();
    ch->stats.me = roll_3d6();
    ch->stats.ma = roll_3d6();
    ch->stats.ps = roll_3d6();
    ch->stats.pp = roll_3d6();
    ch->stats.pe = roll_3d6();
    ch->stats.pb = roll_3d6();
    ch->stats.spd = roll_3d6();
    
    /* Set basic stats based on race/OCC */
    ch->level = 1;
    ch->xp = 0;
    
    /* HP = PE + 1d6 */
    ch->hp = ch->stats.pe + roll_1d6();
    ch->max_hp = ch->hp;
    
    /* SDC = 20 + bonuses */
    ch->sdc = 20;
    ch->max_sdc = 20;
    
    /* ISP/PPE based on OCC (simplified for now) */
    if (strcmp(ch->occ, "Cyber-Knight") == 0) {
        ch->max_isp = 30 + (ch->stats.me / 2);
        ch->isp = ch->max_isp;
    }
    if (strcmp(ch->occ, "Ley Line Walker") == 0) {
        ch->max_ppe = 60 + (ch->stats.pe * 2);
        ch->ppe = ch->max_ppe;
    }
}

/* Display character stats */
void chargen_display_stats(PlayerSession *sess) {
    if (!sess) return;
    
    Character *ch = &sess->character;
    
    send_to_player(sess, "\n");
    send_to_player(sess, "╔═══════════════════════════════════════════════════════╗\n");
    send_to_player(sess, "║              YOUR CHARACTER SHEET                    ║\n");
    send_to_player(sess, "╠═══════════════════════════════════════════════════════╣\n");
    send_to_player(sess, "║  Name: %-45s ║\n", sess->username);
    send_to_player(sess, "║  Race: %-20s  O.C.C.: %-17s ║\n", ch->race, ch->occ);
    send_to_player(sess, "╠═══════════════════════════════════════════════════════╣\n");
    send_to_player(sess, "║  ATTRIBUTES                                          ║\n");
    send_to_player(sess, "║  IQ: %-3d   ME: %-3d   MA: %-3d   PS: %-3d             ║\n",
                  ch->stats.iq, ch->stats.me, ch->stats.ma, ch->stats.ps);
    send_to_player(sess, "║  PP: %-3d   PE: %-3d   PB: %-3d   SPD: %-3d            ║\n",
                  ch->stats.pp, ch->stats.pe, ch->stats.pb, ch->stats.spd);
    send_to_player(sess, "╠═══════════════════════════════════════════════════════╣\n");
    send_to_player(sess, "║  HP: %-3d/%-3d    S.D.C.: %-3d/%-3d                     ║\n",
                  ch->hp, ch->max_hp, ch->sdc, ch->max_sdc);
    
    if (ch->max_isp > 0) {
        send_to_player(sess, "║  I.S.P.: %-3d/%-3d (Psionic Energy)                  ║\n",
                      ch->isp, ch->max_isp);
    }
    if (ch->max_ppe > 0) {
        send_to_player(sess, "║  P.P.E.: %-3d/%-3d (Magical Energy)                  ║\n",
                      ch->ppe, ch->max_ppe);
    }
    
    send_to_player(sess, "╚═══════════════════════════════════════════════════════╝\n");
}

/* Complete character generation */
void chargen_complete(PlayerSession *sess) {
    if (!sess) return;
    
    sess->chargen_state = CHARGEN_COMPLETE;
    sess->state = STATE_PLAYING;
    
    /* Place player in starting room */
    Room *start = room_get_start();
    if (start) {
        sess->current_room = start;
        room_add_player(start, sess);
    }
    
    send_to_player(sess, "\n");
    send_to_player(sess, "\033[1;32mCharacter creation complete!\033[0m\n");
    send_to_player(sess, "Welcome to Rifts Earth, %s!\n", sess->username);
    
    /* Auto-save new character */
    send_to_player(sess, "\nSaving your character...\n");
    if (save_character(sess)) {
        send_to_player(sess, " Character saved!\n");
    } else {
        send_to_player(sess, " Warning: Failed to save character.\n");
    }
    
    send_to_player(sess, "\n");
    
    /* Auto-look at starting room */
    cmd_look(sess, "");
    send_to_player(sess, "\n> ");
}

/* Process chargen input */
void chargen_process_input(PlayerSession *sess, const char *input) {
    if (!sess || !input) return;
    
    Character *ch = &sess->character;
    int choice = atoi(input);
    
    switch (sess->chargen_state) {
        case CHARGEN_RACE_SELECT:
            /* Handle pagination */
            if (input[0] == 'n' || input[0] == 'N') {
                int total_pages = (NUM_RACES + ITEMS_PER_PAGE - 1) / ITEMS_PER_PAGE;
                if (sess->chargen_page < total_pages - 1) {
                    sess->chargen_page++;
                    /* Redisplay */
                    send_to_player(sess, "\n=== SELECT YOUR RACE (Page %d/%d) ===\n\n", 
                                   sess->chargen_page + 1, total_pages);
                    
                    int start = sess->chargen_page * ITEMS_PER_PAGE;
                    int end = start + ITEMS_PER_PAGE;
                    if (end > NUM_RACES) end = NUM_RACES;
                    
                    for (int i = start; i < end; i++) {
                        send_to_player(sess, "  %2d. %s - %s\n", 
                                      i + 1, ALL_RACES[i].name, ALL_RACES[i].desc);
                    }
                    
                    send_to_player(sess, "\n");
                    if (sess->chargen_page > 0) {
                        send_to_player(sess, "  Type 'p' for previous page\n");
                    }
                    if (end < NUM_RACES) {
                        send_to_player(sess, "  Type 'n' for next page\n");
                    }
                    send_to_player(sess, "\nEnter choice (1-%d): ", NUM_RACES);
                } else {
                    send_to_player(sess, "Already on last page.\nEnter choice (1-%d): ", NUM_RACES);
                }
                return;
            }
            
            if (input[0] == 'p' || input[0] == 'P') {
                if (sess->chargen_page > 0) {
                    sess->chargen_page--;
                    /* Redisplay */
                    int total_pages = (NUM_RACES + ITEMS_PER_PAGE - 1) / ITEMS_PER_PAGE;
                    send_to_player(sess, "\n=== SELECT YOUR RACE (Page %d/%d) ===\n\n", 
                                   sess->chargen_page + 1, total_pages);
                    
                    int start = sess->chargen_page * ITEMS_PER_PAGE;
                    int end = start + ITEMS_PER_PAGE;
                    if (end > NUM_RACES) end = NUM_RACES;
                    
                    for (int i = start; i < end; i++) {
                        send_to_player(sess, "  %2d. %s - %s\n", 
                                      i + 1, ALL_RACES[i].name, ALL_RACES[i].desc);
                    }
                    
                    send_to_player(sess, "\n");
                    if (sess->chargen_page > 0) {
                        send_to_player(sess, "  Type 'p' for previous page\n");
                    }
                    if (end < NUM_RACES) {
                        send_to_player(sess, "  Type 'n' for next page\n");
                    }
                    send_to_player(sess, "\nEnter choice (1-%d): ", NUM_RACES);
                } else {
                    send_to_player(sess, "Already on first page.\nEnter choice (1-%d): ", NUM_RACES);
                }
                return;
            }
            
            /* Handle selection */
            if (choice >= 1 && choice <= NUM_RACES) {
                ch->race = strdup(ALL_RACES[choice - 1].name);
                
                send_to_player(sess, "\nYou selected: \033[1;32m%s\033[0m\n\n", ch->race);
                
                /* Move to OCC selection */
                sess->chargen_state = CHARGEN_OCC_SELECT;
                sess->chargen_page = 0;  /* Reset page for OCC */
                
                int total_pages = (NUM_OCCS + ITEMS_PER_PAGE - 1) / ITEMS_PER_PAGE;
                send_to_player(sess, "=== SELECT YOUR O.C.C. (Page %d/%d) ===\n\n", 
                               sess->chargen_page + 1, total_pages);
                
                int start = 0;
                int end = ITEMS_PER_PAGE;
                if (end > NUM_OCCS) end = NUM_OCCS;
                
                for (int i = start; i < end; i++) {
                    send_to_player(sess, "  %2d. %s - %s\n", 
                                  i + 1, ALL_OCCS[i].name, ALL_OCCS[i].desc);
                }
                
                send_to_player(sess, "\n");
                if (end < NUM_OCCS) {
                    send_to_player(sess, "  Type 'n' for next page\n");
                }
                send_to_player(sess, "\nEnter choice (1-%d): ", NUM_OCCS);
            } else {
                send_to_player(sess, "Invalid choice. Please enter 1-%d: ", NUM_RACES);
            }
            break;
            
        case CHARGEN_OCC_SELECT:
            /* Handle pagination */
            if (input[0] == 'n' || input[0] == 'N') {
                int total_pages = (NUM_OCCS + ITEMS_PER_PAGE - 1) / ITEMS_PER_PAGE;
                if (sess->chargen_page < total_pages - 1) {
                    sess->chargen_page++;
                    /* Redisplay */
                    send_to_player(sess, "\n=== SELECT YOUR O.C.C. (Page %d/%d) ===\n\n", 
                                   sess->chargen_page + 1, total_pages);
                    
                    int start = sess->chargen_page * ITEMS_PER_PAGE;
                    int end = start + ITEMS_PER_PAGE;
                    if (end > NUM_OCCS) end = NUM_OCCS;
                    
                    for (int i = start; i < end; i++) {
                        send_to_player(sess, "  %2d. %s - %s\n", 
                                      i + 1, ALL_OCCS[i].name, ALL_OCCS[i].desc);
                    }
                    
                    send_to_player(sess, "\n");
                    if (sess->chargen_page > 0) {
                        send_to_player(sess, "  Type 'p' for previous page\n");
                    }
                    if (end < NUM_OCCS) {
                        send_to_player(sess, "  Type 'n' for next page\n");
                    }
                    send_to_player(sess, "\nEnter choice (1-%d): ", NUM_OCCS);
                } else {
                    send_to_player(sess, "Already on last page.\nEnter choice (1-%d): ", NUM_OCCS);
                }
                return;
            }
            
            if (input[0] == 'p' || input[0] == 'P') {
                if (sess->chargen_page > 0) {
                    sess->chargen_page--;
                    /* Redisplay */
                    int total_pages = (NUM_OCCS + ITEMS_PER_PAGE - 1) / ITEMS_PER_PAGE;
                    send_to_player(sess, "\n=== SELECT YOUR O.C.C. (Page %d/%d) ===\n\n", 
                                   sess->chargen_page + 1, total_pages);
                    
                    int start = sess->chargen_page * ITEMS_PER_PAGE;
                    int end = start + ITEMS_PER_PAGE;
                    if (end > NUM_OCCS) end = NUM_OCCS;
                    
                    for (int i = start; i < end; i++) {
                        send_to_player(sess, "  %2d. %s - %s\n", 
                                      i + 1, ALL_OCCS[i].name, ALL_OCCS[i].desc);
                    }
                    
                    send_to_player(sess, "\n");
                    if (sess->chargen_page > 0) {
                        send_to_player(sess, "  Type 'p' for previous page\n");
                    }
                    if (end < NUM_OCCS) {
                        send_to_player(sess, "  Type 'n' for next page\n");
                    }
                    send_to_player(sess, "\nEnter choice (1-%d): ", NUM_OCCS);
                } else {
                    send_to_player(sess, "Already on first page.\nEnter choice (1-%d): ", NUM_OCCS);
                }
                return;
            }
            
            /* Handle selection */
            if (choice >= 1 && choice <= NUM_OCCS) {
                ch->occ = strdup(ALL_OCCS[choice - 1].name);
                
                send_to_player(sess, "\nYou selected: \033[1;32m%s\033[0m\n\n", ch->occ);
                
                /* Assign OCC-specific skills */
                occ_assign_skills(sess, ch->occ);
                
                send_to_player(sess, "Getting your starting skills...\n");
                send_to_player(sess, "Rolling your attributes...\n");
                
                chargen_roll_stats(sess);
                chargen_display_stats(sess);
                
                sess->chargen_state = CHARGEN_STATS_CONFIRM;
                send_to_player(sess, "\n");
                send_to_player(sess, "Accept these stats? (yes/reroll): ");
            } else {
                send_to_player(sess, "Invalid choice. Please enter 1-%d: ", NUM_OCCS);
            }
            break;
            
        case CHARGEN_STATS_CONFIRM:
            if (strncasecmp(input, "yes", 3) == 0 || strncasecmp(input, "y", 1) == 0) {
                chargen_complete(sess);
            } else if (strncasecmp(input, "reroll", 6) == 0 || strncasecmp(input, "r", 1) == 0) {
                send_to_player(sess, "\nRerolling stats...\n");
                chargen_roll_stats(sess);
                chargen_display_stats(sess);
                send_to_player(sess, "\n");
                send_to_player(sess, "Accept these stats? (yes/reroll): ");
            } else {
                send_to_player(sess, "Please answer 'yes' or 'reroll': ");
            }
            break;
            
        default:
            break;
    }
}

/* Stats command */
void cmd_stats(PlayerSession *sess, const char *args) {
    if (!sess) return;
    
    if (sess->state != STATE_PLAYING) {
        send_to_player(sess, "You must complete character creation first.\n");
        return;
    }
    
    chargen_display_stats(sess);
}

/* Skills command */
void cmd_skills(PlayerSession *sess, const char *args) {
    if (!sess) return;
    
    if (sess->state != STATE_PLAYING) {
        send_to_player(sess, "You must complete character creation first.\n");
        return;
    }
    
    skill_display_list(sess);
}

/* ========== COMBAT COMMANDS ========== */

void cmd_attack(PlayerSession *sess, const char *args) {
    if (!sess || sess->state != STATE_PLAYING) {
        send_to_player(sess, "You must complete character creation first.\n");
        return;
    }
    
    if (!args || !(*args)) {
        send_to_player(sess, "Attack who?\n");
        return;
    }
    
    // For Phase 3, we'll implement basic NPC combat
    // For now, demonstrate combat initiation
    send_to_player(sess, "Combat system ready! Use 'strike' or 'shoot' commands.\n");
    send_to_player(sess, "(Full combat with NPCs coming in next iteration)\n");
}

void cmd_strike(PlayerSession *sess, const char *args) {
    if (!sess || sess->state != STATE_PLAYING) {
        send_to_player(sess, "You must complete character creation first.\n");
        return;
    }
    
    // Check if in combat
    CombatRound *combat = combat_get_active(sess);
    if (!combat) {
        send_to_player(sess, "You are not in combat.\n");
        return;
    }
    
    // Find this player's participant
    CombatParticipant *attacker = NULL;
    for (CombatParticipant *p = combat->participants; p; p = p->next) {
        if (p->session == sess) {
            attacker = p;
            break;
        }
    }
    
    if (!attacker) {
        send_to_player(sess, "Error: You are not in this combat.\n");
        return;
    }
    
    // Check if it's this player's turn
    if (combat->current != attacker) {
        send_to_player(sess, "It is not your turn.\n");
        return;
    }
    
    // Check if already acted
    if (attacker->actions_remaining <= 0) {
        send_to_player(sess, "You have already acted this round.\n");
        return;
    }
    
    // Find target (for now, just attack the other participant)
    CombatParticipant *target = NULL;
    for (CombatParticipant *p = combat->participants; p; p = p->next) {
        if (p != attacker) {
            target = p;
            break;
        }
    }
    
    if (!target) {
        send_to_player(sess, "No valid target found.\n");
        return;
    }
    
    // Perform melee attack
    DamageResult result = combat_attack_melee(attacker, target);
    
    // Use up action
    attacker->actions_remaining--;
    
    // Check if target died
    if (result.is_kill) {
        combat_award_experience(attacker, target);
        combat_remove_participant(combat, target);
        
        // End combat if only one participant left
        if (combat->num_participants <= 1) {
            combat_end(combat);
            return;
        }
    }
    
    // Advance to next turn
    combat_next_turn(combat);
}

void cmd_shoot(PlayerSession *sess, const char *args) {
    if (!sess || sess->state != STATE_PLAYING) {
        send_to_player(sess, "You must complete character creation first.\n");
        return;
    }
    
    // Check if in combat
    CombatRound *combat = combat_get_active(sess);
    if (!combat) {
        send_to_player(sess, "You are not in combat.\n");
        return;
    }
    
    // Find this player's participant
    CombatParticipant *attacker = NULL;
    for (CombatParticipant *p = combat->participants; p; p = p->next) {
        if (p->session == sess) {
            attacker = p;
            break;
        }
    }
    
    if (!attacker) {
        send_to_player(sess, "Error: You are not in this combat.\n");
        return;
    }
    
    // Check if it's this player's turn
    if (combat->current != attacker) {
        send_to_player(sess, "It is not your turn.\n");
        return;
    }
    
    // Check if already acted
    if (attacker->actions_remaining <= 0) {
        send_to_player(sess, "You have already acted this round.\n");
        return;
    }
    
    // Find target (for now, just attack the other participant)
    CombatParticipant *target = NULL;
    for (CombatParticipant *p = combat->participants; p; p = p->next) {
        if (p != attacker) {
            target = p;
            break;
        }
    }
    
    if (!target) {
        send_to_player(sess, "No valid target found.\n");
        return;
    }
    
    // Perform ranged attack
    DamageResult result = combat_attack_ranged(attacker, target);
    
    // Use up action
    attacker->actions_remaining--;
    
    // Check if target died
    if (result.is_kill) {
        combat_award_experience(attacker, target);
        combat_remove_participant(combat, target);
        
        // End combat if only one participant left
        if (combat->num_participants <= 1) {
            combat_end(combat);
            return;
        }
    }
    
    // Advance to next turn
    combat_next_turn(combat);
}

void cmd_dodge(PlayerSession *sess, const char *args) {
    if (!sess || sess->state != STATE_PLAYING) {
        send_to_player(sess, "You must complete character creation first.\n");
        return;
    }
    
    // Check if in combat
    CombatRound *combat = combat_get_active(sess);
    if (!combat) {
        send_to_player(sess, "You are not in combat.\n");
        return;
    }
    
    // Find this player's participant
    CombatParticipant *defender = NULL;
    for (CombatParticipant *p = combat->participants; p; p = p->next) {
        if (p->session == sess) {
            defender = p;
            break;
        }
    }
    
    if (!defender) {
        send_to_player(sess, "Error: You are not in this combat.\n");
        return;
    }
    
    // Check if it's this player's turn
    if (combat->current != defender) {
        send_to_player(sess, "It is not your turn.\n");
        return;
    }
    
    // Check if already acted
    if (defender->actions_remaining <= 0) {
        send_to_player(sess, "You have already acted this round.\n");
        return;
    }
    
    // Set defending flag
    defender->is_defending = true;
    defender->actions_remaining--;
    
    send_to_player(sess, "You take a defensive stance, ready to dodge incoming attacks.\n");
    
    // Advance to next turn
    combat_next_turn(combat);
}

void cmd_flee(PlayerSession *sess, const char *args) {
    if (!sess || sess->state != STATE_PLAYING) {
        send_to_player(sess, "You must complete character creation first.\n");
        return;
    }
    
    // Check if in combat
    CombatRound *combat = combat_get_active(sess);
    if (!combat) {
        send_to_player(sess, "You are not in combat.\n");
        return;
    }
    
    // Find this player's participant
    CombatParticipant *fleeing = NULL;
    for (CombatParticipant *p = combat->participants; p; p = p->next) {
        if (p->session == sess) {
            fleeing = p;
            break;
        }
    }
    
    if (!fleeing) {
        send_to_player(sess, "Error: You are not in this combat.\n");
        return;
    }
    
    // Roll SPD check (1d20 + SPD vs 15)
    int spd = fleeing->character->stats.spd;
    int roll = combat_d20();
    int total = roll + spd;
    
    if (total >= 15) {
        // Successful flee
        send_to_player(sess, "You successfully flee from combat!\n");
        
        char msg[256];
        snprintf(msg, sizeof(msg), "%s flees from combat!\n", fleeing->name);
        combat_broadcast(combat, msg);
        
        combat_remove_participant(combat, fleeing);
        
        // End combat if only one participant left
        if (combat->num_participants <= 1) {
            combat_end(combat);
        }
    } else {
        // Failed flee
        char msg[256];
        snprintf(msg, sizeof(msg), "You fail to escape! (Rolled %d+%d=%d vs 15)\n", roll, spd, total);
        send_to_player(sess, msg);
        fleeing->actions_remaining--;
        
        // Advance to next turn
        combat_next_turn(combat);
    }
}

/* ========== CHARACTER PERSISTENCE ========== */

/* Check if a character save file exists */
int character_exists(const char *username) {
    if (!username || !username[0]) return 0;
    
    char filepath[512];
    snprintf(filepath, sizeof(filepath), "lib/save/players/%s.dat", username);
    
    struct stat st;
    return (stat(filepath, &st) == 0);
}

/* Save character to disk */
int save_character(PlayerSession *sess) {
    if (!sess || !sess->username[0]) {
        fprintf(stderr, "[Save] Error: Invalid session\n");
        return 0;
    }
    
    /* Create save directory if it doesn't exist */
    mkdir("lib", 0755);
    mkdir("lib/save", 0755);
    mkdir("lib/save/players", 0755);
    
    /* Build filepath */
    char filepath[512];
    snprintf(filepath, sizeof(filepath), "lib/save/players/%s.dat", sess->username);
    
    /* Create backup of existing save */
    char backup[512];
    snprintf(backup, sizeof(backup), "%s.bak", filepath);
    rename(filepath, backup);  /* Move old save to backup (ignore errors) */
    
    /* Open file for writing */
    FILE *f = fopen(filepath, "wb");
    if (!f) {
        fprintf(stderr, "[Save] Failed to open %s for writing: %s\n", 
                filepath, strerror(errno));
        return 0;
    }
    
    /* Write magic number (for validation) */
    uint32_t magic = 0x414D4C50;  /* "AMLP" in hex */
    fwrite(&magic, sizeof(uint32_t), 1, f);
    
    /* Write version (for future compatibility) */
    uint16_t version = 1;
    fwrite(&version, sizeof(uint16_t), 1, f);
    
    /* Write username */
    size_t name_len = strlen(sess->username);
    fwrite(&name_len, sizeof(size_t), 1, f);
    fwrite(sess->username, 1, name_len, f);
    
    /* Write privilege level */
    fwrite(&sess->privilege_level, sizeof(int), 1, f);
    
    /* Write character data */
    Character *ch = &sess->character;
    
    /* Write race */
    if (ch->race) {
        size_t race_len = strlen(ch->race);
        fwrite(&race_len, sizeof(size_t), 1, f);
        fwrite(ch->race, 1, race_len, f);
    } else {
        size_t zero = 0;
        fwrite(&zero, sizeof(size_t), 1, f);
    }
    
    /* Write OCC */
    if (ch->occ) {
        size_t occ_len = strlen(ch->occ);
        fwrite(&occ_len, sizeof(size_t), 1, f);
        fwrite(ch->occ, 1, occ_len, f);
    } else {
        size_t zero = 0;
        fwrite(&zero, sizeof(size_t), 1, f);
    }
    
    /* Write stats */
    fwrite(&ch->stats, sizeof(CharacterStats), 1, f);
    
    /* Write numeric values */
    fwrite(&ch->level, sizeof(int), 1, f);
    fwrite(&ch->xp, sizeof(int), 1, f);
    fwrite(&ch->hp, sizeof(int), 1, f);
    fwrite(&ch->max_hp, sizeof(int), 1, f);
    fwrite(&ch->sdc, sizeof(int), 1, f);
    fwrite(&ch->max_sdc, sizeof(int), 1, f);
    fwrite(&ch->isp, sizeof(int), 1, f);
    fwrite(&ch->max_isp, sizeof(int), 1, f);
    fwrite(&ch->ppe, sizeof(int), 1, f);
    fwrite(&ch->max_ppe, sizeof(int), 1, f);
    
    /* Write current room ID */
    int room_id = sess->current_room ? sess->current_room->id : 0;
    fwrite(&room_id, sizeof(int), 1, f);
    
    /* Write timestamp */
    time_t now = time(NULL);
    fwrite(&now, sizeof(time_t), 1, f);
    
    fclose(f);
    
    fprintf(stderr, "[Save] Character '%s' saved to %s\n", sess->username, filepath);
    return 1;
}

/* Load character from disk */
int load_character(PlayerSession *sess, const char *username) {
    if (!sess || !username || !username[0]) {
        fprintf(stderr, "[Load] Error: Invalid parameters\n");
        return 0;
    }
    
    /* Build filepath */
    char filepath[512];
    snprintf(filepath, sizeof(filepath), "lib/save/players/%s.dat", username);
    
    /* Check if file exists */
    FILE *f = fopen(filepath, "rb");
    if (!f) {
        fprintf(stderr, "[Load] No save file found for '%s'\n", username);
        return 0;  /* New player, need to create character */
    }
    
    /* Read and validate magic number */
    uint32_t magic;
    if (fread(&magic, sizeof(uint32_t), 1, f) != 1 || magic != 0x414D4C50) {
        fprintf(stderr, "[Load] Invalid save file format for '%s'\n", username);
        fclose(f);
        return 0;
    }
    
    /* Read version */
    uint16_t version;
    if (fread(&version, sizeof(uint16_t), 1, f) != 1) {
        fprintf(stderr, "[Load] Failed to read version for '%s'\n", username);
        fclose(f);
        return 0;
    }
    
    if (version != 1) {
        fprintf(stderr, "[Load] Unsupported save file version %d for '%s'\n", 
                version, username);
        fclose(f);
        return 0;
    }
    
    /* Read username */
    size_t name_len;
    if (fread(&name_len, sizeof(size_t), 1, f) != 1 || name_len >= sizeof(sess->username)) {
        fprintf(stderr, "[Load] Invalid username length for '%s'\n", username);
        fclose(f);
        return 0;
    }
    fread(sess->username, 1, name_len, f);
    sess->username[name_len] = '\0';
    
    /* Read privilege level */
    fread(&sess->privilege_level, sizeof(int), 1, f);
    
    /* Read character data */
    Character *ch = &sess->character;
    
    /* Read race */
    size_t race_len;
    fread(&race_len, sizeof(size_t), 1, f);
    if (race_len > 0) {
        char race_buf[256];
        if (race_len < sizeof(race_buf)) {
            fread(race_buf, 1, race_len, f);
            race_buf[race_len] = '\0';
            ch->race = strdup(race_buf);
        } else {
            fseek(f, race_len, SEEK_CUR);  /* Skip invalid data */
        }
    }
    
    /* Read OCC */
    size_t occ_len;
    fread(&occ_len, sizeof(size_t), 1, f);
    if (occ_len > 0) {
        char occ_buf[256];
        if (occ_len < sizeof(occ_buf)) {
            fread(occ_buf, 1, occ_len, f);
            occ_buf[occ_len] = '\0';
            ch->occ = strdup(occ_buf);
        } else {
            fseek(f, occ_len, SEEK_CUR);  /* Skip invalid data */
        }
    }
    
    /* Read stats */
    fread(&ch->stats, sizeof(CharacterStats), 1, f);
    
    /* Read numeric values */
    fread(&ch->level, sizeof(int), 1, f);
    fread(&ch->xp, sizeof(int), 1, f);
    fread(&ch->hp, sizeof(int), 1, f);
    fread(&ch->max_hp, sizeof(int), 1, f);
    fread(&ch->sdc, sizeof(int), 1, f);
    fread(&ch->max_sdc, sizeof(int), 1, f);
    fread(&ch->isp, sizeof(int), 1, f);
    fread(&ch->max_isp, sizeof(int), 1, f);
    fread(&ch->ppe, sizeof(int), 1, f);
    fread(&ch->max_ppe, sizeof(int), 1, f);
    
    /* Read current room ID */
    int room_id;
    fread(&room_id, sizeof(int), 1, f);
    
    /* Read timestamp (for info only) */
    time_t saved_time;
    fread(&saved_time, sizeof(time_t), 1, f);
    
    fclose(f);
    
    /* Set room pointer */
    sess->current_room = room_get_by_id(room_id);
    if (!sess->current_room) {
        sess->current_room = room_get_start();  /* Fallback to start room */
    }
    
    fprintf(stderr, "[Load] Character '%s' loaded from %s (saved %ld seconds ago)\n", 
            username, filepath, time(NULL) - saved_time);
    
    return 1;
}
