/*
 * /lib/std/skill.lpc - Base class for Palladium-style skills
 *
 * Skills have:
 * - Base percentage
 * - Bonus per level
 * - Stat requirements
 * - Skill category (weapon, physical, technical, etc.)
 */

inherit "/std/object";

/* ========================= Skill Properties ========================= */
private string skill_id;          /* Unique identifier */
private string skill_name;        /* Display name */
private string skill_desc;        /* Description */
private string skill_category;    /* Category: physical, weapon, technical, etc. */
private int base_percentage;      /* Starting percentage */
private int level_bonus;          /* Bonus per level */
private int max_percentage;       /* Maximum percentage (usually 98) */

/* Stat requirements - minimum stat values needed */
private mapping stat_requirements;

/* Related skills - knowing these gives bonuses */
private string *related_skills;   /* Prerequisite or synergy skills */
private int *related_bonuses;     /* Bonuses from related skills */

/* Learning properties */
private int training_time;        /* Weeks to learn */
private int is_primary_only;      /* Can only be taken as OCC skill */

void create() {
    ::create();
    
    skill_id = "base_skill";
    skill_name = "Base Skill";
    skill_desc = "A basic skill.";
    skill_category = "general";
    base_percentage = 30;
    level_bonus = 5;
    max_percentage = 98;
    
    stat_requirements = ([]);
    related_skills = ({});
    related_bonuses = ({});
    
    training_time = 4;
    is_primary_only = 0;
}

/* ========================= Accessors ========================= */
string query_skill_id() { return skill_id; }
void set_skill_id(string id) { skill_id = id; }

string query_skill_name() { return skill_name; }
void set_skill_name(string name) { skill_name = name; }

string query_skill_desc() { return skill_desc; }
void set_skill_desc(string desc) { skill_desc = desc; }

string query_skill_category() { return skill_category; }
void set_skill_category(string cat) { skill_category = cat; }

int query_base_percentage() { return base_percentage; }
void set_base_percentage(int pct) { base_percentage = pct; }

int query_level_bonus() { return level_bonus; }
void set_level_bonus(int bonus) { level_bonus = bonus; }

int query_max_percentage() { return max_percentage; }
void set_max_percentage(int max) { max_percentage = max; }

mapping query_stat_requirements() { return copy(stat_requirements); }
void set_stat_requirements(mapping reqs) { stat_requirements = copy(reqs); }

string *query_related_skills() { return related_skills; }
void set_related_skills(string *skills) { related_skills = skills; }

int *query_related_bonuses() { return related_bonuses; }
void set_related_bonuses(int *bonuses) { related_bonuses = bonuses; }

int query_training_time() { return training_time; }
void set_training_time(int weeks) { training_time = weeks; }

int query_is_primary_only() { return is_primary_only; }
void set_is_primary_only(int val) { is_primary_only = val; }

/* ========================= Skill Calculations ========================= */

/*
 * Calculate what percentage a living would have at a given level
 * taking into account IQ bonus and related skills
 */
int calculate_percentage(object living, int level) {
    int pct = base_percentage;
    
    /* Add per-level bonus */
    if (level > 1) {
        pct += level_bonus * (level - 1);
    }
    
    /* Add IQ bonus if applicable */
    if (living) {
        int iq = living->query_stat("iq");
        if (iq >= 16) {
            /* +1% per IQ point above 15 for technical/science skills */
            if (skill_category == "technical" || 
                skill_category == "science" ||
                skill_category == "medical" ||
                skill_category == "electrical") {
                pct += (iq - 15);
            }
        }
    }
    
    /* Add bonuses from related skills */
    if (living && sizeof(related_skills) > 0) {
        for (int i = 0; i < sizeof(related_skills); i++) {
            if (living->has_skill(related_skills[i])) {
                if (i < sizeof(related_bonuses)) {
                    pct += related_bonuses[i];
                } else {
                    pct += 5;  /* Default +5% if no bonus specified */
                }
            }
        }
    }
    
    /* Cap at max */
    if (pct > max_percentage) pct = max_percentage;
    
    return pct;
}

/*
 * Check if a living meets the stat requirements
 */
int meets_requirements(object living) {
    if (!living) return 0;
    if (sizeof(stat_requirements) == 0) return 1;
    
    string *stats = keys(stat_requirements);
    foreach (string stat : stats) {
        int required = stat_requirements[stat];
        int actual = living->query_stat(stat);
        if (actual < required) {
            return 0;
        }
    }
    
    return 1;
}

/*
 * Get description of why requirements aren't met
 */
string query_requirements_failure(object living) {
    if (!living) return "No character specified.";
    if (sizeof(stat_requirements) == 0) return "";
    
    string *failed = ({});
    string *stats = keys(stat_requirements);
    
    foreach (string stat : stats) {
        int required = stat_requirements[stat];
        int actual = living->query_stat(stat);
        if (actual < required) {
            failed += ({ upper_case(stat) + " " + required + 
                        " (you have " + actual + ")" });
        }
    }
    
    if (sizeof(failed) == 0) return "";
    return "Requires: " + implode(failed, ", ");
}

/*
 * Attempt a skill roll for a living at their current skill level
 */
int attempt(object living) {
    if (!living) return 0;
    
    int pct = living->query_skill(skill_id);
    if (pct <= 0) return 0;
    
    int roll = random(100) + 1;
    
    /* Critical failure on 96-100 */
    if (roll >= 96) return -1;
    
    /* Success if roll <= skill percentage */
    if (roll <= pct) return 1;
    
    return 0;
}
