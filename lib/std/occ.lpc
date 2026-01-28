// /lib/std/occ.lpc
// Base O.C.C. (Occupational Character Class) Object for Rifts/Palladium MUD
//
// All OCC files in /occs/ should inherit from this base class.
// Provides the foundational structure for class abilities, skills,
// level progression, and equipment packages.
//
// Usage:
//   inherit "/std/occ";
//   void create() {
//       ::create();
//       set_occ_name("Vagabond");
//       set_occ_id("vagabond");
//       // ... configure OCC-specific attributes
//   }

#include <stats.h>
#include <occs.h>

// OCC identification
string occ_name;            // Display name: "Cyber-Knight"
string occ_id;              // Internal ID: "cyber_knight"
string occ_description;     // Full class description
string occ_category;        // Category: adventurer, military, magic, etc.
string occ_alignment_req;   // Alignment requirements (good, selfish, evil, any)

// Stat requirements (minimum stats to select this OCC)
mapping stat_requirements;  // ([ "iq": 10, "ps": 14 ])

// Stat bonuses (applied on class selection)
mapping stat_bonuses;       // ([ "pe": 2, "spd": 4 ])

// Experience and leveling
int current_level;          // Current level (0 = not yet assigned)
int current_xp;             // Current experience points
int* xp_table;              // XP required for each level (use XP_LEVELS default)

// OCC abilities by level
mapping occ_abilities;      // ([ 1: ({ "ability1", "ability2" }), 3: ({ "ability3" }) ])
string* base_abilities;     // Abilities granted at level 1

// Combat bonuses per level
mapping combat_progression; // ([ 1: ([ "attacks": 2, "strike": 0 ]), 5: ([ "attacks": 3 ]) ])
int base_attacks;           // Attacks per melee round at level 1
int attacks_per_level;      // Additional attacks gained (every N levels)
int attacks_level_interval; // Gain attack every N levels

// Skill system
string* occ_skills;         // Automatic OCC skills at full bonus
string* occ_related_skills; // OCC related skills (reduced bonus)
int occ_skill_count;        // Number of OCC related skills to pick
string* elective_categories;// Allowed elective skill categories
int elective_count;         // Number of elective skills to pick
int secondary_count;        // Number of secondary skills to pick
mapping skill_bonuses;      // ([ "pilot": 10, "technical": 5 ]) category bonuses

// PPE/ISP (Magic/Psionics)
int base_ppe;               // Base PPE for magic classes
int ppe_per_level;          // PPE gained per level
int base_isp;               // Base ISP for psionic classes
int isp_per_level;          // ISP gained per level
string magic_type;          // Type of magic: "none", "ley_line", "techno_wizard", etc.
string psionic_type;        // Type: "none", "minor", "major", "master"

// HP/SDC/MDC progression
int base_hp;                // Starting HP (usually PE + 1d6)
int hp_per_level;           // HP gained per level (usually 1d6)
int base_sdc;               // Starting SDC
int sdc_per_level;          // SDC gained per level
int is_mdc_class;           // 1 = This OCC converts character to MDC
int mdc_per_level;          // MDC gained per level (for MDC classes)

// Equipment packages
string* standard_equipment; // Standard gear everyone gets
string* optional_gear;      // Optional equipment choices
int starting_credits_min;   // Minimum starting credits
int starting_credits_max;   // Maximum starting credits
string* starting_weapons;   // Default weapons
string* starting_armor;     // Default armor options

// Race restrictions
string* race_requirements;  // Empty = any race, otherwise must be in list
string* race_exclusions;    // Races that cannot select this OCC

// Cybernetics
int cybernetics_allowed;    // 0 = none, 1 = limited, 2 = full
int max_cybernetic_slots;   // Maximum cybernetic modifications
string* starting_cybernetics; // Cybernetics included with class

// Special class features
string* special_powers;     // Unique class powers/abilities
mapping power_levels;       // ([ "power_name": starting_level ])
string vehicle_type;        // Special vehicle: "robot", "power_armor", "none"
string special_training;    // Special training type: "hand_to_hand_*", etc.
string hand_to_hand_type;   // HTH style: "basic", "expert", "martial_arts", "assassin"

// Roleplaying
string background;          // Typical background
string* organizations;      // Related organizations/groups
string* enemies;            // Common enemies

// Creation and initialization
void create() {
    occ_name = "Unknown OCC";
    occ_id = "unknown";
    occ_description = "An undefined occupational character class.";
    occ_category = OCC_CAT_ADVENTURER;
    occ_alignment_req = "any";
    
    stat_requirements = ([]);
    stat_bonuses = ([]);
    
    current_level = 0;
    current_xp = 0;
    xp_table = XP_LEVELS;  // Use default from occs.h
    
    occ_abilities = ([]);
    base_abilities = ({});
    
    combat_progression = ([]);
    base_attacks = 2;
    attacks_per_level = 1;
    attacks_level_interval = 3;  // Gain 1 attack every 3 levels
    
    occ_skills = ({});
    occ_related_skills = ({});
    occ_skill_count = 6;
    elective_categories = ({});
    elective_count = 4;
    secondary_count = 6;
    skill_bonuses = ([]);
    
    base_ppe = 0;
    ppe_per_level = 0;
    base_isp = 0;
    isp_per_level = 0;
    magic_type = "none";
    psionic_type = "none";
    
    base_hp = 0;  // Usually PE + 1d6
    hp_per_level = 0;  // Usually 1d6
    base_sdc = 0;
    sdc_per_level = 0;
    is_mdc_class = 0;
    mdc_per_level = 0;
    
    standard_equipment = ({});
    optional_gear = ({});
    starting_credits_min = 0;
    starting_credits_max = 0;
    starting_weapons = ({});
    starting_armor = ({});
    
    race_requirements = ({});
    race_exclusions = ({});
    
    cybernetics_allowed = 1;  // Limited by default
    max_cybernetic_slots = 4;
    starting_cybernetics = ({});
    
    special_powers = ({});
    power_levels = ([]);
    vehicle_type = "none";
    special_training = "";
    hand_to_hand_type = "basic";
    
    background = "";
    organizations = ({});
    enemies = ({});
}

// =============================================================================
// SETTERS
// =============================================================================

void set_occ_name(string name) {
    occ_name = name;
}

void set_occ_id(string id) {
    occ_id = id;
}

void set_occ_description(string desc) {
    occ_description = desc;
}

void set_occ_category(string category) {
    occ_category = category;
}

void set_alignment_requirement(string req) {
    occ_alignment_req = req;
}

void set_stat_requirement(string stat, int value) {
    if (!stat_requirements) stat_requirements = ([]);
    stat_requirements[stat] = value;
}

void set_stat_requirements(mapping reqs) {
    stat_requirements = reqs;
}

void set_stat_bonus(string stat, int value) {
    if (!stat_bonuses) stat_bonuses = ([]);
    stat_bonuses[stat] = value;
}

void set_stat_bonuses(mapping bonuses) {
    stat_bonuses = bonuses;
}

void set_xp_table(int* table) {
    xp_table = table;
}

void set_occ_abilities(mapping abilities) {
    occ_abilities = abilities;
}

void add_ability_at_level(int level, string ability) {
    if (!occ_abilities) occ_abilities = ([]);
    if (!occ_abilities[level]) occ_abilities[level] = ({});
    occ_abilities[level] += ({ ability });
}

void set_base_abilities(string* abilities) {
    base_abilities = abilities;
}

void set_combat_progression(mapping progression) {
    combat_progression = progression;
}

void set_base_attacks(int attacks) {
    base_attacks = attacks;
}

void set_attacks_per_level(int attacks, int interval) {
    attacks_per_level = attacks;
    attacks_level_interval = interval;
}

void set_occ_skills(string* skills) {
    occ_skills = skills;
}

void set_occ_related_skills(string* skills) {
    occ_related_skills = skills;
}

void set_occ_skill_count(int count) {
    occ_skill_count = count;
}

void set_elective_categories(string* categories) {
    elective_categories = categories;
}

void set_elective_count(int count) {
    elective_count = count;
}

void set_secondary_count(int count) {
    secondary_count = count;
}

void set_skill_bonus(string category, int bonus) {
    if (!skill_bonuses) skill_bonuses = ([]);
    skill_bonuses[category] = bonus;
}

void set_skill_bonuses(mapping bonuses) {
    skill_bonuses = bonuses;
}

void set_base_ppe(int ppe) {
    base_ppe = ppe;
}

void set_ppe_per_level(int ppe) {
    ppe_per_level = ppe;
}

void set_base_isp(int isp) {
    base_isp = isp;
}

void set_isp_per_level(int isp) {
    isp_per_level = isp;
}

void set_magic_type(string type) {
    magic_type = type;
}

void set_psionic_type(string type) {
    psionic_type = type;
}

void set_base_hp(int hp) {
    base_hp = hp;
}

void set_hp_per_level(int hp) {
    hp_per_level = hp;
}

void set_base_sdc(int sdc) {
    base_sdc = sdc;
}

void set_sdc_per_level(int sdc) {
    sdc_per_level = sdc;
}

void set_mdc_class(int flag) {
    is_mdc_class = flag;
}

void set_mdc_per_level(int mdc) {
    mdc_per_level = mdc;
    if (mdc > 0) is_mdc_class = 1;
}

void set_standard_equipment(string* equipment) {
    standard_equipment = equipment;
}

void set_optional_gear(string* gear) {
    optional_gear = gear;
}

void set_starting_credits(int min_credits, int max_credits) {
    starting_credits_min = min_credits;
    starting_credits_max = max_credits;
}

void set_starting_weapons(string* weapons) {
    starting_weapons = weapons;
}

void set_starting_armor(string* armor) {
    starting_armor = armor;
}

void set_race_requirements(string* races) {
    race_requirements = races;
}

void set_race_exclusions(string* races) {
    race_exclusions = races;
}

void set_cybernetics_allowed(int level) {
    cybernetics_allowed = level;
}

void set_max_cybernetic_slots(int slots) {
    max_cybernetic_slots = slots;
}

void set_starting_cybernetics(string* cyber) {
    starting_cybernetics = cyber;
}

void set_special_powers(string* powers) {
    special_powers = powers;
}

void add_special_power(string power, int level) {
    if (!special_powers) special_powers = ({});
    if (!power_levels) power_levels = ([]);
    
    if (member_array(power, special_powers) == -1) {
        special_powers += ({ power });
    }
    power_levels[power] = level;
}

void set_vehicle_type(string type) {
    vehicle_type = type;
}

void set_special_training(string training) {
    special_training = training;
}

void set_hand_to_hand_type(string hth) {
    hand_to_hand_type = hth;
}

void set_background(string bg) {
    background = bg;
}

void set_organizations(string* orgs) {
    organizations = orgs;
}

void set_enemies(string* enemy_list) {
    enemies = enemy_list;
}

// =============================================================================
// GETTERS
// =============================================================================

string query_occ_name() {
    return occ_name;
}

string query_occ_id() {
    return occ_id;
}

string query_occ_description() {
    return occ_description;
}

string query_occ_category() {
    return occ_category;
}

string query_alignment_requirement() {
    return occ_alignment_req;
}

mapping query_stat_requirements() {
    return stat_requirements ? stat_requirements : ([]);
}

int query_stat_requirement(string stat) {
    if (stat_requirements && stat_requirements[stat]) {
        return stat_requirements[stat];
    }
    return 0;
}

mapping query_stat_bonuses() {
    return stat_bonuses ? stat_bonuses : ([]);
}

int query_stat_bonus(string stat) {
    if (stat_bonuses && stat_bonuses[stat]) {
        return stat_bonuses[stat];
    }
    return 0;
}

int query_level() {
    return current_level;
}

int query_xp() {
    return current_xp;
}

int query_xp_for_level(int level) {
    if (level < 1) return 0;
    if (level > sizeof(xp_table)) return xp_table[sizeof(xp_table) - 1] * 2;
    return xp_table[level - 1];
}

int query_xp_to_next_level() {
    int next_level = current_level + 1;
    if (next_level > sizeof(xp_table)) {
        return (xp_table[sizeof(xp_table) - 1] * 2) - current_xp;
    }
    return query_xp_for_level(next_level) - current_xp;
}

mapping query_occ_abilities() {
    return occ_abilities ? occ_abilities : ([]);
}

string* query_abilities_at_level(int level) {
    if (occ_abilities && occ_abilities[level]) {
        return occ_abilities[level];
    }
    return ({});
}

string* query_base_abilities() {
    return base_abilities ? base_abilities : ({});
}

// Get all abilities for current level and below
string* query_current_abilities() {
    string* abilities = base_abilities ? copy(base_abilities) : ({});
    
    for (int lvl = 1; lvl <= current_level; lvl++) {
        if (occ_abilities && occ_abilities[lvl]) {
            abilities += occ_abilities[lvl];
        }
    }
    
    return abilities;
}

int query_base_attacks() {
    return base_attacks;
}

int query_attacks_at_level(int level) {
    int attacks = base_attacks;
    if (attacks_level_interval > 0) {
        attacks += (level / attacks_level_interval) * attacks_per_level;
    }
    return attacks;
}

int query_current_attacks() {
    return query_attacks_at_level(current_level);
}

string* query_occ_skills() {
    return occ_skills ? occ_skills : ({});
}

string* query_occ_related_skills() {
    return occ_related_skills ? occ_related_skills : ({});
}

int query_occ_skill_count() {
    return occ_skill_count;
}

string* query_elective_categories() {
    return elective_categories ? elective_categories : ({});
}

int query_elective_count() {
    return elective_count;
}

int query_secondary_count() {
    return secondary_count;
}

mapping query_skill_bonuses() {
    return skill_bonuses ? skill_bonuses : ([]);
}

int query_skill_bonus(string category) {
    if (skill_bonuses && skill_bonuses[category]) {
        return skill_bonuses[category];
    }
    return 0;
}

int query_base_ppe() {
    return base_ppe;
}

int query_ppe_per_level() {
    return ppe_per_level;
}

int query_ppe_at_level(int level) {
    return base_ppe + (ppe_per_level * (level - 1));
}

int query_base_isp() {
    return base_isp;
}

int query_isp_per_level() {
    return isp_per_level;
}

int query_isp_at_level(int level) {
    return base_isp + (isp_per_level * (level - 1));
}

string query_magic_type() {
    return magic_type;
}

string query_psionic_type() {
    return psionic_type;
}

int query_base_hp() {
    return base_hp;
}

int query_hp_per_level() {
    return hp_per_level;
}

int query_base_sdc() {
    return base_sdc;
}

int query_sdc_per_level() {
    return sdc_per_level;
}

int query_is_mdc_class() {
    return is_mdc_class;
}

int query_mdc_per_level() {
    return mdc_per_level;
}

string* query_standard_equipment() {
    return standard_equipment ? standard_equipment : ({});
}

string* query_optional_gear() {
    return optional_gear ? optional_gear : ({});
}

int query_starting_credits_min() {
    return starting_credits_min;
}

int query_starting_credits_max() {
    return starting_credits_max;
}

int roll_starting_credits() {
    if (starting_credits_max == starting_credits_min) {
        return starting_credits_min;
    }
    int range = starting_credits_max - starting_credits_min;
    return starting_credits_min + random(range + 1);
}

string* query_starting_weapons() {
    return starting_weapons ? starting_weapons : ({});
}

string* query_starting_armor() {
    return starting_armor ? starting_armor : ({});
}

string* query_race_requirements() {
    return race_requirements ? race_requirements : ({});
}

string* query_race_exclusions() {
    return race_exclusions ? race_exclusions : ({});
}

int query_cybernetics_allowed() {
    return cybernetics_allowed;
}

int query_max_cybernetic_slots() {
    return max_cybernetic_slots;
}

string* query_starting_cybernetics() {
    return starting_cybernetics ? starting_cybernetics : ({});
}

string* query_special_powers() {
    return special_powers ? special_powers : ({});
}

mapping query_power_levels() {
    return power_levels ? power_levels : ([]);
}

int query_power_level(string power) {
    if (power_levels && power_levels[power]) {
        return power_levels[power];
    }
    return 0;
}

string query_vehicle_type() {
    return vehicle_type;
}

string query_special_training() {
    return special_training;
}

string query_hand_to_hand_type() {
    return hand_to_hand_type;
}

string query_background() {
    return background;
}

string* query_organizations() {
    return organizations ? organizations : ({});
}

string* query_enemies() {
    return enemies ? enemies : ({});
}

// =============================================================================
// VALIDATION METHODS
// =============================================================================

// Check if a player meets stat requirements for this OCC
int meets_stat_requirements(object player) {
    if (!player) return 0;
    if (!sizeof(stat_requirements)) return 1;
    
    foreach (string stat in keys(stat_requirements)) {
        if (player->query_base_stat(stat) < stat_requirements[stat]) {
            return 0;
        }
    }
    return 1;
}

// Check if a race can select this OCC
int race_can_select(string race_id) {
    // Check exclusions first
    if (sizeof(race_exclusions) && member_array(race_id, race_exclusions) != -1) {
        return 0;
    }
    
    // If no requirements list, all non-excluded races are allowed
    if (!sizeof(race_requirements)) {
        return 1;
    }
    
    // Must be in requirements list
    return member_array(race_id, race_requirements) != -1;
}

// Check if alignment is valid for this OCC
int alignment_allowed(string alignment) {
    if (occ_alignment_req == "any") return 1;
    
    if (occ_alignment_req == "good") {
        return alignment == ALIGN_PRINCIPLED || 
               alignment == ALIGN_SCRUPULOUS;
    }
    if (occ_alignment_req == "selfish") {
        return alignment == ALIGN_UNPRINCIPLED ||
               alignment == ALIGN_ANARCHIST;
    }
    if (occ_alignment_req == "evil") {
        return alignment == ALIGN_ABERRANT ||
               alignment == ALIGN_MISCREANT ||
               alignment == ALIGN_DIABOLIC;
    }
    if (occ_alignment_req == "not_evil") {
        return alignment != ALIGN_ABERRANT &&
               alignment != ALIGN_MISCREANT &&
               alignment != ALIGN_DIABOLIC;
    }
    
    return 1;
}

// =============================================================================
// CHARACTER APPLICATION METHODS
// =============================================================================

// Apply OCC to a player during character creation
void apply_to_player(object player) {
    if (!player) return;
    
    // Set starting level
    current_level = 1;
    current_xp = 0;
    player->set_level(1);
    player->set_xp(0);
    
    // Apply stat bonuses
    foreach (string stat in keys(stat_bonuses)) {
        int current = player->query_base_stat(stat);
        player->set_base_stat(stat, current + stat_bonuses[stat]);
    }
    
    // Set HP (PE + rolled dice)
    int pe = player->query_stat("pe");
    int starting_hp = pe + random(6) + 1;  // PE + 1d6
    if (base_hp > 0) starting_hp = base_hp;
    player->set_max_hp(starting_hp);
    player->set_hp(starting_hp);
    
    // Set SDC
    int starting_sdc = base_sdc > 0 ? base_sdc : 0;
    player->add_sdc(starting_sdc);
    
    // Set PPE/ISP if magical/psionic class
    if (base_ppe > 0) {
        player->set_max_ppe(base_ppe);
        player->set_ppe(base_ppe);
    }
    if (base_isp > 0) {
        player->set_max_isp(base_isp);
        player->set_isp(base_isp);
    }
    
    // Set combat stats
    player->set_attacks_per_round(base_attacks);
    player->set_hand_to_hand(hand_to_hand_type);
    
    // Grant base abilities
    foreach (string ability in base_abilities) {
        player->grant_ability(ability, 1);
    }
    
    // Grant level 1 abilities
    if (occ_abilities && occ_abilities[1]) {
        foreach (string ability in occ_abilities[1]) {
            int level = power_levels[ability] ? power_levels[ability] : 1;
            player->grant_ability(ability, level);
        }
    }
    
    // Grant special powers
    foreach (string power in special_powers) {
        int level = power_levels[power] ? power_levels[power] : 1;
        player->grant_power(power, level);
    }
    
    // Grant OCC skills at full proficiency
    foreach (string skill in occ_skills) {
        player->add_skill(skill, 1, 1);  // skill, proficiency, is_occ_skill
    }
    
    // Set cybernetics limits
    player->set_max_cybernetics(max_cybernetic_slots);
    player->set_cybernetics_allowed(cybernetics_allowed);
    
    // Install starting cybernetics
    foreach (string cyber in starting_cybernetics) {
        player->add_cybernetic(cyber);
    }
    
    // Give standard equipment
    foreach (string item_path in standard_equipment) {
        object item = clone_object(item_path);
        if (item) {
            item->move(player);
        }
    }
    
    // Give starting weapons
    foreach (string weapon_path in starting_weapons) {
        object weapon = clone_object(weapon_path);
        if (weapon) {
            weapon->move(player);
        }
    }
    
    // Give starting armor
    foreach (string armor_path in starting_armor) {
        object armor = clone_object(armor_path);
        if (armor) {
            armor->move(player);
        }
    }
    
    // Give starting credits
    int credits = roll_starting_credits();
    player->add_credits(credits);
}

// Apply level up to a player
void level_up(object player) {
    if (!player) return;
    
    current_level++;
    player->set_level(current_level);
    
    // Roll HP gain
    int hp_gain = hp_per_level > 0 ? (random(hp_per_level) + 1) : (random(6) + 1);
    player->add_max_hp(hp_gain);
    
    // Add SDC if applicable
    if (sdc_per_level > 0) {
        player->add_sdc(sdc_per_level);
    }
    
    // Add PPE/ISP if applicable
    if (ppe_per_level > 0) {
        player->add_max_ppe(ppe_per_level);
    }
    if (isp_per_level > 0) {
        player->add_max_isp(isp_per_level);
    }
    
    // Add MDC if MDC class
    if (is_mdc_class && mdc_per_level > 0) {
        player->add_mdc(mdc_per_level);
    }
    
    // Update attacks if new interval hit
    int new_attacks = query_attacks_at_level(current_level);
    player->set_attacks_per_round(new_attacks);
    
    // Grant abilities for this level
    if (occ_abilities && occ_abilities[current_level]) {
        foreach (string ability in occ_abilities[current_level]) {
            int level = power_levels[ability] ? power_levels[ability] : 1;
            player->grant_ability(ability, level);
        }
    }
    
    // Apply combat progression if defined
    if (combat_progression && combat_progression[current_level]) {
        mapping bonuses = combat_progression[current_level];
        foreach (string bonus in keys(bonuses)) {
            if (bonus == "attacks") {
                player->set_attacks_per_round(bonuses[bonus]);
            } else {
                player->add_combat_bonus(bonus, bonuses[bonus]);
            }
        }
    }
    
    // Notify player
    tell_object(player, "\n*** You have advanced to level " + current_level + "! ***\n");
    tell_object(player, "HP gained: " + hp_gain + "\n");
}

// Award experience and check for level up
int award_xp(object player, int xp) {
    if (!player || xp <= 0) return 0;
    
    current_xp += xp;
    player->set_xp(current_xp);
    
    // Check for level up
    int levels_gained = 0;
    while (current_xp >= query_xp_for_level(current_level + 1)) {
        level_up(player);
        levels_gained++;
    }
    
    return levels_gained;
}

// =============================================================================
// DISPLAY METHODS
// =============================================================================

// Get a formatted OCC info display
string get_occ_info() {
    string info = "";
    
    info += "=== " + occ_name + " ===\n\n";
    info += occ_description + "\n\n";
    
    info += "Category: " + capitalize(occ_category) + "\n";
    info += "Alignment: " + capitalize(occ_alignment_req) + "\n";
    
    // Stat requirements
    if (sizeof(stat_requirements)) {
        info += "\nStat Requirements:\n";
        foreach (string stat in keys(stat_requirements)) {
            info += "  " + upper_case(stat) + ": " + stat_requirements[stat] + "+\n";
        }
    }
    
    // Stat bonuses
    if (sizeof(stat_bonuses)) {
        info += "\nStat Bonuses:\n";
        foreach (string stat in keys(stat_bonuses)) {
            int bonus = stat_bonuses[stat];
            string sign = bonus >= 0 ? "+" : "";
            info += "  " + upper_case(stat) + ": " + sign + bonus + "\n";
        }
    }
    
    // Combat
    info += "\nCombat:\n";
    info += "  Base Attacks: " + base_attacks + "\n";
    info += "  Hand-to-Hand: " + capitalize(replace_string(hand_to_hand_type, "_", " ")) + "\n";
    
    // Magic/Psionics
    if (base_ppe > 0 || base_isp > 0) {
        info += "\nMystical Abilities:\n";
        if (base_ppe > 0) {
            info += "  Base P.P.E.: " + base_ppe + " (+" + ppe_per_level + "/level)\n";
            info += "  Magic Type: " + capitalize(replace_string(magic_type, "_", " ")) + "\n";
        }
        if (base_isp > 0) {
            info += "  Base I.S.P.: " + base_isp + " (+" + isp_per_level + "/level)\n";
            info += "  Psionic Type: " + capitalize(psionic_type) + "\n";
        }
    }
    
    // Special powers
    if (sizeof(special_powers)) {
        info += "\nSpecial Powers:\n";
        foreach (string power in special_powers) {
            info += "  - " + capitalize(replace_string(power, "_", " ")) + "\n";
        }
    }
    
    // OCC Skills
    if (sizeof(occ_skills)) {
        info += "\nO.C.C. Skills:\n";
        foreach (string skill in occ_skills) {
            info += "  - " + capitalize(replace_string(skill, "_", " ")) + "\n";
        }
    }
    
    // Race restrictions
    if (sizeof(race_requirements)) {
        info += "\nAvailable To:\n";
        foreach (string race in race_requirements) {
            info += "  - " + capitalize(replace_string(race, "_", " ")) + "\n";
        }
    }
    if (sizeof(race_exclusions)) {
        info += "\nNot Available To:\n";
        foreach (string race in race_exclusions) {
            info += "  - " + capitalize(replace_string(race, "_", " ")) + "\n";
        }
    }
    
    // Cybernetics
    if (cybernetics_allowed > 0) {
        info += "\nCybernetics: ";
        if (cybernetics_allowed == 1) info += "Limited (" + max_cybernetic_slots + " slots)\n";
        else info += "Full (" + max_cybernetic_slots + " slots)\n";
    } else {
        info += "\nCybernetics: Not Allowed\n";
    }
    
    // Starting credits
    if (starting_credits_max > 0) {
        info += "\nStarting Credits: " + starting_credits_min + " - " + starting_credits_max + "\n";
    }
    
    return info;
}

// Get short one-line description
string get_short_description() {
    return occ_name + " - " + capitalize(occ_category);
}

// Get XP progress display
string get_xp_progress() {
    string progress = "";
    int next_xp = query_xp_for_level(current_level + 1);
    int to_go = next_xp - current_xp;
    
    progress += "Level: " + current_level + "\n";
    progress += "XP: " + current_xp + " / " + next_xp + "\n";
    progress += "To Next Level: " + to_go + " XP\n";
    
    // Simple progress bar
    int percent = (current_xp * 100) / next_xp;
    int filled = percent / 5;
    string bar = "[";
    for (int i = 0; i < 20; i++) {
        if (i < filled) bar += "=";
        else bar += " ";
    }
    bar += "] " + percent + "%";
    progress += bar + "\n";
    
    return progress;
}
