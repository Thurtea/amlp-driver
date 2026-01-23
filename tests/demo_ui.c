#include "terminal_ui.h"
#include <stdio.h>
#include <unistd.h>

// Need this for usleep on some systems
#define _DEFAULT_SOURCE

int main(void) {
    tui_init();
    
    // Draw title box
    tui_draw_box(5, 2, 60, 3, "Terminal UI Demo");
    
    // Simulate compilation progress
    for (int i = 0; i <= 10; i++) {
        float progress = i / 10.0f;
        tui_status(8, 6, "Compiling project files...");
        tui_progress_bar(8, 7, 50, progress);
        usleep(200000);  // 200ms delay
    }
    
    // Show warnings
    const char* warnings[] = {
        "Unused variable 'x' in function foo()",
        "Implicit conversion from int to float"
    };
    tui_warning_box(warnings, 2);
    
    // Show errors (empty for demo)
    tui_error_box(NULL, 0);
    
    // Show summary
    tui_summary(14, 2, 0, 1.23f);
    
    tui_cleanup();
    return 0;
}
