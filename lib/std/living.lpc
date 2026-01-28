/*
 * /lib/std/living.lpc - Base class for all living creatures
 * 
 * Provides foundation for:
 * - HP/Health management
 * - Language system (speaking, understanding, proficiency)
 * - Introduction system (name visibility)
 * - Palladium stats (IQ, ME, MA, PS, PP, PE, PB, SPD)
 * - Skills, Psionics, and Magic point pools
 */

inherit "/std/object";

/* ========================= Core State ========================= */
private int hp;
private int max_hp;
private int is_alive;

/* ========================= Palladium Stats ========================= */
private mapping stats;  /* IQ, ME, MA, PS, PP, PE, PB, SPD */
private int sdc;        /* Structural Damage Capacity */
private int max_sdc;
private int mdc;        /* Mega Damage Capacity (for MDC beings) */
private int max_mdc;

/* ========================= Race/OCC Info ========================= */
private string race_id;
private string occ_id;
private string race_name;
private string race_article;  /* "a" or "an" */

/* ========================= Language System ========================= */
private mapping languages;       /* lang_id: proficiency (0-100) */
private string primary_language; /* Default speaking language */

/* ========================= Introduction System ========================= */
private mapping introduced_to;   /* object_path: 1 if introduced */
private string real_name;        /* Actual character name */

/* ========================= Skills System ========================= */
private mapping skills;          /* skill_id: percentage (0-100) */
private string *primary_skills;  /* OCC skills */
private string *secondary_skills;/* Secondary skills */

/* ========================= Psionics System ========================= */
private int isp;                 /* Inner Strength Points */
private int max_isp;
private string *psionic_powers;  /* Array of power IDs */
private string psionic_category; /* "none", "minor", "major", "master" */

/* ========================= Magic System ========================= */
private int ppe;                 /* Potential Psychic Energy */
private int max_ppe;
private mapping spells_known;    /* spell_id: level learned */
private int spell_caster_level;

/* ========================= Properties ========================= */
private mapping properties;      /* Generic property storage */

/* ========================= Initialization ========================= */
void create() {
    ::create();
    
    /* Basic state */
    hp = 100;
    max_hp = 100;
    is_alive = 1;
    sdc = 0;
    max_sdc = 0;
    mdc = 0;
    max_mdc = 0;
    
    /* Initialize Palladium stats */
    stats = ([
        "iq": 10,   /* Intelligence Quotient */
        "me": 10,   /* Mental Endurance */
        "ma": 10,   /* Mental Affinity */
        "ps": 10,   /* Physical Strength */
        "pp": 10,   /* Physical Prowess */
        "pe": 10,   /* Physical Endurance */
        "pb": 10,   /* Physical Beauty */
        "spd": 10,  /* Speed */
    ]);
    
    /* Race/OCC */
    race_id = "human";
    occ_id = "";
    race_name = "Human";
    race_article = "a";
    
    /* Languages - everyone starts with American */
    languages = ([ "american": 100 ]);
    primary_language = "american";
    
    /* Introductions */
    introduced_to = ([]);
    real_name = "";
    
    /* Skills */
    skills = ([]);
    primary_skills = ({});
    secondary_skills = ({});
    
    /* Psionics */
    isp = 0;
    max_isp = 0;
    psionic_powers = ({});
    psionic_category = "none";
    
    /* Magic */
    ppe = 0;
    max_ppe = 0;
    spells_known = ([]);
    spell_caster_level = 0;
    
    /* Properties */
    properties = ([]);
}

/* ========================= HP/Health Management ========================= */
int query_hp() { return hp; }
int query_max_hp() { return max_hp; }
void set_hp(int new_hp) { hp = new_hp; }
void set_max_hp(int new_max) { max_hp = new_max; }

int query_sdc() { return sdc; }
int query_max_sdc() { return max_sdc; }
void set_sdc(int new_sdc) { sdc = new_sdc; }
void set_max_sdc(int new_max) { max_sdc = new_max; }

int query_mdc() { return mdc; }
int query_max_mdc() { return max_mdc; }
void set_mdc(int new_mdc) { mdc = new_mdc; }
void set_max_mdc(int new_max) { max_mdc = new_max; }

void heal(int amount) {
    hp += amount;
    if (hp > max_hp) hp = max_hp;
}

void damage(int amount) {
    hp -= amount;
    if (hp <= 0) {
        hp = 0;
        die();
    }
}

void die() {
    is_alive = 0;
    /* Override in subclasses for death handling */
}

int is_living() { return is_alive; }

/* ========================= Palladium Stats ========================= */
int query_stat(string stat_name) {
    stat_name = lower_case(stat_name);
    if (stats[stat_name]) {
        return stats[stat_name];
    }
    return 0;
}

void set_stat(string stat_name, int value) {
    stat_name = lower_case(stat_name);
    stats[stat_name] = value;
}

void modify_stat(string stat_name, int amount) {
    stat_name = lower_case(stat_name);
    if (!stats[stat_name]) stats[stat_name] = 10;
    stats[stat_name] += amount;
}

mapping query_all_stats() {
    return copy(stats);
}

void set_all_stats(mapping new_stats) {
    stats = copy(new_stats);
}

/* Calculate stat bonus (Palladium style) */
int query_stat_bonus(string stat_name) {
    int val = query_stat(stat_name);
    if (val < 16) return 0;
    if (val <= 17) return 1;
    if (val <= 20) return 2;
    if (val <= 23) return 3;
    if (val <= 25) return 4;
    if (val <= 28) return 5;
    return 6;
}

/* ========================= Race/OCC System ========================= */
void set_race_id(string id) { race_id = id; }
string query_race_id() { return race_id; }

void set_occ_id(string id) { occ_id = id; }
string query_occ_id() { return occ_id; }

void set_race_name(string name) { race_name = name; }
string query_race_name() { return race_name; }

void set_race_article(string art) { race_article = art; }
string query_race_article() { return race_article; }

/* Get race description for introductions */
string query_race_description() {
    return race_article + " " + lower_case(race_name);
}

/* ========================= Language System ========================= */

void set_languages(mapping langs) {
    languages = copy(langs);
}

mapping query_languages() {
    return copy(languages);
}

void add_language(string lang_id, int proficiency) {
    if (proficiency < 0) proficiency = 0;
    if (proficiency > 100) proficiency = 100;
    languages[lang_id] = proficiency;
}

void remove_language(string lang_id) {
    map_delete(languages, lang_id);
    if (primary_language == lang_id) {
        primary_language = "american";
    }
}

int query_language_proficiency(string lang_id) {
    if (languages[lang_id]) {
        return languages[lang_id];
    }
    return 0;
}

int speaks_language(string lang_id) {
    return (languages[lang_id] && languages[lang_id] > 0);
}

void set_primary_language(string lang_id) {
    if (speaks_language(lang_id)) {
        primary_language = lang_id;
    }
}

string query_primary_language() {
    return primary_language;
}

string *query_known_languages() {
    return keys(languages);
}

void improve_language(string lang_id, int amount) {
    if (!languages[lang_id]) {
        languages[lang_id] = amount;
    } else {
        languages[lang_id] += amount;
    }
    if (languages[lang_id] > 100) languages[lang_id] = 100;
}

/* ========================= Introduction System ========================= */

void set_real_name(string name) {
    real_name = name;
}

string query_real_name() {
    return real_name;
}

/*
 * Check if a viewer knows this living's name
 */
int query_introduced(object viewer) {
    if (!viewer) return 0;
    
    string viewer_path = object_name(viewer);
    return (introduced_to[viewer_path] == 1);
}

/*
 * Introduce this living to a viewer
 */
void introduce_to(object viewer) {
    if (!viewer) return;
    
    string viewer_path = object_name(viewer);
    introduced_to[viewer_path] = 1;
}

/*
 * Remove introduction (forget)
 */
void forget_introduction(object viewer) {
    if (!viewer) return;
    
    string viewer_path = object_name(viewer);
    map_delete(introduced_to, viewer_path);
}

/*
 * Get the name to display to a specific viewer.
 * Returns real name if introduced, race description if not.
 */
string query_introduction_name(object viewer) {
    if (!viewer) return query_race_description();
    
    /* Check if viewer has been introduced */
    if (query_introduced(viewer)) {
        return real_name ? real_name : query_name();
    }
    
    /* Return race description */
    return query_race_description();
}

/*
 * Introduce self to everyone in current room
 */
void introduce_to_room() {
    object env = environment();
    if (!env) return;
    
    object *contents = all_inventory(env);
    foreach (object ob : contents) {
        if (ob != this_object() && living(ob)) {
            introduce_to(ob);
        }
    }
}

/* ========================= Skills System ========================= */

void set_skills(mapping new_skills) {
    skills = copy(new_skills);
}

mapping query_all_skills() {
    return copy(skills);
}

void add_skill(string skill_id, int percentage) {
    if (percentage < 0) percentage = 0;
    if (percentage > 98) percentage = 98;  /* Max skill is 98% */
    skills[skill_id] = percentage;
}

int query_skill(string skill_id) {
    if (skills[skill_id]) {
        return skills[skill_id];
    }
    return 0;
}

int has_skill(string skill_id) {
    return (skills[skill_id] && skills[skill_id] > 0);
}

void improve_skill(string skill_id, int amount) {
    if (!skills[skill_id]) {
        skills[skill_id] = amount;
    } else {
        skills[skill_id] += amount;
    }
    if (skills[skill_id] > 98) skills[skill_id] = 98;
}

/*
 * Attempt a skill roll
 * Returns 1 on success, 0 on failure
 * Returns -1 on critical failure (roll of 96-100)
 */
int attempt_skill(string skill_id) {
    int skill_pct = query_skill(skill_id);
    if (skill_pct <= 0) return 0;
    
    int roll = random(100) + 1;
    
    /* Critical failure */
    if (roll >= 96) return -1;
    
    /* Success if roll is under skill percentage */
    if (roll <= skill_pct) return 1;
    
    return 0;
}

void set_primary_skills(string *skls) {
    primary_skills = skls;
}

string *query_primary_skills() {
    return primary_skills;
}

void set_secondary_skills(string *skls) {
    secondary_skills = skls;
}

string *query_secondary_skills() {
    return secondary_skills;
}

/* ========================= Psionics System ========================= */

int query_isp() { return isp; }
int query_max_isp() { return max_isp; }

void set_isp(int amount) {
    isp = amount;
    if (isp < 0) isp = 0;
    if (isp > max_isp) isp = max_isp;
}

void set_max_isp(int amount) {
    max_isp = amount;
}

void add_isp(int amount) {
    isp += amount;
    if (isp > max_isp) isp = max_isp;
}

int use_isp(int amount) {
    if (isp < amount) return 0;  /* Not enough ISP */
    isp -= amount;
    return 1;  /* Success */
}

void set_psionic_powers(string *powers) {
    psionic_powers = powers;
}

string *query_psionic_powers() {
    return psionic_powers;
}

void add_psionic_power(string power_id) {
    if (member_array(power_id, psionic_powers) == -1) {
        psionic_powers += ({ power_id });
    }
}

int knows_power(string power_id) {
    return (member_array(power_id, psionic_powers) != -1);
}

void set_psionic_category(string cat) {
    psionic_category = cat;
}

string query_psionic_category() {
    return psionic_category;
}

int is_psionic() {
    return (psionic_category != "none" && sizeof(psionic_powers) > 0);
}

/* ========================= Magic System ========================= */

int query_ppe() { return ppe; }
int query_max_ppe() { return max_ppe; }

void set_ppe(int amount) {
    ppe = amount;
    if (ppe < 0) ppe = 0;
    if (ppe > max_ppe) ppe = max_ppe;
}

void set_max_ppe(int amount) {
    max_ppe = amount;
}

void add_ppe(int amount) {
    ppe += amount;
    if (ppe > max_ppe) ppe = max_ppe;
}

int use_ppe(int amount) {
    if (ppe < amount) return 0;  /* Not enough PPE */
    ppe -= amount;
    return 1;  /* Success */
}

void set_spells(mapping new_spells) {
    spells_known = copy(new_spells);
}

mapping query_spells() {
    return copy(spells_known);
}

void add_spell(string spell_id, int level) {
    spells_known[spell_id] = level;
}

int knows_spell(string spell_id) {
    return (spells_known[spell_id] != 0);
}

int query_spell_level(string spell_id) {
    if (spells_known[spell_id]) {
        return spells_known[spell_id];
    }
    return 0;
}

void set_spell_caster_level(int level) {
    spell_caster_level = level;
}

int query_spell_caster_level() {
    return spell_caster_level;
}

int is_spell_caster() {
    return (spell_caster_level > 0 && sizeof(spells_known) > 0);
}

/* ========================= Properties System ========================= */

void set_property(string key, mixed value) {
    properties[key] = value;
}

mixed query_property(string key) {
    return properties[key];
}

void remove_property(string key) {
    map_delete(properties, key);
}

int has_property(string key) {
    return (properties[key] != 0);
}

/* Remove tongues effect (called by call_out) */
void remove_tongues() {
    remove_property("tongues_active");
    tell_object(this_object(), "The tongues spell wears off.\n");
}
