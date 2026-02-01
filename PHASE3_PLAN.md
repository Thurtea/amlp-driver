# PHASE 3: Combat System Implementation Plan

**Version:** 1.0  
**Date:** February 1, 2026  
**Status:** Planning  
**Timeline:** 5-7 days  
**Priority:** HIGH - Core gameplay loop  
**Dependencies:** Phase 1 (Races/OCCs) ✅, Phase 2 (Skills) ✅

---

## 📋 Overview

Phase 3 implements the core combat system for the Rifts MUD. This includes turn-based combat, initiative, attack/defense mechanics, damage calculation, and death/respawn systems. The combat system will integrate with Phase 2 skills (Weapon Proficiencies, Hand to Hand, Acrobatics) and will be the foundation for all future combat-related features.

**Key Goals:**
- Turn-based tactical combat with initiative
- Attack/defense roll system (1d20 + modifiers vs target number)
- Damage types: SDC (personal), HP (last resort), MDC (mega-damage)
- Critical hits and fumbles
- Experience from victories
- Death and respawn mechanics

---

## 🎯 Core Combat Mechanics (Rifts RPG Rules)

### Initiative System
- **Roll:** SPD attribute + 1d20 each combat round
- **Turn order:** Highest to lowest
- **Ties:** Higher SPD wins, then random
- **Actions per round:** 1 action (attack, dodge, parry, or flee)

### Attack Roll
- **Melee Attack:** 1d20 + strike bonus (from skills/stats) vs target number
- **Ranged Attack:** 1d20 + WP bonus (from weapon skills) vs target number
- **Target Number:** 
  - Base: 8 (standard difficulty)
  - Modified by: range, cover, movement, defender's dodge/parry
- **Natural 20:** Critical hit (double damage)
- **Natural 1:** Fumble (lose next action or drop weapon)

### Defense Roll
- **Dodge:** 1d20 + dodge bonus (from Acrobatics, Hand to Hand)
  - Must be declared before attack roll
  - Uses your action for the round
- **Parry:** 1d20 + parry bonus (from Hand to Hand, WP skills)
  - Free defense (doesn't use action)
  - Melee attacks only
  - Limited uses per round (based on Hand to Hand skill level)

### Damage Calculation
- **Weapon Damage:** Dice roll (e.g., 2d6 for vibro-blade, 3d6 MD for laser)
- **Strength Bonus:** PS attribute adds damage to melee
  - PS 16-20: +1
  - PS 21-25: +2
  - PS 26-30: +3
  - PS 31+: +4
- **Damage Application:**
  1. Armor AR check (if AR > strike roll, no damage)
  2. Subtract from SDC first
  3. When SDC = 0, damage goes to HP
  4. At 0 HP: character is dead/dying

---

## 📊 Data Structures

### CombatState (enum)
```c
typedef enum {
    COMBAT_NONE = 0,        // Not in combat
    COMBAT_INITIATIVE,      // Rolling initiative
    COMBAT_ACTIVE,          // Taking turns
    COMBAT_ENDED            // Combat finished
} CombatState;
```

### CombatParticipant (struct)
```c
typedef struct CombatParticipant {
    char *name;                  // Participant name
    bool is_player;              // Player vs NPC
    void *entity;                // Pointer to Character or NPC struct
    int initiative;              // Current initiative roll
    int actions_remaining;       // Actions left this round
    bool is_defending;           // Currently dodging
    struct CombatParticipant *target;  // Current target
    struct CombatParticipant *next;    // Linked list
} CombatParticipant;
```

### CombatRound (struct)
```c
typedef struct CombatRound {
    CombatState state;              // Current combat state
    CombatParticipant *participants; // Linked list of combatants
    CombatParticipant *current;     // Current turn
    int round_number;               // Combat round counter
} CombatRound;
```

---

## 🛠️ Implementation Steps

### Step 1: Combat Infrastructure (2-3 hours)
**Files:** `src/combat.h`, `src/combat.c`

**Tasks:**
1. Create `combat.h` with:
   - CombatState enum
   - CombatParticipant struct
   - CombatRound struct
   - Function declarations
2. Create `combat.c` with:
   - `combat_init()` - Initialize combat subsystem
   - `combat_start()` - Begin combat between entities
   - `combat_end()` - End combat, cleanup
   - `combat_get_participant()` - Find participant by name
   - `combat_add_participant()` - Add entity to combat
   - `combat_remove_participant()` - Remove entity from combat

**Integration:**
- Modify `driver.c` to call `combat_init()` at startup
- Modify `Makefile` to compile `combat.c`

---

### Step 2: Initiative System (1-2 hours)
**Files:** `src/combat.c`

**Tasks:**
1. Implement `combat_roll_initiative()`:
   - For each participant: SPD + 1d20
   - Sort participants by initiative (highest first)
   - Handle ties (higher SPD wins)
2. Implement `combat_next_turn()`:
   - Advance to next participant
   - Check if round is complete (all acted)
   - Roll new initiative for next round
3. Add initiative display function:
   - Show turn order to all combatants

**Skills Integration:**
- Use Hand to Hand skill level for initiative bonus (+1 per 20% skill)

---

### Step 3: Attack System (3-4 hours)
**Files:** `src/combat.c`, `src/combat.h`

**Tasks:**
1. Implement `combat_attack_melee()`:
   - Roll 1d20 + strike bonus
   - Check if hit (vs target number 8 + modifiers)
   - Apply defender's parry if available
   - Apply weapon damage + PS bonus
   - Send combat messages
2. Implement `combat_attack_ranged()`:
   - Roll 1d20 + WP bonus (from skills)
   - Check range (close/medium/long penalties)
   - No parry allowed (only dodge)
   - Apply weapon damage (no PS bonus)
3. Implement `combat_calculate_damage()`:
   - Roll weapon dice
   - Add PS bonus for melee
   - Check for critical hit (natural 20 = double damage)
   - Return damage amount
4. Implement `combat_apply_damage()`:
   - Check armor AR (if strike roll < AR, no damage)
   - Apply to SDC first
   - Overflow to HP when SDC depleted
   - Check for death (HP <= 0)

**Skills Integration:**
- **Hand to Hand:** +5% = +1 strike, +1 parry
- **WP Sword/Rifle/Pistol:** +5% = +1 strike with that weapon
- **Acrobatics:** Determine dodge count per round

**Formula Reference:**
- Strike bonus = (Hand to Hand % / 20) + (WP % / 20)
- Parry bonus = (Hand to Hand % / 20)
- Dodge bonus = (Acrobatics % / 20) + (Hand to Hand % / 20)

---

### Step 4: Defense System (2-3 hours)
**Files:** `src/combat.c`

**Tasks:**
1. Implement `combat_defend_parry()`:
   - Roll 1d20 + parry bonus
   - Check if parry >= attack roll
   - Decrement parry count for round
   - Send success/failure message
2. Implement `combat_defend_dodge()`:
   - Roll 1d20 + dodge bonus
   - Check if dodge >= attack roll
   - Mark player as "used action" (no attack this round)
   - Send success/failure message
3. Implement `combat_defend_auto()`:
   - Automatically attempt parry if available
   - Fall back to dodge if no parries left

---

### Step 5: Combat Commands (2-3 hours)
**Files:** `src/driver.c`, `src/chargen.c`

**Tasks:**
1. Implement `cmd_attack()`:
   - Parse target name
   - Check if target exists and is valid
   - Start combat if not already in combat
   - Add both entities to combat round
2. Implement `cmd_strike()`:
   - Check if in combat
   - Check if your turn
   - Call `combat_attack_melee()`
3. Implement `cmd_shoot()`:
   - Check if in combat
   - Check if your turn
   - Check if have ranged weapon equipped
   - Call `combat_attack_ranged()`
4. Implement `cmd_dodge()`:
   - Check if in combat
   - Set defending flag
   - Use up action for round
5. Implement `cmd_flee()`:
   - Check if in combat
   - Roll SPD check (1d20 + SPD vs 15)
   - If success: remove from combat
   - If fail: lose action, stay in combat

**Command Integration:**
- Add all commands to driver.c command parser
- Add help text for each command

---

### Step 6: Death & Experience (1-2 hours)
**Files:** `src/combat.c`, `src/chargen.c`

**Tasks:**
1. Implement `combat_check_death()`:
   - Called after damage application
   - If HP <= 0: mark as dead, remove from combat
   - Send death message to all combatants
2. Implement `combat_award_experience()`:
   - Calculate XP based on defeated enemy level
   - Award XP to all participants (divide if party)
   - Send XP gain message
3. Implement respawn system:
   - Dead characters respawn at designated location
   - Restore HP/SDC to 50%
   - Remove from combat
   - XP penalty (lose 10% of current level XP)

**Character struct extension:**
```c
typedef struct {
    // ... existing fields ...
    int experience;
    int level;
} Character;
```

---

### Step 7: Combat Messages & Feedback (1-2 hours)
**Files:** `src/combat.c`

**Tasks:**
1. Create combat message templates:
   - Attack hit: "You strike {target} with {weapon} for {damage} damage!"
   - Attack miss: "You swing at {target} but miss!"
   - Parry success: "You parry {attacker}'s attack!"
   - Dodge success: "You dodge {attacker}'s attack!"
   - Critical hit: "CRITICAL HIT! You strike {target} for {damage} damage!"
   - Death: "{target} falls to the ground, defeated!"
2. Implement `combat_broadcast()`:
   - Send message to all participants in combat
   - Format with colors (red for damage, green for healing, yellow for crits)
3. Create initiative display:
   - Show turn order at start of each round
   - Highlight current turn

---

### Step 8: Testing & Balancing (2-3 hours)

**Test Cases:**
1. ✅ Two players can initiate combat
2. ✅ Initiative rolls correctly (SPD + 1d20)
3. ✅ Turn order is correct
4. ✅ Melee attacks apply damage
5. ✅ Ranged attacks work
6. ✅ Parry reduces damage
7. ✅ Dodge uses action
8. ✅ Critical hits double damage
9. ✅ Death removes from combat
10. ✅ Experience is awarded
11. ✅ Flee command works
12. ✅ Multiple rounds work correctly

**Balance Testing:**
- Cyber-Knight (high Hand to Hand) vs Ley Line Walker (low Hand to Hand)
- Juicer (high SPD) vs Body Fixer (low SPD)
- Melee combat vs ranged combat
- Test with 3+ participants

**Create test script: `test_combat.sh`**

---

## 📦 Deliverables

### Code Files (New)
- [ ] `src/combat.h` (150 lines) - Combat structures and API
- [ ] `src/combat.c` (800 lines) - Combat system implementation

### Code Files (Modified)
- [ ] `src/driver.c` - Add combat commands, combat_init() call
- [ ] `src/chargen.c` - Add cmd_attack, cmd_strike, cmd_shoot, cmd_dodge, cmd_flee
- [ ] `src/chargen.h` - Add experience, level to Character struct
- [ ] `Makefile` - Add combat.c to build

### Documentation
- [ ] `PHASE3_IMPLEMENTATION.md` - Complete Phase 3 summary
- [ ] Update `README.md` with combat commands

### Testing
- [ ] `test_combat.sh` - Automated combat test script
- [ ] Manual testing: 2-player combat, 3+ player combat, flee, death

---

## 🔧 Skills Integration Matrix

| Skill | Combat Application | Bonus Formula |
|-------|-------------------|---------------|
| Hand to Hand | Strike bonus, Parry bonus, Initiative | +1 per 20% |
| Acrobatics | Dodge bonus, Dodge count | +1 per 20% |
| WP Sword | Melee strike with swords | +1 per 20% |
| WP Rifle | Ranged strike with rifles | +1 per 20% |
| WP Pistol | Ranged strike with pistols | +1 per 20% |

**Usage tracking:**
- Each combat action increments skill.uses
- Every 10 uses = +1% to skill (implemented in Phase 2 refinement)

---

## 📈 Success Criteria

**Functional Requirements:**
- ✅ Players can initiate combat with `attack` command
- ✅ Initiative system works correctly
- ✅ Turn-based combat flow is smooth
- ✅ Attacks hit/miss based on rolls
- ✅ Damage reduces SDC/HP correctly
- ✅ Death removes player from combat
- ✅ Experience is awarded
- ✅ Flee command works 70% of the time

**Technical Requirements:**
- ✅ Clean compilation (0 errors)
- ✅ No memory leaks in combat system
- ✅ Combat state persists across rounds
- ✅ All combat messages display correctly

**Balance Requirements:**
- ✅ Combat lasts 3-5 rounds on average
- ✅ High-skill OCCs have clear advantage
- ✅ Skills contribute meaningfully to combat
- ✅ Death is possible but not too frequent

---

## 🔗 Dependencies & Integration

**Phase 2 Skills (Dependencies):**
- Hand to Hand skill → strike/parry bonuses
- WP skills → weapon strike bonuses
- Acrobatics skill → dodge bonuses

**Phase 1 OCCs (Dependencies):**
- OCC attributes (PS, PP, SPD) → combat modifiers
- OCC starting gear → initial weapons (placeholder until Phase 4)

**Phase 4 Equipment (Future):**
- Weapon damage tables
- Armor AR/SDC values
- Equipment restrictions

---

## 🚀 Implementation Timeline

**Day 1 (3-4 hours):**
- Step 1: Combat infrastructure
- Step 2: Initiative system

**Day 2 (4-5 hours):**
- Step 3: Attack system
- Skills integration for strike bonuses

**Day 3 (3-4 hours):**
- Step 4: Defense system
- Parry/dodge implementation

**Day 4 (3-4 hours):**
- Step 5: Combat commands
- Driver.c integration

**Day 5 (2-3 hours):**
- Step 6: Death & experience
- Step 7: Combat messages

**Day 6-7 (4-6 hours):**
- Step 8: Testing & balancing
- Bug fixes and refinement
- Documentation

**Total Estimated Time:** 19-25 hours (5-7 days at 3-4 hours/day)

---

## 📋 Post-Phase 3 Refinements

After Phase 3 is complete and stable:

1. **Skill Advancement:**
   - Track skill usage in combat
   - Increment skill % every 10 uses
   - Display skill improvement messages

2. **Advanced Combat Mechanics:**
   - Called shots (target specific locations)
   - Cover and concealment
   - Simultaneous attacks
   - Burst fire for automatic weapons

3. **Status Effects:**
   - Stunned (lose 1 action)
   - Bleeding (damage over time)
   - Unconscious (0 HP but not dead)

4. **Combat AI (NPCs):**
   - Basic decision making
   - Target selection
   - Flee when low HP

---

## 🎯 Next Phase Preview: Phase 4 Equipment

After combat is working, Phase 4 will add:
- Item database (weapons, armor, consumables)
- Inventory system with weight limits
- Equipment slots (weapon, armor, accessories)
- Equipment bonuses to attributes
- 20+ weapons with varied damage types
- 15+ armor types with AR/SDC values

This will complete the core gameplay loop: Character → Skills → Combat → Equipment → Quests

---

**Phase 3 Plan Complete**  
**Ready to begin implementation**
