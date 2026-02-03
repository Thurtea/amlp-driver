#include "magic.h"
#include "chargen.h"
#include "session_internal.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* Forward declaration */
void send_to_player(PlayerSession *session, const char *format, ...);

/* =============== MAGIC SPELLS DATABASE =============== */

MagicSpell MAGIC_SPELLS[34] = {
    /* WARLOCK GRADE SPELLS (0-7) - Level 1-5 */
    {
        .id = 0, .name = "Magic Armor", .description = "AR +4, 1d6 armor per caster level",
        .ppe_cost = 1, .ppe_per_round = 0, .duration_rounds = 60, .base_damage = 0,
        .damage_dice = 0, .damage_sides = 0, .is_mega_damage = false, .range_feet = 0,
        .area_effect_feet = 0, .school = SPELL_WARLOCK, .level_name = "Level 1",
        .casting_time_rounds = 1, .can_overwhelm = false, .is_ritual = false,
        .is_passive_bonus = false, .keywords = "defense armor protection"
    },
    {
        .id = 1, .name = "Detect Magic", .description = "Sense magical auras in area",
        .ppe_cost = 1, .ppe_per_round = 0, .duration_rounds = 10, .base_damage = 0,
        .damage_dice = 0, .damage_sides = 0, .is_mega_damage = false, .range_feet = 100,
        .area_effect_feet = 50, .school = SPELL_WARLOCK, .level_name = "Level 1",
        .casting_time_rounds = 1, .can_overwhelm = false, .is_ritual = false,
        .is_passive_bonus = false, .keywords = "sense detect magic aura"
    },
    {
        .id = 2, .name = "Light", .description = "Create bright magical light source",
        .ppe_cost = 1, .ppe_per_round = 0, .duration_rounds = 120, .base_damage = 0,
        .damage_dice = 0, .damage_sides = 0, .is_mega_damage = false, .range_feet = 50,
        .area_effect_feet = 20, .school = SPELL_WARLOCK, .level_name = "Level 1",
        .casting_time_rounds = 1, .can_overwhelm = false, .is_ritual = false,
        .is_passive_bonus = false, .keywords = "light illumination utility"
    },
    {
        .id = 3, .name = "Mend", .description = "Repair broken item",
        .ppe_cost = 2, .ppe_per_round = 0, .duration_rounds = 1, .base_damage = 0,
        .damage_dice = 0, .damage_sides = 0, .is_mega_damage = false, .range_feet = 10,
        .area_effect_feet = 0, .school = SPELL_WARLOCK, .level_name = "Level 2",
        .casting_time_rounds = 2, .can_overwhelm = false, .is_ritual = true,
        .is_passive_bonus = false, .keywords = "repair utility craft"
    },
    {
        .id = 4, .name = "Magic Shield", .description = "Protective barrier, +2 AR",
        .ppe_cost = 2, .ppe_per_round = 1, .duration_rounds = -1, .base_damage = 0,
        .damage_dice = 0, .damage_sides = 0, .is_mega_damage = false, .range_feet = 0,
        .area_effect_feet = 0, .school = SPELL_WARLOCK, .level_name = "Level 2",
        .casting_time_rounds = 1, .can_overwhelm = false, .is_ritual = false,
        .is_passive_bonus = true, .keywords = "defense shield protection"
    },
    {
        .id = 5, .name = "Identify", .description = "Learn item properties and history",
        .ppe_cost = 3, .ppe_per_round = 0, .duration_rounds = 1, .base_damage = 0,
        .damage_dice = 0, .damage_sides = 0, .is_mega_damage = false, .range_feet = 10,
        .area_effect_feet = 0, .school = SPELL_WARLOCK, .level_name = "Level 3",
        .casting_time_rounds = 2, .can_overwhelm = false, .is_ritual = false,
        .is_passive_bonus = false, .keywords = "knowledge sense identify magic"
    },
    {
        .id = 6, .name = "Alarm", .description = "Set magical trap on object/location",
        .ppe_cost = 2, .ppe_per_round = 0, .duration_rounds = -1, .base_damage = 0,
        .damage_dice = 0, .damage_sides = 0, .is_mega_damage = false, .range_feet = 0,
        .area_effect_feet = 0, .school = SPELL_WARLOCK, .level_name = "Level 2",
        .casting_time_rounds = 1, .can_overwhelm = false, .is_ritual = false,
        .is_passive_bonus = false, .keywords = "defense trap security"
    },
    {
        .id = 7, .name = "Dispel Magic Barrier", .description = "Remove magical protections",
        .ppe_cost = 4, .ppe_per_round = 0, .duration_rounds = 1, .base_damage = 0,
        .damage_dice = 0, .damage_sides = 0, .is_mega_damage = false, .range_feet = 50,
        .area_effect_feet = 0, .school = SPELL_WARLOCK, .level_name = "Level 4",
        .casting_time_rounds = 2, .can_overwhelm = true, .is_ritual = false,
        .is_passive_bonus = false, .keywords = "magic dispel breaker"
    },

    /* MYSTIC/WARLOCK SPELLS (8-15) - Level 5-10 */
    {
        .id = 8, .name = "Fireball", .description = "4d6 MD in 10-foot radius",
        .ppe_cost = 5, .ppe_per_round = 0, .duration_rounds = 1, .base_damage = 0,
        .damage_dice = 4, .damage_sides = 6, .is_mega_damage = true, .range_feet = 150,
        .area_effect_feet = 10, .school = SPELL_MYSTIC, .level_name = "Level 5",
        .casting_time_rounds = 2, .can_overwhelm = true, .is_ritual = false,
        .is_passive_bonus = false, .keywords = "damage fire combat"
    },
    {
        .id = 9, .name = "Lightning Bolt", .description = "2d6 MD ranged electrical bolt",
        .ppe_cost = 5, .ppe_per_round = 0, .duration_rounds = 1, .base_damage = 0,
        .damage_dice = 2, .damage_sides = 6, .is_mega_damage = true, .range_feet = 200,
        .area_effect_feet = 0, .school = SPELL_MYSTIC, .level_name = "Level 5",
        .casting_time_rounds = 1, .can_overwhelm = true, .is_ritual = false,
        .is_passive_bonus = false, .keywords = "damage lightning combat"
    },
    {
        .id = 10, .name = "Teleport", .description = "Move to known location (up to 1 mile)",
        .ppe_cost = 8, .ppe_per_round = 0, .duration_rounds = 1, .base_damage = 0,
        .damage_dice = 0, .damage_sides = 0, .is_mega_damage = false, .range_feet = -1,
        .area_effect_feet = 0, .school = SPELL_MYSTIC, .level_name = "Level 8",
        .casting_time_rounds = 3, .can_overwhelm = false, .is_ritual = false,
        .is_passive_bonus = false, .keywords = "movement travel teleport"
    },
    {
        .id = 11, .name = "Ice Shards", .description = "3d6 MD frozen projectiles",
        .ppe_cost = 4, .ppe_per_round = 0, .duration_rounds = 1, .base_damage = 0,
        .damage_dice = 3, .damage_sides = 6, .is_mega_damage = true, .range_feet = 120,
        .area_effect_feet = 8, .school = SPELL_MYSTIC, .level_name = "Level 5",
        .casting_time_rounds = 1, .can_overwhelm = true, .is_ritual = false,
        .is_passive_bonus = false, .keywords = "damage ice cold combat"
    },
    {
        .id = 12, .name = "Web of Protection", .description = "Immobilize enemies in area",
        .ppe_cost = 3, .ppe_per_round = 1, .duration_rounds = -1, .base_damage = 0,
        .damage_dice = 0, .damage_sides = 0, .is_mega_damage = false, .range_feet = 60,
        .area_effect_feet = 20, .school = SPELL_MYSTIC, .level_name = "Level 5",
        .casting_time_rounds = 2, .can_overwhelm = false, .is_ritual = false,
        .is_passive_bonus = false, .keywords = "control crowd restrain"
    },
    {
        .id = 13, .name = "Summon Lesser Creature", .description = "Call elemental (1d4 rounds)",
        .ppe_cost = 6, .ppe_per_round = 0, .duration_rounds = -1, .base_damage = 0,
        .damage_dice = 0, .damage_sides = 0, .is_mega_damage = false, .range_feet = 30,
        .area_effect_feet = 0, .school = SPELL_MYSTIC, .level_name = "Level 6",
        .casting_time_rounds = 3, .can_overwhelm = false, .is_ritual = true,
        .is_passive_bonus = false, .keywords = "summon creature elemental"
    },
    {
        .id = 14, .name = "Frenzy", .description = "Target attacks faster (+3 attacks)",
        .ppe_cost = 4, .ppe_per_round = 0, .duration_rounds = 6, .base_damage = 0,
        .damage_dice = 0, .damage_sides = 0, .is_mega_damage = false, .range_feet = 30,
        .area_effect_feet = 0, .school = SPELL_BATTLE_MAGE, .level_name = "Level 5",
        .casting_time_rounds = 1, .can_overwhelm = false, .is_ritual = false,
        .is_passive_bonus = false, .keywords = "buff combat speed"
    },
    {
        .id = 15, .name = "Mirror Image", .description = "Create 1d4 duplicate illusions",
        .ppe_cost = 4, .ppe_per_round = 0, .duration_rounds = 20, .base_damage = 0,
        .damage_dice = 0, .damage_sides = 0, .is_mega_damage = false, .range_feet = 0,
        .area_effect_feet = 0, .school = SPELL_MYSTIC, .level_name = "Level 6",
        .casting_time_rounds = 2, .can_overwhelm = false, .is_ritual = false,
        .is_passive_bonus = false, .keywords = "defense illusion deception"
    },

    /* WIZARD GRADE SPELLS (16-25) - Level 10-15 */
    {
        .id = 16, .name = "Rift Teleportation", .description = "Open dimensional portal (50 feet)",
        .ppe_cost = 10, .ppe_per_round = 0, .duration_rounds = 10, .base_damage = 0,
        .damage_dice = 0, .damage_sides = 0, .is_mega_damage = false, .range_feet = -1,
        .area_effect_feet = 0, .school = SPELL_WIZARD, .level_name = "Level 10",
        .casting_time_rounds = 4, .can_overwhelm = false, .is_ritual = false,
        .is_passive_bonus = false, .keywords = "movement teleport rift"
    },
    {
        .id = 17, .name = "Magic Missile", .description = "1d6+1 per missile (up to IQ missiles)",
        .ppe_cost = 3, .ppe_per_round = 0, .duration_rounds = 1, .base_damage = 0,
        .damage_dice = 1, .damage_sides = 6, .is_mega_damage = true, .range_feet = 200,
        .area_effect_feet = 0, .school = SPELL_WIZARD, .level_name = "Level 4",
        .casting_time_rounds = 1, .can_overwhelm = true, .is_ritual = false,
        .is_passive_bonus = false, .keywords = "damage magic combat"
    },
    {
        .id = 18, .name = "Plague", .description = "Disease spreads to enemies in area",
        .ppe_cost = 8, .ppe_per_round = 0, .duration_rounds = -1, .base_damage = 0,
        .damage_dice = 0, .damage_sides = 0, .is_mega_damage = false, .range_feet = 100,
        .area_effect_feet = 30, .school = SPELL_WIZARD, .level_name = "Level 12",
        .casting_time_rounds = 3, .can_overwhelm = false, .is_ritual = true,
        .is_passive_bonus = false, .keywords = "damage disease poison"
    },
    {
        .id = 19, .name = "Summon Greater Creature", .description = "Call powerful demon",
        .ppe_cost = 10, .ppe_per_round = 0, .duration_rounds = -1, .base_damage = 0,
        .damage_dice = 0, .damage_sides = 0, .is_mega_damage = false, .range_feet = 50,
        .area_effect_feet = 0, .school = SPELL_WIZARD, .level_name = "Level 12",
        .casting_time_rounds = 5, .can_overwhelm = false, .is_ritual = true,
        .is_passive_bonus = false, .keywords = "summon creature demon"
    },
    {
        .id = 20, .name = "Meteor Storm", .description = "6d6 MD over 30-foot radius",
        .ppe_cost = 12, .ppe_per_round = 0, .duration_rounds = 1, .base_damage = 0,
        .damage_dice = 6, .damage_sides = 6, .is_mega_damage = true, .range_feet = 300,
        .area_effect_feet = 30, .school = SPELL_WIZARD, .level_name = "Level 15",
        .casting_time_rounds = 3, .can_overwhelm = true, .is_ritual = false,
        .is_passive_bonus = false, .keywords = "damage fire area combat"
    },
    {
        .id = 21, .name = "Time Dilation", .description = "Move at 2x speed for 1d6 rounds",
        .ppe_cost = 10, .ppe_per_round = 0, .duration_rounds = 6, .base_damage = 0,
        .damage_dice = 0, .damage_sides = 0, .is_mega_damage = false, .range_feet = 0,
        .area_effect_feet = 0, .school = SPELL_WIZARD, .level_name = "Level 12",
        .casting_time_rounds = 2, .can_overwhelm = false, .is_ritual = false,
        .is_passive_bonus = false, .keywords = "buff speed movement time"
    },
    {
        .id = 22, .name = "Stone to Flesh", .description = "Reverse petrification curse",
        .ppe_cost = 7, .ppe_per_round = 0, .duration_rounds = 1, .base_damage = 0,
        .damage_dice = 0, .damage_sides = 0, .is_mega_damage = false, .range_feet = 50,
        .area_effect_feet = 0, .school = SPELL_RITUAL, .level_name = "Level 10",
        .casting_time_rounds = 3, .can_overwhelm = false, .is_ritual = true,
        .is_passive_bonus = false, .keywords = "heal restore cure curse"
    },
    {
        .id = 23, .name = "Healing Circle", .description = "Heal all allies 3d6 HP",
        .ppe_cost = 6, .ppe_per_round = 0, .duration_rounds = 1, .base_damage = 0,
        .damage_dice = 3, .damage_sides = 6, .is_mega_damage = false, .range_feet = 50,
        .area_effect_feet = 30, .school = SPELL_RITUAL, .level_name = "Level 10",
        .casting_time_rounds = 2, .can_overwhelm = true, .is_ritual = false,
        .is_passive_bonus = false, .keywords = "heal recovery restoration"
    },
    {
        .id = 24, .name = "Enchant Item", .description = "Add magic bonuses to equipment",
        .ppe_cost = 5, .ppe_per_round = 0, .duration_rounds = -1, .base_damage = 0,
        .damage_dice = 0, .damage_sides = 0, .is_mega_damage = false, .range_feet = 0,
        .area_effect_feet = 0, .school = SPELL_RITUAL, .level_name = "Level 8",
        .casting_time_rounds = 4, .can_overwhelm = false, .is_ritual = true,
        .is_passive_bonus = false, .keywords = "item craft enchantment"
    },
    {
        .id = 25, .name = "Scrying", .description = "See through remote scrying orb",
        .ppe_cost = 4, .ppe_per_round = 1, .duration_rounds = -1, .base_damage = 0,
        .damage_dice = 0, .damage_sides = 0, .is_mega_damage = false, .range_feet = 5280,
        .area_effect_feet = 0, .school = SPELL_WIZARD, .level_name = "Level 8",
        .casting_time_rounds = 2, .can_overwhelm = false, .is_ritual = false,
        .is_passive_bonus = false, .keywords = "vision sense remote scry"
    },

    /* BATTLE MAGE SPELLS (26-31) - Combat Focused */
    {
        .id = 26, .name = "Power Fist", .description = "+2d6 damage to melee attack",
        .ppe_cost = 3, .ppe_per_round = 0, .duration_rounds = 1, .base_damage = 0,
        .damage_dice = 2, .damage_sides = 6, .is_mega_damage = true, .range_feet = 0,
        .area_effect_feet = 0, .school = SPELL_BATTLE_MAGE, .level_name = "Level 3",
        .casting_time_rounds = 1, .can_overwhelm = false, .is_ritual = false,
        .is_passive_bonus = false, .keywords = "damage combat melee buff"
    },
    {
        .id = 27, .name = "Armor Enhancement", .description = "+50 MDC armor temporarily",
        .ppe_cost = 4, .ppe_per_round = 0, .duration_rounds = 10, .base_damage = 0,
        .damage_dice = 0, .damage_sides = 0, .is_mega_damage = false, .range_feet = 0,
        .area_effect_feet = 0, .school = SPELL_BATTLE_MAGE, .level_name = "Level 4",
        .casting_time_rounds = 1, .can_overwhelm = false, .is_ritual = false,
        .is_passive_bonus = true, .keywords = "defense armor protection"
    },
    {
        .id = 28, .name = "Smite", .description = "+4d6 damage on next attack",
        .ppe_cost = 5, .ppe_per_round = 0, .duration_rounds = 2, .base_damage = 0,
        .damage_dice = 4, .damage_sides = 6, .is_mega_damage = true, .range_feet = 0,
        .area_effect_feet = 0, .school = SPELL_BATTLE_MAGE, .level_name = "Level 6",
        .casting_time_rounds = 1, .can_overwhelm = true, .is_ritual = false,
        .is_passive_bonus = false, .keywords = "damage combat buff attack"
    },
    {
        .id = 29, .name = "Protective Circle", .description = "Shield allies, +3 AR",
        .ppe_cost = 6, .ppe_per_round = 1, .duration_rounds = -1, .base_damage = 0,
        .damage_dice = 0, .damage_sides = 0, .is_mega_damage = false, .range_feet = 20,
        .area_effect_feet = 15, .school = SPELL_BATTLE_MAGE, .level_name = "Level 7",
        .casting_time_rounds = 2, .can_overwhelm = false, .is_ritual = false,
        .is_passive_bonus = true, .keywords = "defense protection area"
    },
    {
        .id = 30, .name = "Spell Reflection", .description = "Bounce magical attacks",
        .ppe_cost = 5, .ppe_per_round = 1, .duration_rounds = -1, .base_damage = 0,
        .damage_dice = 0, .damage_sides = 0, .is_mega_damage = false, .range_feet = 0,
        .area_effect_feet = 0, .school = SPELL_WIZARD, .level_name = "Level 9",
        .casting_time_rounds = 2, .can_overwhelm = false, .is_ritual = false,
        .is_passive_bonus = true, .keywords = "defense protection magic"
    },
    {
        .id = 31, .name = "Combat Healing", .description = "Heal 2d6 HP during fight",
        .ppe_cost = 3, .ppe_per_round = 0, .duration_rounds = 1, .base_damage = 0,
        .damage_dice = 2, .damage_sides = 6, .is_mega_damage = false, .range_feet = 0,
        .area_effect_feet = 0, .school = SPELL_BATTLE_MAGE, .level_name = "Level 3",
        .casting_time_rounds = 1, .can_overwhelm = true, .is_ritual = false,
        .is_passive_bonus = false, .keywords = "heal recovery restoration"
    },

    /* RITUAL/CEREMONIAL SPELLS (32-33) */
    {
        .id = 32, .name = "Ritual Healing", .description = "Cure disease, poison, curses",
        .ppe_cost = 4, .ppe_per_round = 0, .duration_rounds = 1, .base_damage = 0,
        .damage_dice = 0, .damage_sides = 0, .is_mega_damage = false, .range_feet = 0,
        .area_effect_feet = 0, .school = SPELL_RITUAL, .level_name = "Level 5",
        .casting_time_rounds = 3, .can_overwhelm = false, .is_ritual = true,
        .is_passive_bonus = false, .keywords = "heal cure disease poison"
    },
    {
        .id = 33, .name = "Resurrection", .description = "Bring character back from death",
        .ppe_cost = 20, .ppe_per_round = 0, .duration_rounds = 1, .base_damage = 0,
        .damage_dice = 0, .damage_sides = 0, .is_mega_damage = false, .range_feet = 0,
        .area_effect_feet = 0, .school = SPELL_RITUAL, .level_name = "Level 20",
        .casting_time_rounds = 10, .can_overwhelm = false, .is_ritual = true,
        .is_passive_bonus = false, .keywords = "heal restore resurrection"
    }
};

int MAGIC_SPELL_COUNT = 34;

/* =============== INITIALIZATION =============== */

void magic_init(void) {
    /* Verify spell database is loaded */
    if (MAGIC_SPELL_COUNT != 34) {
        fprintf(stderr, "WARNING: Magic database count mismatch! Expected 34, got %d\n",
                MAGIC_SPELL_COUNT);
    }
}

/* =============== SPELL LOOKUP =============== */

MagicSpell* magic_find_spell_by_id(int spell_id) {
    if (spell_id < 0 || spell_id >= MAGIC_SPELL_COUNT) {
        return NULL;
    }
    return &MAGIC_SPELLS[spell_id];
}

MagicSpell* magic_find_spell_by_name(const char *name) {
    if (!name) return NULL;
    
    for (int i = 0; i < MAGIC_SPELL_COUNT; i++) {
        if (strcasecmp(MAGIC_SPELLS[i].name, name) == 0) {
            return &MAGIC_SPELLS[i];
        }
    }
    return NULL;
}

KnownSpell* magic_find_known_spell(struct Character *ch, int spell_id) {
    if (!ch || !ch->magic.spells) return NULL;
    
    for (int i = 0; i < ch->magic.spell_count; i++) {
        if (ch->magic.spells[i].spell_id == spell_id) {
            return &ch->magic.spells[i];
        }
    }
    return NULL;
}

/* =============== ABILITY MANAGEMENT =============== */

void magic_init_abilities(struct Character *ch) {
    if (!ch) return;
    
    ch->magic.spells = NULL;
    ch->magic.spell_count = 0;
    
    /* Calculate PPE based on ME and IQ */
    ch->magic.ppe_base = (ch->stats.me * 2) + (ch->stats.iq / 2);
    ch->magic.ppe_max = ch->magic.ppe_base;
    ch->magic.ppe_current = ch->magic.ppe_max;
    ch->magic.ppe_recovery_rate = (ch->stats.me / 10) + 1;
    
    ch->magic.is_casting = false;
    ch->magic.casting_spell_id = -1;
    ch->magic.casting_rounds_remaining = 0;
    
    ch->magic.is_meditating = false;
    ch->magic.meditation_rounds_active = 0;
}

void magic_free_abilities(struct Character *ch) {
    if (!ch) return;
    
    if (ch->magic.spells) {
        free(ch->magic.spells);
        ch->magic.spells = NULL;
    }
    ch->magic.spell_count = 0;
}

void magic_learn_spell(struct Character *ch, int spell_id) {
    if (!ch) return;
    
    /* Check if already knows spell */
    if (magic_find_known_spell(ch, spell_id)) {
        return;  /* Already knows this spell */
    }
    
    /* Expand spells array */
    ch->magic.spell_count++;
    ch->magic.spells = realloc(ch->magic.spells, sizeof(KnownSpell) * ch->magic.spell_count);
    
    /* Add new spell at rank 1 */
    KnownSpell *new_spell = &ch->magic.spells[ch->magic.spell_count - 1];
    new_spell->spell_id = spell_id;
    new_spell->rank = 1;
    new_spell->total_casts = 0;
    new_spell->last_cast = time(NULL);
}

void magic_add_starting_spells(struct Character *ch, const char *occ_name) {
    if (!ch || !occ_name) return;
    
    if (strcasecmp(occ_name, "Ley Line Walker") == 0) {
        magic_learn_spell(ch, 0);   /* Magic Armor */
        magic_learn_spell(ch, 1);   /* Detect Magic */
        magic_learn_spell(ch, 2);   /* Light */
        magic_learn_spell(ch, 8);   /* Fireball */
        magic_learn_spell(ch, 9);   /* Lightning Bolt */
        ch->magic.ppe_max = 50;
        ch->magic.ppe_current = 50;
    }
    else if (strcasecmp(occ_name, "Warlock") == 0) {
        magic_learn_spell(ch, 0);   /* Magic Armor */
        magic_learn_spell(ch, 1);   /* Detect Magic */
        magic_learn_spell(ch, 7);   /* Dispel Magic Barrier */
        magic_learn_spell(ch, 6);   /* Alarm */
        magic_learn_spell(ch, 8);   /* Fireball */
        ch->magic.ppe_max = 45;
        ch->magic.ppe_current = 45;
    }
    else if (strcasecmp(occ_name, "Mystic Wizard") == 0) {
        magic_learn_spell(ch, 0);   /* Magic Armor */
        magic_learn_spell(ch, 1);   /* Detect Magic */
        magic_learn_spell(ch, 8);   /* Fireball */
        magic_learn_spell(ch, 9);   /* Lightning Bolt */
        magic_learn_spell(ch, 10);  /* Teleport */
        magic_learn_spell(ch, 12);  /* Web of Protection */
        ch->magic.ppe_max = 60;
        ch->magic.ppe_current = 60;
    }
    else if (strcasecmp(occ_name, "Techno-Wizard") == 0) {
        magic_learn_spell(ch, 0);   /* Magic Armor */
        magic_learn_spell(ch, 5);   /* Identify */
        magic_learn_spell(ch, 3);   /* Mend */
        magic_learn_spell(ch, 24);  /* Enchant Item */
        ch->magic.ppe_max = 40;
        ch->magic.ppe_current = 40;
    }
    /* For non-magic classes, PPE is still available but no spells start */
}

/* =============== PPE MANAGEMENT =============== */

int magic_get_ppe_max(struct Character *ch) {
    if (!ch) return 0;
    return ch->magic.ppe_max;
}

int magic_get_ppe_current(struct Character *ch) {
    if (!ch) return 0;
    return ch->magic.ppe_current;
}

void magic_set_ppe_current(struct Character *ch, int amount) {
    if (!ch) return;
    if (amount < 0) amount = 0;
    if (amount > ch->magic.ppe_max) amount = ch->magic.ppe_max;
    ch->magic.ppe_current = amount;
}

void magic_recover_ppe(struct Character *ch, int amount) {
    if (!ch) return;
    ch->magic.ppe_current += amount;
    if (ch->magic.ppe_current > ch->magic.ppe_max) {
        ch->magic.ppe_current = ch->magic.ppe_max;
    }
}

void magic_spend_ppe(struct Character *ch, int amount) {
    if (!ch) return;
    ch->magic.ppe_current -= amount;
    if (ch->magic.ppe_current < 0) {
        ch->magic.ppe_current = 0;
    }
}

bool magic_can_cast_spell(struct Character *ch, int spell_id) {
    if (!ch) return false;
    
    MagicSpell *spell = magic_find_spell_by_id(spell_id);
    if (!spell) return false;
    
    /* Check if knows spell */
    if (!magic_find_known_spell(ch, spell_id)) {
        return false;
    }
    
    /* Check PPE cost */
    if (ch->magic.ppe_current < spell->ppe_cost) {
        return false;
    }
    
    return true;
}

/* =============== SPELL CASTING (STUB) =============== */

bool magic_start_casting(PlayerSession *sess, struct Character *ch,
                         int spell_id, const char *target_name) {
    if (!sess || !ch) return false;
    
    if (!magic_can_cast_spell(ch, spell_id)) {
        send_to_player(sess, "You don't have enough PPE to cast that spell.\n");
        return false;
    }
    
    MagicSpell *spell = magic_find_spell_by_id(spell_id);
    if (!spell) return false;
    
    /* Begin casting */
    ch->magic.is_casting = true;
    ch->magic.casting_spell_id = spell_id;
    ch->magic.casting_rounds_remaining = spell->casting_time_rounds;
    
    send_to_player(sess, "You begin casting %s...\n", spell->name);
    
    return true;
}

bool magic_continue_casting(struct Character *ch) {
    if (!ch || !ch->magic.is_casting) return false;
    
    ch->magic.casting_rounds_remaining--;
    return ch->magic.casting_rounds_remaining > 0;
}

bool magic_complete_cast(PlayerSession *sess, struct Character *ch) {
    if (!sess || !ch) return false;
    
    if (!ch->magic.is_casting) return false;
    
    int spell_id = ch->magic.casting_spell_id;
    MagicSpell *spell = magic_find_spell_by_id(spell_id);
    
    if (!spell) {
        ch->magic.is_casting = false;
        return false;
    }
    
    /* Spend PPE */
    magic_spend_ppe(ch, spell->ppe_cost);
    magic_record_spell_cast(ch, spell_id);
    
    /* Simple feedback */
    send_to_player(sess, "You cast %s! (PPE: %d/%d)\n",
                   spell->name, ch->magic.ppe_current, ch->magic.ppe_max);
    
    ch->magic.is_casting = false;
    ch->magic.casting_spell_id = -1;
    return true;
}

void magic_interrupt_cast(struct Character *ch) {
    if (!ch) return;
    ch->magic.is_casting = false;
    ch->magic.casting_spell_id = -1;
    ch->magic.casting_rounds_remaining = 0;
}

/* =============== DISPLAY =============== */

void magic_display_spells(PlayerSession *sess) {
    if (!sess) return;
    
    Character *ch = &sess->character;
    
    send_to_player(sess, "=== KNOWN SPELLS ===\n");
    
    if (ch->magic.spell_count == 0) {
        send_to_player(sess, "You have no magic spells.\n");
        return;
    }
    
    for (int i = 0; i < ch->magic.spell_count; i++) {
        KnownSpell *ks = &ch->magic.spells[i];
        MagicSpell *spell = magic_find_spell_by_id(ks->spell_id);
        
        if (spell) {
            send_to_player(sess, "%d. %s (%d PPE) [RANK %d] - %s\n   %s\n",
                           i + 1, spell->name, spell->ppe_cost, ks->rank, spell->level_name,
                           spell->description);
        }
    }
    
    send_to_player(sess, "\n");
}

void magic_display_ppe(PlayerSession *sess) {
    if (!sess) return;
    
    Character *ch = &sess->character;
    int recovery = ch->magic.ppe_recovery_rate;
    
    send_to_player(sess, "PPE: %d/%d (%d%%) - +%d per round\n",
                   ch->magic.ppe_current, ch->magic.ppe_max,
                   (ch->magic.ppe_current * 100) / ch->magic.ppe_max,
                   recovery);
}

/* =============== ADVANCEMENT =============== */

void magic_record_spell_cast(struct Character *ch, int spell_id) {
    if (!ch) return;
    
    KnownSpell *ks = magic_find_known_spell(ch, spell_id);
    if (!ks) return;
    
    ks->total_casts++;
    ks->last_cast = time(NULL);
    
    magic_check_spell_rank_advance(ch, spell_id);
}

void magic_check_spell_rank_advance(struct Character *ch, int spell_id) {
    if (!ch) return;
    
    KnownSpell *ks = magic_find_known_spell(ch, spell_id);
    if (!ks) return;
    
    /* Advancement table: 40, 60, 80, 100, 120, 140, 160, 180, 200 casts */
    int advancement_table[] = {0, 40, 60, 80, 100, 120, 140, 160, 180, 200};
    
    for (int rank = 1; rank < 10; rank++) {
        if (ks->rank == rank && ks->total_casts >= advancement_table[rank + 1]) {
            ks->rank++;
            return;
        }
    }
}

/* =============== TICKS =============== */

void magic_spell_tick(struct Character *ch) {
    if (!ch) return;
    
    /* Recover PPE naturally each round */
    magic_recover_ppe(ch, ch->magic.ppe_recovery_rate);
    
    /* Handle ongoing casting */
    if (ch->magic.is_casting) {
        if (!magic_continue_casting(ch)) {
            /* Casting complete - would call magic_complete_cast here */
        }
    }
}

void magic_meditate_tick(struct Character *ch) {
    if (!ch) return;
    
    if (ch->magic.is_meditating && ch->magic.meditation_rounds_active > 0) {
        /* Recover +1d6 PPE per meditation round */
        int recover = 1 + (rand() % 6);  /* 1d6 */
        magic_recover_ppe(ch, recover);
        ch->magic.meditation_rounds_active--;
    }
}
