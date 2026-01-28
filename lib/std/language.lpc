/*
 * /lib/std/language.lpc - Base Language Class
 * 
 * Palladium RIFTS-style language system.
 * Each language has a difficulty level and family for related languages.
 */

inherit "/std/object";

private string language_name;
private string language_id;        /* File identifier (e.g., "american") */
private string language_family;    /* Language family for bonuses */
private int language_difficulty;   /* 1-10, higher = harder to learn */
private string language_region;    /* Primary region where spoken */
private string *related_languages; /* Languages that grant learning bonus */

void create() {
    ::create();
    language_name = "Unknown";
    language_id = "unknown";
    language_family = "common";
    language_difficulty = 5;
    language_region = "Unknown";
    related_languages = ({});
}

/* ========================= Setters ========================= */

void set_language_name(string name) {
    language_name = name;
    set_name(name);
}

void set_language_id(string id) {
    language_id = id;
}

void set_language_family(string family) {
    language_family = family;
}

void set_language_difficulty(int diff) {
    if (diff < 1) diff = 1;
    if (diff > 10) diff = 10;
    language_difficulty = diff;
}

void set_language_region(string region) {
    language_region = region;
}

void set_related_languages(string *langs) {
    related_languages = langs;
}

void add_related_language(string lang) {
    if (member_array(lang, related_languages) == -1) {
        related_languages += ({ lang });
    }
}

/* ========================= Getters ========================= */

string query_language_name() { return language_name; }
string query_language_id() { return language_id; }
string query_language_family() { return language_family; }
int query_language_difficulty() { return language_difficulty; }
string query_language_region() { return language_region; }
string *query_related_languages() { return related_languages; }

/* ========================= Comprehension ========================= */

/*
 * Calculate comprehension level for a specific person.
 * Returns 0-100 representing percentage understanding.
 */
int query_comprehension(object who) {
    if (!who) return 0;
    
    /* Check if they have tongues spell active */
    if (who->query_property("tongues_active")) {
        return 100;
    }
    
    /* Check their proficiency in this language */
    return who->query_language_proficiency(language_id);
}

/*
 * Garble text based on comprehension level.
 * Higher comprehension = more readable text.
 */
string garble_text(string text, int comprehension) {
    if (comprehension >= 95) return text;
    if (comprehension <= 0) return garble_complete(text);
    
    string result = "";
    int len = strlen(text);
    
    for (int i = 0; i < len; i++) {
        string ch = text[i..i];
        
        /* Preserve spaces and punctuation */
        if (ch == " " || ch == "." || ch == "," || ch == "!" || 
            ch == "?" || ch == "'" || ch == "-") {
            result += ch;
            continue;
        }
        
        /* Roll to keep character based on comprehension */
        if (random(100) < comprehension) {
            result += ch;
        } else {
            /* Replace with similar-sounding character */
            result += garble_character(ch);
        }
    }
    
    return result;
}

/*
 * Completely garble text (0% comprehension)
 */
string garble_complete(string text) {
    string result = "";
    int len = strlen(text);
    
    for (int i = 0; i < len; i++) {
        string ch = text[i..i];
        
        /* Preserve spaces */
        if (ch == " ") {
            result += " ";
        } else if (ch == "." || ch == "!" || ch == "?") {
            result += ch;
        } else {
            result += "#";
        }
    }
    
    return result;
}

/*
 * Replace a character with a similar-sounding one
 */
string garble_character(string ch) {
    string vowels = "aeiou";
    string consonants = "bcdfghjklmnpqrstvwxyz";
    
    ch = lower_case(ch);
    
    /* Vowels replace with vowels */
    if (member_array(ch[0], explode(vowels, "")) != -1) {
        return vowels[random(5)..random(5)];
    }
    
    /* Consonants replace with consonants */
    if (member_array(ch[0], explode(consonants, "")) != -1) {
        return consonants[random(21)..random(21)];
    }
    
    return "x";
}

/*
 * Calculate bonus for learning this language based on known languages
 */
int query_learning_bonus(object who) {
    if (!who) return 0;
    
    int bonus = 0;
    
    /* Check if they know related languages */
    foreach (string related : related_languages) {
        int prof = who->query_language_proficiency(related);
        if (prof > 50) {
            /* Knowing a related language at 50%+ gives learning bonus */
            bonus += (prof - 50) / 5;
        }
    }
    
    /* Check same language family */
    string *known = who->query_known_languages();
    foreach (string lang_id : known) {
        object lang = load_language(lang_id);
        if (lang && lang->query_language_family() == language_family) {
            int prof = who->query_language_proficiency(lang_id);
            if (prof > 30) {
                bonus += (prof - 30) / 10;
            }
        }
    }
    
    /* Cap bonus at 30% */
    if (bonus > 30) bonus = 30;
    
    return bonus;
}

/*
 * Load a language object
 */
object load_language(string lang_id) {
    string path = "/languages/" + lang_id;
    return load_object(path);
}
