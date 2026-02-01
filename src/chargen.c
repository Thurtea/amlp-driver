#include "chargen.h"
#include "session_internal.h"
#include "room.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* External function from session.c */
extern void send_to_player(PlayerSession *session, const char *format, ...);

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

/* Initialize chargen for new player */
void chargen_init(PlayerSession *sess) {
    if (!sess) return;
    
    sess->chargen_state = CHARGEN_RACE_SELECT;
    
    send_to_player(sess, "\n");
    send_to_player(sess, "\033[1;35m╔═══════════════════════════════════════╗\033[0m\n");
    send_to_player(sess, "\033[1;35m║      CHARACTER CREATION SYSTEM       ║\033[0m\n");
    send_to_player(sess, "\033[1;35m║        Rifts Earth - 109 P.A.        ║\033[0m\n");
    send_to_player(sess, "\033[1;35m╚═══════════════════════════════════════╝\033[0m\n");
    send_to_player(sess, "\n");
    send_to_player(sess, "\033[1;33m=== SELECT YOUR RACE ===\033[0m\n\n");
    send_to_player(sess, "  1. \033[1;32mHuman\033[0m - Baseline race, adaptable and determined\n");
    send_to_player(sess, "  2. \033[1;34mElf\033[0m - Graceful and magical, attuned to nature\n");
    send_to_player(sess, "  3. \033[1;31mDwarf\033[0m - Stout and resilient, master craftsmen\n");
    send_to_player(sess, "  4. \033[1;35mDragon Hatchling\033[0m - Young but powerful, magical beings\n");
    send_to_player(sess, "\n");
    send_to_player(sess, "Enter choice (1-4): ");
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
    send_to_player(sess, "\033[1;36m╔═══════════════════════════════════════════════════════╗\033[0m\n");
    send_to_player(sess, "\033[1;36m║              YOUR CHARACTER SHEET                    ║\033[0m\n");
    send_to_player(sess, "\033[1;36m╠═══════════════════════════════════════════════════════╣\033[0m\n");
    send_to_player(sess, "\033[1;36m║\033[0m  Name: %-45s \033[1;36m║\033[0m\n", sess->username);
    send_to_player(sess, "\033[1;36m║\033[0m  Race: %-20s  O.C.C.: %-17s \033[1;36m║\033[0m\n", ch->race, ch->occ);
    send_to_player(sess, "\033[1;36m╠═══════════════════════════════════════════════════════╣\033[0m\n");
    send_to_player(sess, "\033[1;36m║\033[0m  \033[1;33mATTRIBUTES\033[0m                                          \033[1;36m║\033[0m\n");
    send_to_player(sess, "\033[1;36m║\033[0m  IQ: %-3d   ME: %-3d   MA: %-3d   PS: %-3d             \033[1;36m║\033[0m\n",
                  ch->stats.iq, ch->stats.me, ch->stats.ma, ch->stats.ps);
    send_to_player(sess, "\033[1;36m║\033[0m  PP: %-3d   PE: %-3d   PB: %-3d   SPD: %-3d            \033[1;36m║\033[0m\n",
                  ch->stats.pp, ch->stats.pe, ch->stats.pb, ch->stats.spd);
    send_to_player(sess, "\033[1;36m╠═══════════════════════════════════════════════════════╣\033[0m\n");
    send_to_player(sess, "\033[1;36m║\033[0m  HP: %-3d/%-3d    S.D.C.: %-3d/%-3d                     \033[1;36m║\033[0m\n",
                  ch->hp, ch->max_hp, ch->sdc, ch->max_sdc);
    
    if (ch->max_isp > 0) {
        send_to_player(sess, "\033[1;36m║\033[0m  I.S.P.: %-3d/%-3d (Psionic Energy)                  \033[1;36m║\033[0m\n",
                      ch->isp, ch->max_isp);
    }
    if (ch->max_ppe > 0) {
        send_to_player(sess, "\033[1;36m║\033[0m  P.P.E.: %-3d/%-3d (Magical Energy)                  \033[1;36m║\033[0m\n",
                      ch->ppe, ch->max_ppe);
    }
    
    send_to_player(sess, "\033[1;36m╚═══════════════════════════════════════════════════════╝\033[0m\n");
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
            if (choice >= 1 && choice <= 4) {
                const char *races[] = {"Human", "Elf", "Dwarf", "Dragon Hatchling"};
                ch->race = strdup(races[choice - 1]);
                
                send_to_player(sess, "\nYou selected: \033[1;32m%s\033[0m\n\n", ch->race);
                
                sess->chargen_state = CHARGEN_OCC_SELECT;
                send_to_player(sess, "\033[1;33m=== SELECT YOUR O.C.C. ===\033[0m\n\n");
                send_to_player(sess, "  1. \033[1;31mCyber-Knight\033[0m - Techno-warrior with psionic powers\n");
                send_to_player(sess, "  2. \033[1;35mLey Line Walker\033[0m - Master of magical energies\n");
                send_to_player(sess, "  3. \033[1;33mRogue Scientist\033[0m - Tech expert and inventor\n");
                send_to_player(sess, "  4. \033[1;36mTechno-Wizard\033[0m - Blend of magic and technology\n");
                send_to_player(sess, "\n");
                send_to_player(sess, "Enter choice (1-4): ");
            } else {
                send_to_player(sess, "Invalid choice. Please enter 1-4: ");
            }
            break;
            
        case CHARGEN_OCC_SELECT:
            if (choice >= 1 && choice <= 4) {
                const char *occs[] = {"Cyber-Knight", "Ley Line Walker", "Rogue Scientist", "Techno-Wizard"};
                ch->occ = strdup(occs[choice - 1]);
                
                send_to_player(sess, "\nYou selected: \033[1;32m%s\033[0m\n\n", ch->occ);
                send_to_player(sess, "Rolling your attributes...\n");
                
                chargen_roll_stats(sess);
                chargen_display_stats(sess);
                
                sess->chargen_state = CHARGEN_STATS_CONFIRM;
                send_to_player(sess, "\n");
                send_to_player(sess, "Accept these stats? (yes/reroll): ");
            } else {
                send_to_player(sess, "Invalid choice. Please enter 1-4: ");
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
