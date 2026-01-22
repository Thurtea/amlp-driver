// strings.c - String manipulation functions

int strlen_custom(string s) {
    return strlen(s);
}

string concatenate(string a, string b) {
    return a + b;
}

string uppercase(string s) {
    return upper_case(s);
}

string lowercase(string s) {
    return lower_case(s);
}

string *split_string(string s, string delim) {
    return explode(s, delim);
}

string join_array(string *arr, string sep) {
    return implode(arr, sep);
}

string trim_whitespace(string s) {
    return trim(s);
}

string substring(string s, int start, int len) {
    return s[start..start+len-1];
}

void test_strings() {
    string greeting;
    string *words;
    
    greeting = concatenate("Hello, ", "World!");
    printf("Concatenation: %s\n", greeting);
    printf("Uppercase: %s\n", uppercase(greeting));
    printf("Lowercase: %s\n", lowercase(greeting));
    printf("Length: %d\n", strlen_custom(greeting));
    
    words = split_string("one two three four", " ");
    printf("Joined: %s\n", join_array(words, "-"));
}
