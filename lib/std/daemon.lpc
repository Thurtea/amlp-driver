// /lib/std/daemon.c
// Base class for daemon objects

void create() {
    // Daemons don't destruct
    set_no_clean(1);
}

// Prevent daemon from being destructed
int clean_up(int force) {
    if (force) return 0;
    return 1;
}
