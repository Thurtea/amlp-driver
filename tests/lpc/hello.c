// hello.c - Simple LPC program demonstrating basic output

void hello() {
    write("Hello, World!\n");
}

void greet(string name) {
    write("Greetings, " + name + "!\n");
}

int main() {
    hello();
    greet("LPC");
    return 0;
}
