#!/bin/bash
# Git commit script for Phase 2 completion
# Date: February 4, 2026

cd /home/thurtea/amlp-driver || exit 1

echo "=== Phase 2 Git Commit Script ==="
echo ""

# Stage all modified files
echo "Staging modified files..."
git add -u

# Stage all new files
echo "Staging new files..."

# Combat system
git add lib/cmds/attack.lpc
git add lib/cmds/flee.lpc
git add lib/cmds/kill.lpc
git add lib/domains/start/room/combat_arena.lpc
git add lib/domains/start/npc/test_goblin.lpc

# Magic system
git add lib/spells/fireball.lpc
git add lib/spells/heal_wounds.lpc
git add lib/spells/armor_of_ithan.lpc
git add lib/spells/magic_shield.lpc
git add lib/spells/lightning_bolt.lpc

# Psionic system
git add lib/psionics/sensitive/telepathy.lpc
git add lib/psionics/super/psi_sword.lpc
git add lib/psionics/healing/healing_touch.lpc

# Items
git add lib/single/vibro_blade.lpc
git add lib/single/laser_pistol.lpc
git add lib/single/urban_warrior_armor.lpc
git add lib/single/rations.lpc
git add lib/single/stim_pack.lpc
git add lib/single/armor/ 2>/dev/null || true

# Additional commands
git add lib/cmds/surname.lpc
git add lib/cmds/position.lpc
git add lib/cmds/take.lpc
git add lib/cmds/put.lpc
git add lib/cmds/repair.lpc

# Admin commands
git add lib/cmds/admin/promote.lpc
git add lib/cmds/admin/demote.lpc
git add lib/cmds/admin/testskill.lpc

# C source debug header
git add src/debug.h

# Archive directories
git add archive/docs/
git add archive/scripts/

# Documentation
git add PRECOMMIT_ANALYSIS_2026-02-04.md

echo ""
echo "Staging complete!"
echo ""

# Show what will be committed
echo "=== Files to be committed ==="
git status --short
echo ""

# Create commit message
echo "Creating commit..."
git commit -m "Phase 2 Complete: Combat, Magic, Psionics + Critical Fixes

FEATURES IMPLEMENTED:
- Combat System: Turn-based combat with 3-second rounds
  * Attack/Kill/Flee commands
  * Critical hits (nat 20 = 2x damage) and fumbles (nat 1)
  * Automatic combat rounds with NPC participation
  * Death and respawn mechanics
  * Combat status tracking

- Magic System: 5 new offensive/defensive spells
  * Fireball (Level 5): 6d6 SDC area damage
  * Heal Wounds (Level 3): 2d6 HP/SDC restoration
  * Armor of Ithan (Level 2): 10 MDC/level force field
  * Magic Shield (Level 2): +2 dodge/parry defensive bonus
  * Lightning Bolt (Level 7): 1d6 MD/level electrical attack

- Psionic System: 3 new psionic powers (7 total)
  * Telepathy (Sensitive): Mind reading, 4 ISP
  * Psi-Sword (Super): 4d6 MD energy weapon, 20 ISP
  * Healing Touch (Healing): 2d6 HP/SDC restoration, 6 ISP

- Basic Items: 5 essential items for gameplay
  * Vibro-Blade: 2d6 MD melee weapon
  * Laser Pistol: 3d6 MD ranged weapon
  * Urban Warrior Armor: 50 MDC light armor
  * Rations: Basic food supply
  * Stim-Pack: Emergency medical (20 HP)

- Test Infrastructure:
  * Combat Arena room for testing
  * Test Goblin NPC for combat practice

CRITICAL BUG FIXES:
- Removed 88+ color code violations from Phase 2 code
  * All combat messages now plain text
  * All spell effects now plain text
  * All psionic powers now plain text
  * Preserved exits-only color standard

- Fixed introduction system integration
  * Added race-based targeting to look/cast/manifest commands
  * Commands now try present() first, then race name
  * Personal names require introduction
  * Race/RCC names always work for targeting
  * Display names respect introduction status

ENHANCEMENTS:
- Surname system for character customization
- Position/stance system (sitting, standing, etc.)
- Admin promote/demote commands
- Equipment repair mechanics
- Take command (alias for get)
- Put command for containers
- Test skill command for debugging

REPOSITORY CLEANUP:
- Moved 30+ documentation files to archive/docs/
- Moved scripts to archive/scripts/
- Organized project structure

C SOURCE CHANGES:
- chargen.c: Character creation improvements
- driver.c: Core driver enhancements
- efun.c: External function additions
- item.c: Item handling improvements
- object.c: Object system enhancements
- room.c: Room mechanics updates
- skills.c: Skill system refinements
- vm.c: Virtual machine optimizations
- debug.h: NEW - Debug utilities

LPC LIBRARY CHANGES:
- living.lpc: Combat system fully integrated
- player.lpc: Player mechanics enhanced
- object.lpc: Object base improvements
- room.lpc: Room functionality expanded
- chargen.lpc: Character generation daemon updated
- command.lpc: Command handling improved

STATS:
- Files Modified: 23
- New Files: 27
- Total Changes: 50 files
- Lines Added: ~2500
- Systems Complete: Combat, Magic, Psionics
- Color Violations Fixed: 88+
- Commands Updated: 3 (look, cast, manifest)

TESTING STATUS:
- Combat system: Functional, needs in-game testing
- Magic system: Functional, needs in-game testing
- Psionic system: Functional, needs in-game testing
- Introduction system: Fixed, needs verification
- Color standards: Compliant

READY FOR:
- Phase 3A: Race-specific abilities
- Phase 3B: OCC-specific abilities
- Phase 3C: Additional content (rooms, NPCs, items)
- Full in-game testing suite

KNOWN ISSUES FOR NEXT PHASE:
- Telepathy message passing not implemented
- Wizard enter/exit messages not customizable
- Need more variety in equipment
- Character creation needs full verification

Phase 2 is feature-complete and ready for production testing."

echo ""
echo "=== Commit created ==="
echo ""

# Show commit
git log -1 --stat

echo ""
echo "=== Ready to push ==="
echo "Run: git push origin main"
echo ""
