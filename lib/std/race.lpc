// /lib/std/race.lpc
// Base Race Object for Rifts/Palladium MUD
//
// All race files in /races/ should inherit from this base class.
// Provides the foundational structure for racial attributes, abilities,
// restrictions, and modifiers.
//
// Usage:
//   inherit "/std/race";
//   void create() {
//       ::create();
//       set_race_name("Human");
//       set_race_id("human");
//       // ... configure race-specific attributes
//   }

#include <stats.h>
#include <races.h>

// Race identification
string race_name;           // Display name: "Human"
string race_id;             // Internal ID: "human"
string race_plural;         // Plural form: "Humans"
string race_description;    // Full description of the race
string race_category;       // Category: human, demihuman, supernatural, etc.

// Physical characteristics
int size_category;          // SIZE_TINY through SIZE_COLOSSAL
int base_height_min;        // Minimum height in inches
int base_height_max;        // Maximum height in inches
int base_weight_min;        // Minimum weight in pounds
int base_weight_max;        // Maximum weight in pounds
int natural_lifespan;       // Average lifespan in years (0 = immortal)
string* body_types;         // Available body types

// Stat modifiers (applied to base stats)
mapping stat_modifiers;     // ([ "ps": 2, "pe": -1 ]) etc.
mapping stat_minimums;      // Minimum stat values
mapping stat_maximums;      // Maximum stat values

// Combat bonuses
mapping combat_bonuses;     // ([ "strike": 1, "dodge": 2 ])
int natural_armor;          // Natural AR (Armor Rating)
int natural_sdc;            // Bonus SDC from race
int natural_mdc;            // For MDC creatures (mega-damage capacity base)
int is_mdc_creature;        // 1 = MDC creature (supernatural/dragon/etc)

// Resistances and immunities (percentage or boolean)
mapping resistances;        // ([ "fire": 50, "cold": 25 ]) = 50% fire resist
string* immunities;         // ({ "poison", "disease" })
string* vulnerabilities;    // ({ "silver", "sunlight" })

// Special abilities and powers
string* racial_abilities;   // List of ability IDs
mapping ability_levels;     // ([ "nightvision": 1, "regeneration": 3 ])
string* racial_verbs;       // Special racial commands/emotes
int psionic_potential;      // Base ISP pool (0 = non-psionic)
int magic_potential;        // Base PPE pool (0 = non-magical)

// OCC restrictions
string* allowed_occs;       // Empty = all allowed, otherwise whitelist
string* forbidden_occs;     // Blacklist of forbidden OCCs
string* recommended_occs;   // Suggested OCCs for this race

// Alignment restrictions
string* allowed_alignments; // Empty = all allowed

// Starting equipment and items
string* starting_equipment; // Default items given at creation
int starting_credits;       // Starting money

// Faction/affiliation
string* default_factions;   // Default faction affiliations
string* hostile_factions;   // Factions that are hostile by default

// Roleplaying information
string culture;             // Cultural background
string homeland;            // Typical homeland
string* languages;          // Languages known at start
string primary_language;    // Native language

// Creation and initialization
void create() {
    race_name = "Unknown Race";
    race_id = "unknown";
    race_plural = "Unknown Races";
    race_description = "An undefined race.";
    race_category = RACE_CAT_HUMAN;
    
    size_category = SIZE_MEDIUM;
    base_height_min = 60;   // 5 feet
    base_height_max = 78;   // 6.5 feet
    base_weight_min = 100;
    base_weight_max = 250;
    natural_lifespan = 80;
    body_types = ({ "average", "thin", "muscular", "heavy" });
    
    stat_modifiers = ([]);
    stat_minimums = ([]);
    stat_maximums = ([]);
    combat_bonuses = ([]);
    
    natural_armor = 0;
    natural_sdc = 0;
    natural_mdc = 0;
    is_mdc_creature = 0;
    
    resistances = ([]);
    immunities = ({});
    vulnerabilities = ({});
    
    racial_abilities = ({});
    ability_levels = ([]);
    racial_verbs = ({});
    psionic_potential = 0;
    magic_potential = 0;
    
    allowed_occs = ({});
    forbidden_occs = ({});
    recommended_occs = ({});
    
    allowed_alignments = ({});
    
    starting_equipment = ({});
    starting_credits = 0;
    
    default_factions = ({});
    hostile_factions = ({});
    
    culture = "";
    homeland = "";
    languages = ({ "american" });
    primary_language = "american";
}

// =============================================================================
// SETTERS
// =============================================================================

void set_race_name(string name) {
    race_name = name;
}

void set_race_id(string id) {
    race_id = id;
}

void set_race_plural(string plural) {
    race_plural = plural;
}

void set_race_description(string desc) {
    race_description = desc;
}

void set_race_category(string category) {
    race_category = category;
}

void set_size_category(int size) {
    size_category = size;
}

void set_height_range(int min_height, int max_height) {
    base_height_min = min_height;
    base_height_max = max_height;
}

void set_weight_range(int min_weight, int max_weight) {
    base_weight_min = min_weight;
    base_weight_max = max_weight;
}

void set_lifespan(int years) {
    natural_lifespan = years;
}

void set_body_types(string* types) {
    body_types = types;
}

void set_stat_modifier(string stat, int modifier) {
    if (!stat_modifiers) stat_modifiers = ([]);
    stat_modifiers[stat] = modifier;
}

void set_stat_modifiers(mapping mods) {
    stat_modifiers = mods;
}

void set_stat_minimum(string stat, int value) {
    if (!stat_minimums) stat_minimums = ([]);
    stat_minimums[stat] = value;
}

void set_stat_maximum(string stat, int value) {
    if (!stat_maximums) stat_maximums = ([]);
    stat_maximums[stat] = value;
}

void set_combat_bonus(string type, int bonus) {
    if (!combat_bonuses) combat_bonuses = ([]);
    combat_bonuses[type] = bonus;
}

void set_combat_bonuses(mapping bonuses) {
    combat_bonuses = bonuses;
}

void set_natural_armor(int ar) {
    natural_armor = ar;
}

void set_natural_sdc(int sdc) {
    natural_sdc = sdc;
}

void set_natural_mdc(int mdc) {
    natural_mdc = mdc;
    if (mdc > 0) is_mdc_creature = 1;
}

void set_mdc_creature(int flag) {
    is_mdc_creature = flag;
}

void set_resistance(string type, int percent) {
    if (!resistances) resistances = ([]);
    resistances[type] = percent;
}

void set_resistances(mapping resists) {
    resistances = resists;
}

void set_immunities(string* immune_list) {
    immunities = immune_list;
}

void add_immunity(string type) {
    if (!immunities) immunities = ({});
    if (member_array(type, immunities) == -1) {
        immunities += ({ type });
    }
}

void set_vulnerabilities(string* vuln_list) {
    vulnerabilities = vuln_list;
}

void add_vulnerability(string type) {
    if (!vulnerabilities) vulnerabilities = ({});
    if (member_array(type, vulnerabilities) == -1) {
        vulnerabilities += ({ type });
    }
}

void set_racial_abilities(string* abilities) {
    racial_abilities = abilities;
}

void add_racial_ability(string ability, int level) {
    if (!racial_abilities) racial_abilities = ({});
    if (!ability_levels) ability_levels = ([]);
    
    if (member_array(ability, racial_abilities) == -1) {
        racial_abilities += ({ ability });
    }
    ability_levels[ability] = level;
}

void set_racial_verbs(string* verbs) {
    racial_verbs = verbs;
}

void set_psionic_potential(int isp) {
    psionic_potential = isp;
}

void set_magic_potential(int ppe) {
    magic_potential = ppe;
}

void set_allowed_occs(string* occs) {
    allowed_occs = occs;
}

void set_forbidden_occs(string* occs) {
    forbidden_occs = occs;
}

void set_recommended_occs(string* occs) {
    recommended_occs = occs;
}

void set_allowed_alignments(string* alignments) {
    allowed_alignments = alignments;
}

void set_starting_equipment(string* equipment) {
    starting_equipment = equipment;
}

void set_starting_credits(int credits) {
    starting_credits = credits;
}

void set_default_factions(string* factions) {
    default_factions = factions;
}

void set_hostile_factions(string* factions) {
    hostile_factions = factions;
}

void set_culture(string cult) {
    culture = cult;
}

void set_homeland(string home) {
    homeland = home;
}

void set_languages(string* langs) {
    languages = langs;
}

void set_primary_language(string lang) {
    primary_language = lang;
    if (member_array(lang, languages) == -1) {
        languages = ({ lang }) + languages;
    }
}

// =============================================================================
// GETTERS
// =============================================================================

string query_race_name() {
    return race_name;
}

string query_race_id() {
    return race_id;
}

string query_race_plural() {
    return race_plural;
}

string query_race_description() {
    return race_description;
}

string query_race_category() {
    return race_category;
}

int query_size_category() {
    return size_category;
}

int* query_height_range() {
    return ({ base_height_min, base_height_max });
}

int* query_weight_range() {
    return ({ base_weight_min, base_weight_max });
}

int query_lifespan() {
    return natural_lifespan;
}

string* query_body_types() {
    return body_types;
}

mapping query_stat_modifiers() {
    return stat_modifiers ? stat_modifiers : ([]);
}

int query_stat_modifier(string stat) {
    if (stat_modifiers && stat_modifiers[stat]) {
        return stat_modifiers[stat];
    }
    return 0;
}

mapping query_stat_minimums() {
    return stat_minimums ? stat_minimums : ([]);
}

mapping query_stat_maximums() {
    return stat_maximums ? stat_maximums : ([]);
}

mapping query_combat_bonuses() {
    return combat_bonuses ? combat_bonuses : ([]);
}

int query_combat_bonus(string type) {
    if (combat_bonuses && combat_bonuses[type]) {
        return combat_bonuses[type];
    }
    return 0;
}

int query_natural_armor() {
    return natural_armor;
}

int query_natural_sdc() {
    return natural_sdc;
}

int query_natural_mdc() {
    return natural_mdc;
}

int query_is_mdc_creature() {
    return is_mdc_creature;
}

mapping query_resistances() {
    return resistances ? resistances : ([]);
}

int query_resistance(string type) {
    if (resistances && resistances[type]) {
        return resistances[type];
    }
    return 0;
}

string* query_immunities() {
    return immunities ? immunities : ({});
}

int query_is_immune(string type) {
    return member_array(type, immunities) != -1;
}

string* query_vulnerabilities() {
    return vulnerabilities ? vulnerabilities : ({});
}

int query_is_vulnerable(string type) {
    return member_array(type, vulnerabilities) != -1;
}

string* query_racial_abilities() {
    return racial_abilities ? racial_abilities : ({});
}

mapping query_ability_levels() {
    return ability_levels ? ability_levels : ([]);
}

int query_ability_level(string ability) {
    if (ability_levels && ability_levels[ability]) {
        return ability_levels[ability];
    }
    return 0;
}

string* query_racial_verbs() {
    return racial_verbs ? racial_verbs : ({});
}

int query_psionic_potential() {
    return psionic_potential;
}

int query_magic_potential() {
    return magic_potential;
}

string* query_allowed_occs() {
    return allowed_occs ? allowed_occs : ({});
}

string* query_forbidden_occs() {
    return forbidden_occs ? forbidden_occs : ({});
}

string* query_recommended_occs() {
    return recommended_occs ? recommended_occs : ({});
}

string* query_allowed_alignments() {
    return allowed_alignments ? allowed_alignments : ({});
}

string* query_starting_equipment() {
    return starting_equipment ? starting_equipment : ({});
}

int query_starting_credits() {
    return starting_credits;
}

string* query_default_factions() {
    return default_factions ? default_factions : ({});
}

string* query_hostile_factions() {
    return hostile_factions ? hostile_factions : ({});
}

string query_culture() {
    return culture;
}

string query_homeland() {
    return homeland;
}

string* query_languages() {
    return languages ? languages : ({});
}

string query_primary_language() {
    return primary_language;
}

// =============================================================================
// VALIDATION METHODS
// =============================================================================

// Check if this race can select a given OCC
int can_select_occ(string occ_id) {
    // If forbidden list contains it, no
    if (sizeof(forbidden_occs) && member_array(occ_id, forbidden_occs) != -1) {
        return 0;
    }
    
    // If allowed list is empty, all non-forbidden are allowed
    if (!sizeof(allowed_occs)) {
        return 1;
    }
    
    // Otherwise must be in allowed list
    return member_array(occ_id, allowed_occs) != -1;
}

// Check if alignment is valid for this race
int can_select_alignment(string alignment) {
    if (!sizeof(allowed_alignments)) {
        return 1; // All allowed
    }
    return member_array(alignment, allowed_alignments) != -1;
}

// =============================================================================
// CHARACTER APPLICATION METHODS
// =============================================================================

// Apply racial modifiers to a player's stats
void apply_to_player(object player) {
    if (!player) return;
    
    // Apply stat modifiers
    string* stats = keys(stat_modifiers);
    foreach (string stat in stats) {
        int current = player->query_base_stat(stat);
        int modifier = stat_modifiers[stat];
        player->set_base_stat(stat, current + modifier);
    }
    
    // Apply stat minimums
    stats = keys(stat_minimums);
    foreach (string stat in stats) {
        int current = player->query_base_stat(stat);
        if (current < stat_minimums[stat]) {
            player->set_base_stat(stat, stat_minimums[stat]);
        }
    }
    
    // Apply stat maximums
    stats = keys(stat_maximums);
    foreach (string stat in stats) {
        int current = player->query_base_stat(stat);
        if (current > stat_maximums[stat]) {
            player->set_base_stat(stat, stat_maximums[stat]);
        }
    }
    
    // Apply combat bonuses
    string* bonuses = keys(combat_bonuses);
    foreach (string bonus in bonuses) {
        player->add_combat_bonus(bonus, combat_bonuses[bonus]);
    }
    
    // Set natural armor/SDC/MDC
    if (natural_armor > 0) {
        player->set_natural_ar(natural_armor);
    }
    if (natural_sdc > 0) {
        player->add_sdc(natural_sdc);
    }
    if (is_mdc_creature && natural_mdc > 0) {
        player->set_mdc(natural_mdc);
    }
    
    // Apply resistances and immunities
    foreach (string resist in keys(resistances)) {
        player->add_resistance(resist, resistances[resist]);
    }
    foreach (string immune in immunities) {
        player->add_immunity(immune);
    }
    foreach (string vuln in vulnerabilities) {
        player->add_vulnerability(vuln);
    }
    
    // Grant racial abilities
    foreach (string ability in racial_abilities) {
        int level = ability_levels[ability] ? ability_levels[ability] : 1;
        player->grant_ability(ability, level);
    }
    
    // Set psionic/magic potential
    if (psionic_potential > 0) {
        player->set_base_isp(psionic_potential);
    }
    if (magic_potential > 0) {
        player->set_base_ppe(magic_potential);
    }
    
    // Add languages
    foreach (string lang in languages) {
        player->add_language(lang);
    }
    player->set_native_language(primary_language);
    
    // Set faction affiliations
    foreach (string faction in default_factions) {
        player->add_faction(faction);
    }
    foreach (string faction in hostile_factions) {
        player->set_hostile_faction(faction);
    }
    
    // Give starting equipment
    foreach (string item_path in starting_equipment) {
        object item = clone_object(item_path);
        if (item) {
            item->move(player);
        }
    }
    
    // Give starting credits
    if (starting_credits > 0) {
        player->add_credits(starting_credits);
    }
}

// Generate random physical characteristics for a player
mapping generate_physical_traits() {
    int height = base_height_min + random(base_height_max - base_height_min + 1);
    int weight = base_weight_min + random(base_weight_max - base_weight_min + 1);
    string body_type = body_types[random(sizeof(body_types))];
    
    return ([
        "height": height,
        "weight": weight,
        "body_type": body_type,
        "size_category": size_category
    ]);
}

// =============================================================================
// DISPLAY METHODS
// =============================================================================

// Get a formatted race info display
string get_race_info() {
    string info = "";
    
    info += "=== " + race_name + " ===\n\n";
    info += race_description + "\n\n";
    
    info += "Category: " + capitalize(race_category) + "\n";
    info += "Size: " + query_size_name() + "\n";
    
    if (natural_lifespan > 0) {
        info += "Lifespan: ~" + natural_lifespan + " years\n";
    } else {
        info += "Lifespan: Immortal\n";
    }
    
    info += "Height: " + (base_height_min / 12) + "'" + (base_height_min % 12) + "\" - " +
            (base_height_max / 12) + "'" + (base_height_max % 12) + "\"\n";
    info += "Weight: " + base_weight_min + " - " + base_weight_max + " lbs\n";
    
    info += "\n";
    
    // Stat modifiers
    if (sizeof(stat_modifiers)) {
        info += "Stat Modifiers:\n";
        foreach (string stat in keys(stat_modifiers)) {
            int mod = stat_modifiers[stat];
            string sign = mod >= 0 ? "+" : "";
            info += "  " + upper_case(stat) + ": " + sign + mod + "\n";
        }
        info += "\n";
    }
    
    // Natural abilities
    if (natural_armor > 0 || natural_sdc > 0 || natural_mdc > 0) {
        info += "Natural Defenses:\n";
        if (natural_armor > 0) info += "  Natural AR: " + natural_armor + "\n";
        if (natural_sdc > 0) info += "  Bonus SDC: +" + natural_sdc + "\n";
        if (natural_mdc > 0) info += "  Base MDC: " + natural_mdc + "\n";
        info += "\n";
    }
    
    // Racial abilities
    if (sizeof(racial_abilities)) {
        info += "Racial Abilities:\n";
        foreach (string ability in racial_abilities) {
            info += "  - " + capitalize(replace_string(ability, "_", " ")) + "\n";
        }
        info += "\n";
    }
    
    // OCC recommendations
    if (sizeof(recommended_occs)) {
        info += "Recommended OCCs:\n";
        foreach (string occ in recommended_occs) {
            info += "  - " + capitalize(replace_string(occ, "_", " ")) + "\n";
        }
        info += "\n";
    }
    
    // Restrictions
    if (sizeof(forbidden_occs)) {
        info += "Forbidden OCCs:\n";
        foreach (string occ in forbidden_occs) {
            info += "  - " + capitalize(replace_string(occ, "_", " ")) + "\n";
        }
        info += "\n";
    }
    
    return info;
}

// Get short one-line description
string get_short_description() {
    return race_name + " - " + capitalize(race_category) + ", " + query_size_name();
}

// Convert size category to name
string query_size_name() {
    switch (size_category) {
        case SIZE_TINY:     return "Tiny";
        case SIZE_SMALL:    return "Small";
        case SIZE_MEDIUM:   return "Medium";
        case SIZE_LARGE:    return "Large";
        case SIZE_HUGE:     return "Huge";
        case SIZE_GIANT:    return "Giant";
        case SIZE_COLOSSAL: return "Colossal";
        default:            return "Unknown";
    }
}
