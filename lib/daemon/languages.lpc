/*
 * /lib/daemon/languages.lpc - Language Management Daemon
 * 
 * Central registry for all languages in the game.
 * Handles language loading, lookups, and proficiency calculations.
 */

private mapping languages;        /* All loaded language objects */
private mapping language_data;    /* Cached language metadata */

void create() {
    languages = ([]);
    language_data = ([]);
    load_all_languages();
}

/*
 * Load all language definitions
 */
void load_all_languages() {
    string *lang_files = get_dir("/languages/*.lpc");
    
    foreach (string file : lang_files) {
        string lang_id = file[0..<5];  /* Remove .lpc */
        string path = "/languages/" + lang_id;
        
        object lang = load_object(path);
        if (lang) {
            languages[lang_id] = lang;
            language_data[lang_id] = ([
                "name": lang->query_language_name(),
                "family": lang->query_language_family(),
                "difficulty": lang->query_language_difficulty(),
                "region": lang->query_language_region(),
            ]);
        }
    }
}

/*
 * Get a specific language object
 */
object query_language(string lang_id) {
    if (!languages[lang_id]) {
        /* Try to load it */
        string path = "/languages/" + lang_id;
        object lang = load_object(path);
        if (lang) {
            languages[lang_id] = lang;
        }
    }
    return languages[lang_id];
}

/*
 * Get language by display name
 */
object query_language_by_name(string name) {
    foreach (string id, object lang : languages) {
        if (lower_case(lang->query_language_name()) == lower_case(name)) {
            return lang;
        }
    }
    return 0;
}

/*
 * Get all language IDs
 */
string *query_all_languages() {
    return keys(languages);
}

/*
 * Get languages by family
 */
string *query_languages_by_family(string family) {
    string *result = ({});
    
    foreach (string id, mapping data : language_data) {
        if (data["family"] == family) {
            result += ({ id });
        }
    }
    
    return result;
}

/*
 * Get language metadata
 */
mapping query_language_data(string lang_id) {
    return language_data[lang_id];
}

/*
 * Get display name for a language ID
 */
string query_language_name(string lang_id) {
    if (language_data[lang_id]) {
        return language_data[lang_id]["name"];
    }
    return capitalize(lang_id);
}

/*
 * Garble text based on listener's comprehension
 * Checks for Tongues spell which grants perfect understanding
 */
string garble_for_listener(object speaker, object listener, string lang_id, string text) {
    /* Check if listener has Tongues spell active */
    if (listener && listener->query_property("tongues_active")) {
        return text;  /* Perfect understanding */
    }
    
    /* Check if speaker has Tongues spell active */
    if (speaker && speaker->query_property("tongues_active")) {
        return text;  /* Understood by all */
    }
    
    object lang = query_language(lang_id);
    if (!lang) return text;
    
    int comprehension = lang->query_comprehension(listener);
    return lang->garble_text(text, comprehension);
}

/*
 * Get proficiency description
 */
string proficiency_description(int prof) {
    if (prof >= 98) return "native";
    if (prof >= 90) return "fluent";
    if (prof >= 70) return "conversational";
    if (prof >= 50) return "functional";
    if (prof >= 30) return "broken";
    if (prof >= 10) return "basic phrases";
    return "barely any";
}

/*
 * Calculate starting proficiency for a language based on race/OCC
 */
int calculate_starting_proficiency(string lang_id, object who) {
    object lang = query_language(lang_id);
    if (!lang) return 0;
    
    int base = 40;  /* Base starting proficiency */
    int iq_bonus = 0;
    
    /* IQ bonus */
    if (who) {
        int iq = who->query_stat("iq");
        if (iq > 12) {
            iq_bonus = (iq - 12) * 3;
        }
    }
    
    /* Difficulty modifier (easier languages start higher) */
    int diff_mod = (10 - lang->query_language_difficulty()) * 3;
    
    /* Learning bonus from related languages */
    int learn_bonus = 0;
    if (who) {
        learn_bonus = lang->query_learning_bonus(who);
    }
    
    int total = base + iq_bonus + diff_mod + learn_bonus;
    
    /* Clamp to 1-95 (can't start native unless racial) */
    if (total < 1) total = 1;
    if (total > 95) total = 95;
    
    return total;
}

/*
 * Reload all languages (for development)
 */
void reload_languages() {
    languages = ([]);
    language_data = ([]);
    load_all_languages();
}
