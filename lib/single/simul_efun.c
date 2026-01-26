// AMLP Simulated Efuns
// Common utility functions available to all objects

// Get object ID number from file_name
varargs int getoid(object ob) {
    int id;
    if (!ob) ob = previous_object();
    sscanf(file_name(ob), "%*s#%d", id);
    return id;
}

// Read and display file contents
void cat(string file) {
    string content;
    content = read_file(file);
    if (content) {
        write(content);
    } else {
        write("File not found: " + file + "\n");
    }
}

// Convert path to absolute
string absolute_path(string path) {
    string current_dir;
    
    if (!path || path == "") return "/";
    if (path[0] == '/') return path;
    
    // Get current object's directory
    current_dir = dirname(file_name(this_object()));
    if (current_dir == "/") return "/" + path;
    return current_dir + "/" + path;
}

// Get directory name from path
string dirname(string path) {
    int pos;
    if (!path || path == "") return "/";
    pos = strsrch(path, "/", -1);
    if (pos <= 0) return "/";
    return path[0..pos-1];
}

// Get basename from path
string basename(string path) {
    int pos;
    if (!path || path == "") return "";
    pos = strsrch(path, "/", -1);
    if (pos < 0) return path;
    return path[pos+1..];
}

// Tell object a message (wrapper for convenience)
void tell(object ob, string msg) {
    if (ob && msg) {
        tell_object(ob, msg);
    }
}

// Simple capitalize first letter
string capitalize(string str) {
    if (!str || str == "") return "";
    return upper_case(str[0..0]) + str[1..];
}

// Pluralize a word (very simple version)
string pluralize(string str) {
    int len;
    if (!str || str == "") return "";
    len = strlen(str);
    if (len < 1) return str;
    
    // Simple rules
    if (str[len-1] == 's' || str[len-1] == 'x' || str[len-1] == 'z') {
        return str + "es";
    }
    if (str[len-1] == 'y' && len > 1) {
        return str[0..len-2] + "ies";
    }
    return str + "s";
}

// File exists check
int file_exists(string path) {
    return file_size(path) >= 0;
}
