#ifndef TERMINAL_UI_H
#define TERMINAL_UI_H

// Terminal UI module for styled build output
// Uses ANSI escape codes and UTF-8 box drawing characters

// Initialize terminal (clear screen, hide cursor)
void tui_init(void);

// Cleanup terminal (show cursor, reset colors)
void tui_cleanup(void);

// Draw a box with title at specified position
void tui_draw_box(int x, int y, int width, int height, const char* title);

// Draw a progress bar showing percentage completion
void tui_progress_bar(int x, int y, int width, float progress);

// Display status message at position
void tui_status(int x, int y, const char* message);

// Display warning box with multiple warning messages
void tui_warning_box(const char** warnings, int count);

// Display error box with multiple error messages
void tui_error_box(const char** errors, int count);

// Display final build summary
void tui_summary(int files, int warnings, int errors, float time_sec);

#endif // TERMINAL_UI_H
