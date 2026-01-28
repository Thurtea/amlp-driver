/*
 * /lib/std/spell.lpc - Base class for magic spells
 *
 * Palladium magic spells:
 * - Use PPE (Potential Psychic Energy)
 * - Have spell levels (1-15)
 * - Some require components or incantations
 */

inherit "/std/object";

/* ========================= Spell Properties ========================= */
private string spell_id;          /* Unique identifier */
private string spell_name;        /* Display name */
private string spell_desc;        /* Description */
private int spell_level;          /* Spell level 1-15 */
private int ppe_cost;             /* PPE to cast */
private int duration_type;        /* 0=instant, 1=per minute, 2=per level */
private int base_duration;        /* Duration in relevant units */
private int range_type;           /* 0=self, 1=touch, 2=ranged */
private int base_range;           /* Range in feet (if ranged) */
private string incantation;       /* Words to speak (if any) */
private int requires_verbal;      /* Must speak incantation */
private int requires_somatic;     /* Requires hand gestures */
private string *components;       /* Required components */
private int casting_time;         /* In melee actions */

void create() {
    ::create();
    
    spell_id = "base_spell";
    spell_name = "Base Spell";
    spell_desc = "A basic magical spell.";
    spell_level = 1;
    ppe_cost = 5;
    duration_type = 0;
    base_duration = 0;
    range_type = 0;
    base_range = 0;
    incantation = "";
    requires_verbal = 1;
    requires_somatic = 1;
    components = ({});
    casting_time = 1;
}

/* ========================= Accessors ========================= */
string query_spell_id() { return spell_id; }
void set_spell_id(string id) { spell_id = id; }

string query_spell_name() { return spell_name; }
void set_spell_name(string name) { spell_name = name; }

string query_spell_desc() { return spell_desc; }
void set_spell_desc(string desc) { spell_desc = desc; }

int query_spell_level() { return spell_level; }
void set_spell_level(int level) { spell_level = level; }

int query_ppe_cost() { return ppe_cost; }
void set_ppe_cost(int cost) { ppe_cost = cost; }

int query_duration_type() { return duration_type; }
void set_duration_type(int type) { duration_type = type; }

int query_base_duration() { return base_duration; }
void set_base_duration(int dur) { base_duration = dur; }

int query_range_type() { return range_type; }
void set_range_type(int type) { range_type = type; }

int query_base_range() { return base_range; }
void set_base_range(int range) { base_range = range; }

string query_incantation() { return incantation; }
void set_incantation(string words) { incantation = words; }

int query_requires_verbal() { return requires_verbal; }
void set_requires_verbal(int val) { requires_verbal = val; }

int query_requires_somatic() { return requires_somatic; }
void set_requires_somatic(int val) { requires_somatic = val; }

string *query_components() { return components; }
void set_components(string *comps) { components = comps; }

int query_casting_time() { return casting_time; }
void set_casting_time(int time) { casting_time = time; }

/* ========================= Casting ========================= */

/*
 * Check if a caster can cast this spell
 * Returns: 1 = can cast, 0 = cannot, -1 = not enough PPE
 */
int can_cast(object caster) {
    if (!caster) return 0;
    
    /* Check if they know the spell */
    if (!caster->knows_spell(spell_id)) {
        return 0;
    }
    
    /* Check PPE */
    if (caster->query_ppe() < ppe_cost) {
        return -1;
    }
    
    /* Check spell caster level vs spell level */
    int caster_level = caster->query_spell_caster_level();
    if (caster_level < spell_level) {
        return 0;  /* Spell too high level */
    }
    
    return 1;
}

/*
 * Cast the spell
 * Override in subclasses for specific effects
 * Returns: 1 = success, 0 = failed
 */
int cast(object caster, object target) {
    if (!caster) return 0;
    
    int check = can_cast(caster);
    if (check <= 0) return 0;
    
    /* Deduct PPE */
    caster->use_ppe(ppe_cost);
    
    /* Speak incantation if required */
    if (requires_verbal && incantation != "") {
        /* The caster speaks in their primary language */
        string lang = caster->query_primary_language();
        /* Incantations are typically in a magical language */
        tell_object(caster, "You intone: \"" + incantation + "\"\n");
    }
    
    /* Apply effect */
    return apply_effect(caster, target);
}

/*
 * Override this in subclasses to apply the spell's effect
 */
int apply_effect(object caster, object target) {
    return 1;  /* Override me */
}

/*
 * Calculate duration based on caster level
 */
int calculate_duration(object caster) {
    int caster_level = caster->query_spell_caster_level();
    
    switch (duration_type) {
        case 0: return 0;  /* Instant */
        case 1: return base_duration;  /* Fixed minutes */
        case 2: return base_duration * caster_level;  /* Per level */
    }
    
    return base_duration;
}

/*
 * Get descriptive text for the spell
 */
string query_info() {
    string result = "";
    result += spell_name + " (Level " + spell_level + ")\n";
    result += "PPE Cost: " + ppe_cost + "\n";
    
    switch (range_type) {
        case 0: result += "Range: Self\n"; break;
        case 1: result += "Range: Touch\n"; break;
        case 2: result += "Range: " + base_range + " feet\n"; break;
    }
    
    switch (duration_type) {
        case 0: result += "Duration: Instant\n"; break;
        case 1: result += "Duration: " + base_duration + " minute(s)\n"; break;
        case 2: result += "Duration: " + base_duration + " minute(s) per level\n"; break;
    }
    
    if (requires_verbal) result += "Requires: Verbal component\n";
    if (requires_somatic) result += "Requires: Somatic component\n";
    if (sizeof(components) > 0) {
        result += "Components: " + implode(components, ", ") + "\n";
    }
    
    result += "\n" + spell_desc + "\n";
    
    return result;
}
