/*
 * /lib/std/psionic.lpc - Base class for psionic powers
 *
 * Palladium psionics categories:
 * - Physical: Mind over matter (telekinesis, healing)
 * - Sensitive: Detection and perception powers
 * - Healing: Restoration and recovery
 * - Super: Most powerful, rare abilities
 */

inherit "/std/object";

/* ========================= Power Properties ========================= */
private string power_id;          /* Unique identifier */
private string power_name;        /* Display name */
private string power_desc;        /* Description */
private string power_category;    /* physical, sensitive, healing, super */
private int isp_cost;             /* ISP to activate */
private int duration_type;        /* 0=instant, 1=per minute, 2=per melee */
private int base_duration;        /* Duration in relevant units */
private int range_type;           /* 0=self, 1=touch, 2=ranged */
private int base_range;           /* Range in feet (if ranged) */
private string *required_stats;   /* Stats to check */
private int *required_values;     /* Minimum values */

void create() {
    ::create();
    
    power_id = "base_power";
    power_name = "Base Power";
    power_desc = "A basic psionic power.";
    power_category = "sensitive";
    isp_cost = 2;
    duration_type = 0;
    base_duration = 0;
    range_type = 0;
    base_range = 0;
    required_stats = ({});
    required_values = ({});
}

/* ========================= Accessors ========================= */
string query_power_id() { return power_id; }
void set_power_id(string id) { power_id = id; }

string query_power_name() { return power_name; }
void set_power_name(string name) { power_name = name; }

string query_power_desc() { return power_desc; }
void set_power_desc(string desc) { power_desc = desc; }

string query_power_category() { return power_category; }
void set_power_category(string cat) { power_category = cat; }

int query_isp_cost() { return isp_cost; }
void set_isp_cost(int cost) { isp_cost = cost; }

int query_duration_type() { return duration_type; }
void set_duration_type(int type) { duration_type = type; }

int query_base_duration() { return base_duration; }
void set_base_duration(int dur) { base_duration = dur; }

int query_range_type() { return range_type; }
void set_range_type(int type) { range_type = type; }

int query_base_range() { return base_range; }
void set_base_range(int range) { base_range = range; }

/* ========================= Requirements ========================= */

void set_requirements(string *stats, int *values) {
    required_stats = stats;
    required_values = values;
}

int meets_requirements(object living) {
    if (!living) return 0;
    if (sizeof(required_stats) == 0) return 1;
    
    for (int i = 0; i < sizeof(required_stats); i++) {
        int actual = living->query_stat(required_stats[i]);
        if (actual < required_values[i]) {
            return 0;
        }
    }
    return 1;
}

/* ========================= Activation ========================= */

/*
 * Check if a living can use this power
 * Returns: 1 = can use, 0 = cannot, -1 = not enough ISP
 */
int can_activate(object living) {
    if (!living) return 0;
    
    /* Check if they know the power */
    if (!living->knows_power(power_id)) {
        return 0;
    }
    
    /* Check ISP */
    if (living->query_isp() < isp_cost) {
        return -1;
    }
    
    return 1;
}

/*
 * Attempt to activate the power
 * Override in subclasses for specific effects
 * Returns: 1 = success, 0 = failed
 */
int activate(object user, object target) {
    if (!user) return 0;
    
    int check = can_activate(user);
    if (check <= 0) return 0;
    
    /* Deduct ISP */
    user->use_isp(isp_cost);
    
    /* Apply effect - override in subclasses */
    return apply_effect(user, target);
}

/*
 * Override this in subclasses to apply the power's effect
 */
int apply_effect(object user, object target) {
    return 1;  /* Override me */
}

/*
 * Get descriptive text for the power
 */
string query_info() {
    string result = "";
    result += power_name + "\n";
    result += "Category: " + capitalize(power_category) + "\n";
    result += "ISP Cost: " + isp_cost + "\n";
    
    switch (range_type) {
        case 0: result += "Range: Self\n"; break;
        case 1: result += "Range: Touch\n"; break;
        case 2: result += "Range: " + base_range + " feet\n"; break;
    }
    
    switch (duration_type) {
        case 0: result += "Duration: Instant\n"; break;
        case 1: result += "Duration: " + base_duration + " minute(s)\n"; break;
        case 2: result += "Duration: " + base_duration + " melee round(s)\n"; break;
    }
    
    result += "\n" + power_desc + "\n";
    
    return result;
}
