# Phase 2: Skills System Implementation Plan

## Date: February 1, 2026
## Status: PLANNING
## Scope: Core Skills Framework + 15-20 foundation skills + OCC-specific packages

---

## 📋 Overview

Transform character creation to include starting skill packages based on OCC selection. Implement a skill system engine in C with individual skill objects in LPC.

### Deliverables
- ✅ skills.c / skills.h - C-based skill system engine
- ✅ 15-20 core skills defined
- ✅ OCC-to-skill mappings for all 35 OCCs
- ✅ Character save/load with skills
- ✅ `skills` command to list character skills
- 🔄 Skill check system (1d100 vs skill %)
- 🔄 Skill use command framework

---

## 🎯 Core Skills (15-20 Foundation)

### Physical (3)
1. **Hand to Hand - Basic** (default for all)
2. **Acrobatics** (dodge, balance, parkour)
3. **Swimming** (aquatic movement)

### Technical (4)
1. **Computer Operations** (hacking, data access)
2. **Mechanics** (vehicle/robot repair)
3. **Electronics** (device creation/repair)
4. **Literacy** (reading, writing)

### Weapons (3)
1. **WP Sword** (melee blade weapons)
2. **WP Rifle** (energy rifles)
3. **WP Pistol** (energy pistols)

### Medical (2)
1. **First Aid** (basic healing)
2. **Paramedic** (critical healing)

### Wilderness (2)
1. **Survival** (wilderness survival)
2. **Tracking** (find targets)

### Magical (1)
1. **Magic - Novice** (foundation for spellcasters)

### Psionic (1)
1. **Psionics - Novice** (foundation for psychics)

---

## 📊 Data Structure

```c
typedef struct {
    const char *name;           /* "Hand to Hand - Basic" */
    const char *category;       /* "Physical" */
    int base_percentage;        /* Starting skill % */
    int modifier_by_stat;       /* PP, PS, MA, ME, PE, SPD, etc */
    const char *description;    /* What the skill does */
} SkillDef;

typedef struct {
    char name[64];              /* Skill name */
    int percentage;             /* Current skill % (0-100+) */
    int uses;                   /* Times used (for advancement) */
} PlayerSkill;
```

---

## 🎯 OCC Skill Packages (All 35 OCCs)

### Format Example
```
OCC: Cyber-Knight
Starting Skills:
  - Hand to Hand - Expert (60%)
  - WP Sword (70%)
  - WP Pistol (50%)
  - Computer Operations (40%)
  - Psionics - Novice (30%)
  - Acrobatics (40%)

Starting ISP: 4d6
Starting PPE: 2d6
```

---

## 📁 Files to Create/Modify

### New Files
- `src/skills.c` - Skill system engine
- `src/skills.h` - Skill definitions and structures
- `tools/skill_generator.py` - Generate skill arrays

### Modified Files
- `src/chargen.c` - Add skill package assignment
- `src/session_internal.h` - Add player skills array
- `src/driver.c` - Add `skills` command
- Character save format - Include skills

### LPC Objects (Phase 2B)
- `std/lib/skills/base_skill.lpc` - Skill base object
- `std/lib/skills/physical_skill.lpc` - Physical skills
- `std/lib/skills/technical_skill.lpc` - Technical skills
- etc.

---

## 🛠️ Implementation Steps

### Step 1: Design Phase (1-2 hours)
- [x] Define skill data structures
- [x] List 15-20 core skills
- [x] Map each OCC to skill packages
- [ ] Define skill percentage formulas

### Step 2: Core Engine (2-3 hours)
- [ ] Create skills.h with SkillDef array
- [ ] Create skills.c with:
  - `skill_init()` - Load skills
  - `skill_get_by_name()` - Find skill
  - `skill_assign_occ()` - Apply OCC package
  - `skill_check()` - Roll vs skill %

### Step 3: Integration (2-3 hours)
- [ ] Update PlayerSession struct
- [ ] Add skills to character save format
- [ ] Implement `skills` command
- [ ] Update chargen_init() for skill selection

### Step 4: Testing (1-2 hours)
- [ ] Test all 35 OCC skill packages
- [ ] Test character creation flow
- [ ] Test save/load with skills
- [ ] Test `skills` command output

### Step 5: Documentation (30 min)
- [ ] Update PHASE2_COMPLETE.md
- [ ] Create skill list reference
- [ ] Add skill usage examples

---

## 🔗 OCC → Skill Package Mappings

### Template
Each OCC gets:
- 3-5 mandatory starting skills
- 1-2 bonus skills (based on OCC focus)
- Starting skill percentages
- ISP/PPE allocation

### Examples

**Cyber-Knight (Combat + Tech + Psionics)**
- Hand to Hand - Expert (65%)
- WP Sword (70%), WP Pistol (55%)
- Computer Operations (45%)
- Psionics - Novice (40%)
- Acrobatics (50%)

**Ley Line Walker (Magic + Knowledge)**
- Hand to Hand - Basic (30%)
- Magic - Novice (80%)
- Literacy (70%)
- Survival (45%)
- WP Pistol (35%)

**Rogue Scientist (Tech + Mechanics)**
- Computer Operations (75%)
- Electronics (70%)
- Mechanics (65%)
- Literacy (80%)
- Hand to Hand - Basic (35%)

**Techno-Wizard (Magic + Tech)**
- Magic - Novice (70%)
- Computer Operations (60%)
- Electronics (65%)
- Literacy (75%)
- WP Pistol (40%)

---

## 📈 Skill Progression Model (Phase 2B)

### Level 1 Advancement
- Each skill starts at OCC base percentage
- Skill max = 95% (base) + stat bonus
- Each successful use increments counter
- Every 10 uses = +1% (can increase indefinitely)

### Example
```
Initial: Hand to Hand 65%
After 10 uses: 66%
After 20 uses: 67%
...max 95% + bonuses
```

---

## ✅ Success Criteria

Phase 2 COMPLETE when:
- [ ] All 35 OCCs have skill packages defined
- [ ] Character creation flow includes skill display
- [ ] `skills` command lists all character skills
- [ ] Skill percentages save and load correctly
- [ ] All 15-20 core skills are defined
- [ ] Skill check function working (1d100 vs %)
- [ ] No compilation errors
- [ ] Test: Create character, verify skills assigned, verify `skills` output

---

## 📊 Estimated Timeline

| Task | Hours | Day |
|------|-------|-----|
| Step 1: Design Phase | 2 | Day 1 |
| Step 2: Core Engine | 3 | Day 1-2 |
| Step 3: Integration | 3 | Day 2 |
| Step 4: Testing | 2 | Day 2 |
| Step 5: Documentation | 0.5 | Day 2 |
| **TOTAL** | **10.5** | **2-3 days** |

---

## 🎯 Next Phase Dependencies

- **Phase 3 (Combat)** requires:
  - Skill system (for attack rolls, dodge, parry)
  - Weapon proficiencies (for weapon damage)
  - Hand to Hand variants (for unarmed combat)

- **Phase 5 (Magic/Psionics)** requires:
  - Magic/Psionics novice skills
  - Spell/power lists per OCC
  - Casting/manifesting skill checks

---

## 📌 Notes

1. **C + LPC Bridge:**
   - C engine handles: skill lookups, checks, stat application
   - LPC objects define: individual skills, advanced functionality

2. **Skill Percentage Formula:**
   - Base = OCC starting %
   - Bonus = (Stat Modifier × 5) if applicable
   - Max = 95% + bonuses

3. **Backward Compatibility:**
   - Existing characters can still load
   - New skill field defaults to empty
   - Character can gain skills on login

4. **Extensibility:**
   - Easy to add 30+ more skills later
   - OCC packages can be updated
   - Skill advancement formula is configurable

---

**Status:** PLANNING COMPLETE → Ready for implementation
**Version:** v0.2.1 (skills framework)
**Next:** Begin Step 1 & 2 implementation
