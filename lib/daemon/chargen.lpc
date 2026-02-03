// /lib/daemon/chargen.lpc
// Character Generation Daemon
//
// Manages the character creation process for new players, including
// race selection, OCC selection, stat rolling, and initial setup.

#include <stats.h>
#include <races.h>
#include <occs.h>

// Cache of loaded race and OCC objects
mapping race_cache;
mapping occ_cache;

// Player creation states
#define STATE_NONE          0
#define STATE_RACE          1
#define STATE_RACE_CONFIRM  2
#define STATE_STATS_ROLL    3
#define STATE_STATS_ASSIGN  4
#define STATE_OCC           5
#define STATE_OCC_CONFIRM   6
#define STATE_ALIGNMENT     7
#define STATE_SKILLS        8
#define STATE_EQUIPMENT     9
#define STATE_FINALIZE      10
#define STATE_COMPLETE      11

// Player creation data storage
mapping player_creation;  // ([ player_name: creation_data ])

void create() {
    race_cache = ([]);
    occ_cache = ([]);
    player_creation = ([]);
}

// =============================================================================
// RACE MANAGEMENT
// =============================================================================

// Load a race object and cache it
object load_race(string race_id) {
    if (race_cache[race_id]) {
        return race_cache[race_id];
    }
    
    string path = "/races/" + race_id;
    object race = load_object(path);
    
    if (race) {
        race_cache[race_id] = race;
    }
    
    return race;
}

// Get list of all available races
string* get_available_races() {
    return ALL_RACES;
}

// Get list of races by category
string* get_races_by_category(string category) {
    string* result = ({});
    
    foreach (string race_id in ALL_RACES) {
        object race = load_race(race_id);
        if (race && race->query_race_category() == category) {
            result += ({ race_id });
        }
    }
    
    return result;
}

// Get race information for display
string get_race_info(string race_id) {
    object race = load_race(race_id);
    if (!race) return "Race not found.\n";
    return race->get_race_info();
}

// Get short race list for selection menu
string get_race_selection_menu() {
    string menu = "";
    string* categories = ({
        RACE_CAT_HUMAN, RACE_CAT_DEMIHUMAN, RACE_CAT_MUTANT,
        RACE_CAT_ALIEN, RACE_CAT_DRAGON, RACE_CAT_SUPERNATURAL,
        RACE_CAT_GIANT
    });
    
    menu += "=== Available Races ===\n\n";
    
    foreach (string cat in categories) {
        string* cat_races = get_races_by_category(cat);
        if (sizeof(cat_races)) {
            menu += capitalize(cat) + ":\n";
            int count = 0;
            foreach (string race_id in cat_races) {
                object race = load_race(race_id);
                if (race) {
                    menu += "  " + race_id + " - " + race->query_race_name() + "\n";
                    count++;
                    if (count >= 10) {
                        menu += "  ... and " + (sizeof(cat_races) - 10) + " more\n";
                        break;
                    }
                }
            }
            menu += "\n";
        }
    }
    
    menu += "Type 'race <name>' to see details, or 'choose <name>' to select.\n";
    
    return menu;
}

// =============================================================================
// OCC MANAGEMENT
// =============================================================================

// Load an OCC object and cache it
object load_occ(string occ_id) {
    if (occ_cache[occ_id]) {
        return occ_cache[occ_id];
    }
    
    string path = "/occs/" + occ_id;
    object occ = load_object(path);
    
    if (occ) {
        occ_cache[occ_id] = occ;
    }
    
    return occ;
}

// Get list of all available OCCs
string* get_available_occs() {
    return ALL_OCCS;
}

// Get list of OCCs valid for a race
string* get_valid_occs_for_race(string race_id) {
    string* result = ({});
    object race = load_race(race_id);
    
    if (!race) return result;
    
    foreach (string occ_id in ALL_OCCS) {
        if (race->can_select_occ(occ_id)) {
            object occ = load_occ(occ_id);
            if (occ && occ->race_can_select(race_id)) {
                result += ({ occ_id });
            }
        }
    }
    
    return result;
}

// Get OCC information for display
string get_occ_info(string occ_id) {
    object occ = load_occ(occ_id);
    if (!occ) return "O.C.C. not found.\n";
    return occ->get_occ_info();
}

// Get OCC selection menu filtered by race
string get_occ_selection_menu(string race_id) {
    string menu = "";
    string* valid_occs = get_valid_occs_for_race(race_id);
    string* categories = ({
        OCC_CAT_ADVENTURER, OCC_CAT_MILITARY, OCC_CAT_SCHOLAR,
        OCC_CAT_MAGIC, OCC_CAT_PSIONIC, OCC_CAT_SPECIAL
    });
    
    menu += "=== Available O.C.C.s for ";
    object race = load_race(race_id);
    if (race) menu += race->query_race_name();
    else menu += race_id;
    menu += " ===\n\n";
    
    foreach (string cat in categories) {
        string* cat_occs = ({});
        foreach (string occ_id in valid_occs) {
            object occ = load_occ(occ_id);
            if (occ && occ->query_occ_category() == cat) {
                cat_occs += ({ occ_id });
            }
        }
        
        if (sizeof(cat_occs)) {
            menu += capitalize(cat) + " O.C.C.s:\n";
            foreach (string occ_id in cat_occs) {
                object occ = load_occ(occ_id);
                if (occ) {
                    menu += "  " + occ_id + " - " + occ->query_occ_name() + "\n";
                }
            }
            menu += "\n";
        }
    }
    
    menu += "Type 'occ <name>' to see details, or 'choose <name>' to select.\n";
    
    return menu;
}

// =============================================================================
// STAT ROLLING
// =============================================================================

// Roll a single stat (3d6)
int roll_stat() {
    return (random(6) + 1) + (random(6) + 1) + (random(6) + 1);
}

// Roll a stat with potential exceptional (16+ = roll additional 1d6)
int roll_stat_exceptional() {
    int base = roll_stat();
    if (base >= 16) {
        base += random(6) + 1;  // Exceptional attribute
    }
    return base;
}

// Roll all 8 Palladium stats
mapping roll_all_stats() {
    return ([
        "iq": roll_stat_exceptional(),
        "me": roll_stat_exceptional(),
        "ma": roll_stat_exceptional(),
        "ps": roll_stat_exceptional(),
        "pp": roll_stat_exceptional(),
        "pe": roll_stat_exceptional(),
        "pb": roll_stat_exceptional(),
        "spd": roll_stat_exceptional()
    ]);
}

// Apply racial modifiers to stats
mapping apply_racial_modifiers(mapping stats, string race_id) {
    object race = load_race(race_id);
    if (!race) return stats;
    
    mapping mods = race->query_stat_modifiers();
    mapping minimums = race->query_stat_minimums();
    mapping maximums = race->query_stat_maximums();
    
    // Apply modifiers
    foreach (string stat in keys(mods)) {
        stats[stat] += mods[stat];
    }
    
    // Apply minimums
    foreach (string stat in keys(minimums)) {
        if (stats[stat] < minimums[stat]) {
            stats[stat] = minimums[stat];
        }
    }
    
    // Apply maximums
    foreach (string stat in keys(maximums)) {
        if (stats[stat] > maximums[stat]) {
            stats[stat] = maximums[stat];
        }
    }
    
    return stats;
}

// Display stats in a formatted way
string format_stats(mapping stats) {
    string output = "";
    
    output += "=== Character Attributes ===\n\n";
    output += "I.Q.: " + sprintf("%2d", stats["iq"]) + "  (Intelligence Quotient)\n";
    output += "M.E.: " + sprintf("%2d", stats["me"]) + "  (Mental Endurance)\n";
    output += "M.A.: " + sprintf("%2d", stats["ma"]) + "  (Mental Affinity)\n";
    output += "P.S.: " + sprintf("%2d", stats["ps"]) + "  (Physical Strength)\n";
    output += "P.P.: " + sprintf("%2d", stats["pp"]) + "  (Physical Prowess)\n";
    output += "P.E.: " + sprintf("%2d", stats["pe"]) + "  (Physical Endurance)\n";
    output += "P.B.: " + sprintf("%2d", stats["pb"]) + "  (Physical Beauty)\n";
    output += "Spd:  " + sprintf("%2d", stats["spd"]) + "  (Speed)\n";
    
    return output;
}

// =============================================================================
// ALIGNMENT
// =============================================================================

string* get_all_alignments() {
    return ({
        ALIGN_PRINCIPLED, ALIGN_SCRUPULOUS,
        ALIGN_UNPRINCIPLED, ALIGN_ANARCHIST,
        ALIGN_ABERRANT, ALIGN_MISCREANT, ALIGN_DIABOLIC
    });
}

string get_alignment_description(string alignment) {
    switch (alignment) {
        case ALIGN_PRINCIPLED:
            return "Principled (Good): Follows a strict moral code, always helps the "
                   "innocent, never lies or kills prisoners. The classic 'white knight'.";
        case ALIGN_SCRUPULOUS:
            return "Scrupulous (Good): Has a strong moral compass but is more pragmatic. "
                   "Will lie if necessary to protect innocents, avoids killing when possible.";
        case ALIGN_UNPRINCIPLED:
            return "Unprincipled (Selfish): Looks out for themselves but won't harm "
                   "innocents. Will bend rules and laws for personal gain but has limits.";
        case ALIGN_ANARCHIST:
            return "Anarchist (Selfish): Values freedom above all else, despises authority. "
                   "Unpredictable but not malicious, acts on whims and desires.";
        case ALIGN_ABERRANT:
            return "Aberrant (Evil): Follows a strict code of conduct but for evil purposes. "
                   "Has honor among thieves, keeps word, but ruthless in pursuing goals.";
        case ALIGN_MISCREANT:
            return "Miscreant (Evil): Self-serving and willing to hurt others for gain. "
                   "No moral code, but not sadistically cruel without purpose.";
        case ALIGN_DIABOLIC:
            return "Diabolic (Evil): The ultimate evil - cruel, sadistic, and enjoys "
                   "causing suffering. No limits on their depravity.";
        default:
            return "Unknown alignment.";
    }
}

string get_alignment_menu(string race_id) {
    string menu = "";
    object race = load_race(race_id);
    string* allowed = ({});
    
    if (race) {
        allowed = race->query_allowed_alignments();
    }
    
    if (!sizeof(allowed)) {
        allowed = get_all_alignments();
    }
    
    menu += "=== Choose Your Alignment ===\n\n";
    menu += "Good Alignments:\n";
    foreach (string align in allowed) {
        if (align == ALIGN_PRINCIPLED || align == ALIGN_SCRUPULOUS) {
            menu += "  " + align + "\n";
        }
    }
    menu += "\nSelfish Alignments:\n";
    foreach (string align in allowed) {
        if (align == ALIGN_UNPRINCIPLED || align == ALIGN_ANARCHIST) {
            menu += "  " + align + "\n";
        }
    }
    menu += "\nEvil Alignments:\n";
    foreach (string align in allowed) {
        if (align == ALIGN_ABERRANT || align == ALIGN_MISCREANT || 
            align == ALIGN_DIABOLIC) {
            menu += "  " + align + "\n";
        }
    }
    
    menu += "\nType 'align <name>' to see details, or 'choose <name>' to select.\n";
    
    return menu;
}

// =============================================================================
// CHARACTER CREATION STATE MANAGEMENT
// =============================================================================

// Initialize character creation for a player
void start_creation(object player) {
    string pname = player->query_name();
    
    player_creation[pname] = ([
        "state": STATE_RACE,
        "race": 0,
        "occ": 0,
        "stats": ([]),
        "alignment": 0,
        "skills": ({}),
        "equipment": ({})
    ]);
    
    tell_object(player, "\n" + get_race_selection_menu());
}

// Get current creation state for a player
int query_creation_state(object player) {
    string pname = player->query_name();
    if (!player_creation[pname]) return STATE_NONE;
    return player_creation[pname]["state"];
}

// Get creation data for a player
mapping query_creation_data(object player) {
    string pname = player->query_name();
    if (!player_creation[pname]) return ([]);
    return player_creation[pname];
}

// Process character creation input
int process_input(object player, string input) {
    string pname = player->query_name();
    if (!player_creation[pname]) return 0;
    
    mapping data = player_creation[pname];
    int state = data["state"];
    string* words = explode(lower_case(input), " ");
    string cmd = sizeof(words) ? words[0] : "";
    string arg = sizeof(words) > 1 ? implode(words[1..], " ") : "";
    
    switch (state) {
        case STATE_RACE:
            return process_race_input(player, cmd, arg);
        case STATE_RACE_CONFIRM:
            return process_race_confirm(player, cmd, arg);
        case STATE_STATS_ROLL:
            return process_stats_roll(player, cmd, arg);
        case STATE_OCC:
            return process_occ_input(player, cmd, arg);
        case STATE_OCC_CONFIRM:
            return process_occ_confirm(player, cmd, arg);
        case STATE_ALIGNMENT:
            return process_alignment_input(player, cmd, arg);
        case STATE_FINALIZE:
            return process_finalize(player, cmd, arg);
    }
    
    return 0;
}

// Process race selection input
int process_race_input(object player, string cmd, string arg) {
    string pname = player->query_name();
    mapping data = player_creation[pname];
    
    if (cmd == "race" || cmd == "info" || cmd == "look") {
        if (!arg || arg == "") {
            tell_object(player, get_race_selection_menu());
            return 1;
        }
        
        object race = load_race(arg);
        if (!race) {
            tell_object(player, "Unknown race: " + arg + "\n");
            return 1;
        }
        
        tell_object(player, "\n" + race->get_race_info());
        return 1;
    }
    
    if (cmd == "choose" || cmd == "select" || cmd == "pick") {
        if (!arg || arg == "") {
            tell_object(player, "Choose which race?\n");
            return 1;
        }
        
        object race = load_race(arg);
        if (!race) {
            tell_object(player, "Unknown race: " + arg + "\n");
            return 1;
        }
        
        data["race"] = arg;
        data["state"] = STATE_RACE_CONFIRM;
        
        tell_object(player, "\nYou have chosen: " + race->query_race_name() + "\n\n");
        tell_object(player, race->get_race_info());
        tell_object(player, "\nConfirm this choice? (yes/no)\n");
        return 1;
    }
    
    if (cmd == "list") {
        tell_object(player, get_race_selection_menu());
        return 1;
    }
    
    return 0;
}

// Process race confirmation
int process_race_confirm(object player, string cmd, string arg) {
    string pname = player->query_name();
    mapping data = player_creation[pname];
    
    if (cmd == "yes" || cmd == "y" || cmd == "confirm") {
        data["state"] = STATE_STATS_ROLL;
        
        // Roll initial stats
        mapping stats = roll_all_stats();
        stats = apply_racial_modifiers(stats, data["race"]);
        data["stats"] = stats;
        
        tell_object(player, "\nRace confirmed! Rolling attributes...\n\n");
        tell_object(player, format_stats(stats));
        tell_object(player, "\nAccept these stats? (yes/reroll)\n");
        tell_object(player, "(You may reroll up to 2 times)\n");
        data["rerolls"] = 0;
        return 1;
    }
    
    if (cmd == "no" || cmd == "n" || cmd == "cancel") {
        data["state"] = STATE_RACE;
        data["race"] = 0;
        tell_object(player, "\nRace selection cancelled.\n");
        tell_object(player, get_race_selection_menu());
        return 1;
    }
    
    tell_object(player, "Confirm this race? (yes/no)\n");
    return 1;
}

// Process stat rolling
int process_stats_roll(object player, string cmd, string arg) {
    string pname = player->query_name();
    mapping data = player_creation[pname];
    
    if (cmd == "yes" || cmd == "y" || cmd == "accept") {
        data["state"] = STATE_OCC;
        tell_object(player, "\nAttributes accepted!\n\n");
        tell_object(player, get_occ_selection_menu(data["race"]));
        return 1;
    }
    
    if (cmd == "reroll" || cmd == "roll" || cmd == "r") {
        if (data["rerolls"] >= 2) {
            tell_object(player, "\nYou have used all your rerolls. Accept these stats.\n");
            return 1;
        }
        
        data["rerolls"]++;
        mapping stats = roll_all_stats();
        stats = apply_racial_modifiers(stats, data["race"]);
        data["stats"] = stats;
        
        tell_object(player, "\nRerolling... (" + (2 - data["rerolls"]) + " rerolls remaining)\n\n");
        tell_object(player, format_stats(stats));
        tell_object(player, "\nAccept these stats? (yes/reroll)\n");
        return 1;
    }
    
    tell_object(player, "Accept these stats? (yes/reroll)\n");
    return 1;
}

// Process OCC selection input
int process_occ_input(object player, string cmd, string arg) {
    string pname = player->query_name();
    mapping data = player_creation[pname];
    
    if (cmd == "occ" || cmd == "class" || cmd == "info" || cmd == "look") {
        if (!arg || arg == "") {
            tell_object(player, get_occ_selection_menu(data["race"]));
            return 1;
        }
        
        object occ = load_occ(arg);
        if (!occ) {
            tell_object(player, "Unknown O.C.C.: " + arg + "\n");
            return 1;
        }
        
        tell_object(player, "\n" + occ->get_occ_info());
        return 1;
    }
    
    if (cmd == "choose" || cmd == "select" || cmd == "pick") {
        if (!arg || arg == "") {
            tell_object(player, "Choose which O.C.C.?\n");
            return 1;
        }
        
        object occ = load_occ(arg);
        if (!occ) {
            tell_object(player, "Unknown O.C.C.: " + arg + "\n");
            return 1;
        }
        
        // Check race compatibility
        object race = load_race(data["race"]);
        if (race && !race->can_select_occ(arg)) {
            tell_object(player, "Your race cannot select that O.C.C.\n");
            return 1;
        }
        
        if (!occ->race_can_select(data["race"])) {
            tell_object(player, "That O.C.C. is not available to your race.\n");
            return 1;
        }
        
        // Check stat requirements
        if (!occ->meets_stat_requirements(player)) {
            tell_object(player, "You do not meet the stat requirements:\n");
            mapping reqs = occ->query_stat_requirements();
            foreach (string stat in keys(reqs)) {
                tell_object(player, "  " + upper_case(stat) + ": " + 
                           reqs[stat] + "+ (you have " + data["stats"][stat] + ")\n");
            }
            return 1;
        }
        
        data["occ"] = arg;
        data["state"] = STATE_OCC_CONFIRM;
        
        tell_object(player, "\nYou have chosen: " + occ->query_occ_name() + "\n\n");
        tell_object(player, occ->get_occ_info());
        tell_object(player, "\nConfirm this choice? (yes/no)\n");
        return 1;
    }
    
    if (cmd == "list") {
        tell_object(player, get_occ_selection_menu(data["race"]));
        return 1;
    }
    
    return 0;
}

// Process OCC confirmation
int process_occ_confirm(object player, string cmd, string arg) {
    string pname = player->query_name();
    mapping data = player_creation[pname];
    
    if (cmd == "yes" || cmd == "y" || cmd == "confirm") {
        data["state"] = STATE_ALIGNMENT;
        tell_object(player, "\nO.C.C. confirmed!\n\n");
        tell_object(player, get_alignment_menu(data["race"]));
        return 1;
    }
    
    if (cmd == "no" || cmd == "n" || cmd == "cancel") {
        data["state"] = STATE_OCC;
        data["occ"] = 0;
        tell_object(player, "\nO.C.C. selection cancelled.\n");
        tell_object(player, get_occ_selection_menu(data["race"]));
        return 1;
    }
    
    tell_object(player, "Confirm this O.C.C.? (yes/no)\n");
    return 1;
}

// Process alignment selection
int process_alignment_input(object player, string cmd, string arg) {
    string pname = player->query_name();
    mapping data = player_creation[pname];
    
    if (cmd == "align" || cmd == "alignment" || cmd == "info" || cmd == "look") {
        if (!arg || arg == "") {
            tell_object(player, get_alignment_menu(data["race"]));
            return 1;
        }
        
        tell_object(player, "\n" + get_alignment_description(arg) + "\n");
        return 1;
    }
    
    if (cmd == "choose" || cmd == "select" || cmd == "pick") {
        if (!arg || arg == "") {
            tell_object(player, "Choose which alignment?\n");
            return 1;
        }
        
        string* valid = get_all_alignments();
        if (member_array(arg, valid) == -1) {
            tell_object(player, "Unknown alignment: " + arg + "\n");
            return 1;
        }
        
        // Check OCC alignment requirement
        object occ = load_occ(data["occ"]);
        if (occ && !occ->alignment_allowed(arg)) {
            tell_object(player, "That alignment is not allowed for your O.C.C.\n");
            return 1;
        }
        
        // Check race alignment restriction
        object race = load_race(data["race"]);
        if (race && !race->can_select_alignment(arg)) {
            tell_object(player, "That alignment is not allowed for your race.\n");
            return 1;
        }
        
        data["alignment"] = arg;
        data["state"] = STATE_FINALIZE;
        
        tell_object(player, "\nAlignment set to: " + capitalize(arg) + "\n");
        tell_object(player, "\n=== Character Summary ===\n");
        tell_object(player, show_character_summary(player));
        tell_object(player, "\nFinalize character creation? (yes/no)\n");
        return 1;
    }
    
    if (cmd == "list") {
        tell_object(player, get_alignment_menu(data["race"]));
        return 1;
    }
    
    return 0;
}

// Process finalization
int process_finalize(object player, string cmd, string arg) {
    string pname = player->query_name();
    mapping data = player_creation[pname];
    
    if (cmd == "yes" || cmd == "y" || cmd == "confirm" || cmd == "done") {
        finalize_character(player);
        return 1;
    }
    
    if (cmd == "no" || cmd == "n" || cmd == "back") {
        data["state"] = STATE_ALIGNMENT;
        tell_object(player, "\nReturning to alignment selection...\n");
        tell_object(player, get_alignment_menu(data["race"]));
        return 1;
    }
    
    tell_object(player, "Finalize character? (yes/no)\n");
    return 1;
}

// Show character summary
string show_character_summary(object player) {
    string pname = player->query_name();
    mapping data = player_creation[pname];
    string summary = "";
    
    object race = load_race(data["race"]);
    object occ = load_occ(data["occ"]);
    
    summary += "Name: " + capitalize(pname) + "\n";
    summary += "Race: " + (race ? race->query_race_name() : data["race"]) + "\n";
    summary += "O.C.C.: " + (occ ? occ->query_occ_name() : data["occ"]) + "\n";
    summary += "Alignment: " + capitalize(data["alignment"]) + "\n";
    summary += "\n";
    summary += format_stats(data["stats"]);
    
    return summary;
}

// Finalize character creation and apply to player
void finalize_character(object player) {
    string pname = player->query_name();
    mapping data = player_creation[pname];
    
    object race = load_race(data["race"]);
    object occ = load_occ(data["occ"]);
    
    // Set basic attributes
    player->set_race_id(data["race"]);
    player->set_occ_id(data["occ"]);
    player->set_property("alignment", data["alignment"]);
    
    // Set race name and article
    if (race) {
        player->set_race_name(race->query_race_name());
        player->set_race_article(race->query_race_article());
    }
    
    // Apply stats using Palladium stat system
    foreach (string stat, int value : data["stats"]) {
        player->set_stat(stat, value);
    }
    
    // Calculate HP based on PE
    int pe = data["stats"]["pe"];
    player->set_max_hp(pe);
    player->set_hp(pe);
    
    // Calculate SDC base (10 + PE bonus)
    int pe_bonus = 0;
    if (pe > 16) pe_bonus = (pe - 16);
    player->set_max_sdc(10 + pe_bonus);
    player->set_sdc(10 + pe_bonus);
    
    // =========================================================
    // LANGUAGE SYSTEM SETUP
    // =========================================================
    
    // Everyone knows American by default
    player->add_language("american", 98);
    player->set_primary_language("american");
    
    // Apply racial languages
    if (race) {
        string* race_langs = race->query_native_languages();
        if (race_langs) {
            foreach (string lang : race_langs) {
                player->add_language(lang, 98);  // Native proficiency
            }
        }
        
        // Additional languages at starting proficiency
        string* extra_langs = race->query_bonus_languages();
        if (extra_langs) {
            foreach (string lang : extra_langs) {
                int prof = 50 + (data["stats"]["iq"] - 10) * 2;
                player->add_language(lang, prof);
            }
        }
    }
    
    // OCC language bonuses
    if (occ) {
        string* occ_langs = occ->query_occ_languages();
        if (occ_langs) {
            foreach (string lang : occ_langs) {
                /* If they already know it, improve it, otherwise add at 60% */
                if (player->speaks_language(lang)) {
                    player->improve_language(lang, 10);
                } else {
                    player->add_language(lang, 60);
                }
            }
        }
    }
    
    // =========================================================
    // SKILLS SYSTEM SETUP
    // =========================================================
    
    if (occ) {
        // Get OCC skills (primary skills)
        string* occ_skills = occ->query_occ_skills();
        if (occ_skills) {
            player->set_primary_skills(occ_skills);
            foreach (string skill_id : occ_skills) {
                /* Calculate starting percentage based on skill + level */
                object skill_d = find_object("/daemon/skills");
                if (!skill_d) skill_d = load_object("/daemon/skills");
                
                int pct = 30;  /* Default */
                if (skill_d) {
                    pct = skill_d->calculate_skill_percentage(skill_id, player, 1);
                }
                player->add_skill(skill_id, pct);
            }
        }
        
        // Related skills - will be selected by player later
        // For now, just set up the secondary skills list
        string* related = occ->query_related_skills();
        if (related) {
            /* Player will select from these during gameplay or extended chargen */
        }
    }
    
    // =========================================================
    // PSIONICS SYSTEM SETUP
    // =========================================================
    
    string psi_category = "none";
    
    // Check if race is naturally psionic
    if (race) {
        psi_category = race->query_psionic_category();
    }
    
    // OCC can grant or enhance psionics
    if (occ) {
        string occ_psi = occ->query_psionic_category();
        if (occ_psi && occ_psi != "none") {
            /* OCC psionics override or enhance race */
            if (psi_category == "none" || 
                occ_psi == "master" ||
                (occ_psi == "major" && psi_category == "minor")) {
                psi_category = occ_psi;
            }
        }
    }
    
    player->set_psionic_category(psi_category);
    
    if (psi_category != "none") {
        // Calculate starting ISP
        int me = data["stats"]["me"];
        int isp;
        switch (psi_category) {
            case "minor": isp = me + random(6) + 1; break;
            case "major": isp = me * 2 + random(6) + random(6) + 2; break;
            case "master": isp = me * 4 + random(6) + random(6) + random(6) + 3; break;
            default: isp = 0;
        }
        player->set_max_isp(isp);
        player->set_isp(isp);
        
        // Grant starting psionic powers
        string* starting_powers = ({});
        
        if (race) {
            starting_powers += race->query_racial_powers();
        }
        if (occ) {
            starting_powers += occ->query_occ_powers();
        }
        
        player->set_psionic_powers(starting_powers);
    }
    
    // =========================================================
    // MAGIC SYSTEM SETUP
    // =========================================================
    
    int is_caster = 0;
    
    if (occ) {
        is_caster = occ->query_is_spell_caster();
    }
    
    if (is_caster) {
        // Calculate starting PPE
        int pe_stat = data["stats"]["pe"];
        int ppe = pe_stat + random(6) + random(6) + random(6) + 3;
        player->set_max_ppe(ppe);
        player->set_ppe(ppe);
        
        // Set caster level (starts at 1)
        player->set_spell_caster_level(1);
        
        // Grant starting spells from OCC
        string* starting_spells = occ->query_starting_spells();
        if (starting_spells) {
            foreach (string spell_id : starting_spells) {
                player->add_spell(spell_id, 1);  /* Level 1 */
            }
        }
    }
    
    // =========================================================
    // INTRODUCTION SYSTEM
    // =========================================================
    
    // Set the real name for introduction system
    player->set_real_name(capitalize(pname));
    
    // =========================================================
    // Race/OCC specific bonuses
    // =========================================================
    
    if (race) {
        race->apply_to_player(player);
    }
    
    if (occ) {
        occ->apply_to_player(player);
    }
    
    // Mark creation complete
    data["state"] = STATE_COMPLETE;
    player->set_property("creation_complete", 1);
    
    tell_object(player, "\n*** CHARACTER CREATION COMPLETE! ***\n\n");
    tell_object(player, "Welcome to Rifts Earth, " + capitalize(pname) + "!\n");
    tell_object(player, "Type 'help newbie' for getting started tips.\n");
    tell_object(player, "Use 'stats' to see your character sheet.\n");
    tell_object(player, "Use 'languages' to see your known languages.\n");
    
    if (psi_category != "none") {
        tell_object(player, "Use 'manifest' to see your psionic powers.\n");
    }
    if (is_caster) {
        tell_object(player, "Use 'cast' to see your spells.\n");
    }
    
    tell_object(player, "\n");
    
    // Clean up creation data
    map_delete(player_creation, pname);
}

// Check if player is in character creation
int is_in_creation(object player) {
    string pname = player->query_name();
    return player_creation[pname] != 0;
}

// Cancel character creation
void cancel_creation(object player) {
    string pname = player->query_name();
    map_delete(player_creation, pname);
}
