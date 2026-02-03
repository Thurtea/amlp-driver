#ifndef ITEM_H
#define ITEM_H

#include <stdbool.h>

/* Forward declarations to avoid circular includes */
typedef struct PlayerSession PlayerSession;
struct Character;

/* Item Types */
typedef enum {
    ITEM_WEAPON_MELEE = 0,
    ITEM_WEAPON_RANGED,
    ITEM_ARMOR,
    ITEM_CONSUMABLE,
    ITEM_ACCESSORY,
    ITEM_TOOL,
    ITEM_QUEST,
    ITEM_MISC
} ItemType;

/* Weapon Types */
typedef enum {
    WEAPON_UNARMED = 0,
    WEAPON_SWORD,
    WEAPON_AXE,
    WEAPON_KNIFE,
    WEAPON_PISTOL,
    WEAPON_RIFLE,
    WEAPON_ENERGY,
    WEAPON_HEAVY,
    WEAPON_STAFF
} WeaponType;

/* Item Statistics */
typedef struct {
    int damage_dice;        /* Number of dice (e.g., 2 for 2d6) */
    int damage_sides;       /* Sides per die (e.g., 6 for 2d6) */
    int damage_bonus;       /* Flat damage bonus */
    int ar;                 /* Armor Rating (roll must exceed to hit) */
    int sdc_mdc;            /* Structural/Mega Damage Capacity */
    bool is_mega_damage;    /* True if MDC, false if SDC */
    int ps_bonus;           /* Strength bonus modifier */
    int pp_bonus;           /* Prowess bonus modifier */
    int strike_bonus;       /* To-hit bonus */
    int parry_bonus;        /* Parry defense bonus */
    int dodge_bonus;        /* Dodge defense bonus */
    int hp_restore;         /* HP restored (for consumables) */
    int isp_restore;        /* ISP restored (for consumables) */
    int ppe_restore;        /* PPE restored (for consumables) */
} ItemStats;

/* Item Structure */
typedef struct Item {
    int id;                     /* Unique item ID */
    char *name;                 /* Item name */
    char *description;          /* Item description */
    ItemType type;              /* Item type */
    WeaponType weapon_type;     /* If weapon, specific type */
    int weight;                 /* Weight in pounds */
    int value;                  /* Credit value */
    ItemStats stats;            /* Item statistics */
    bool is_equipped;           /* Currently equipped flag */
    int stack_count;            /* For stackable items */
    int current_durability;     /* Current condition (for armor) */
    struct Item *next;          /* Linked list */
} Item;

/* Inventory Structure */
typedef struct {
    Item *items;                /* Linked list of items */
    int total_weight;           /* Current carried weight */
    int max_weight;             /* Weight capacity (PS * 10) */
    int item_count;             /* Number of items */
} Inventory;

/* Equipment Slots Structure */
typedef struct {
    Item *weapon_primary;       /* Main weapon */
    Item *weapon_secondary;     /* Off-hand weapon */
    Item *armor;                /* Body armor */
    Item *accessory1;           /* Ring, amulet, etc. */
    Item *accessory2;
    Item *accessory3;
} EquipmentSlots;

/* Item System Functions */
void item_init(void);
Item* item_create(int template_id);
void item_free(Item *item);
Item* item_clone(Item *item);
Item* item_find_by_id(int id);
Item* item_find_by_name(const char *name);
const char* item_type_to_string(ItemType type);
const char* weapon_type_to_string(WeaponType type);

/* Inventory Functions */
void inventory_init(Inventory *inv, int ps_stat);
bool inventory_add(Inventory *inv, Item *item);
Item* inventory_remove(Inventory *inv, const char *item_name);
Item* inventory_find(Inventory *inv, const char *item_name);
int inventory_get_weight(Inventory *inv);
bool inventory_can_carry(Inventory *inv, int additional_weight);
void inventory_free(Inventory *inv);
void inventory_display(PlayerSession *sess);

/* Equipment Functions */
void equipment_init(EquipmentSlots *eq);
bool equipment_equip(struct Character *ch, PlayerSession *sess, Item *item);
Item* equipment_unequip(struct Character *ch, const char *slot_name);
void equipment_get_bonuses(EquipmentSlots *eq, int *strike, int *parry, int *dodge, int *ar);
bool equipment_can_equip(struct Character *ch, Item *item);
void equipment_free(EquipmentSlots *eq);
void equipment_display(PlayerSession *sess);

/* Item Database */
#define TOTAL_ITEM_TEMPLATES 50

extern Item ITEM_TEMPLATES[TOTAL_ITEM_TEMPLATES];

#endif /* ITEM_H */
