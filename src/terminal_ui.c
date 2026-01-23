#include "terminal_ui.h"
#include <stdio.h>
#include <string.h>

// ANSI Color Codes
#define COLOR_RESET   "\033[0m"
#define COLOR_CYAN    "\033[36m"
#define COLOR_YELLOW  "\033[33m"
#define COLOR_RED     "\033[31m"
#define COLOR_GREEN   "\033[32m"
#define COLOR_BOLD    "\033[1m"
#define COLOR_DIM     "\033[2m"

// ANSI Control Codes
#define CURSOR_HIDE   "\033[?25l"
#define CURSOR_SHOW   "\033[?25h"
#define SCREEN_CLEAR  "\033[2J\033[H"
#define CURSOR_HOME   "\033[H"

// UTF-8 Box Drawing Characters (double-line style)
#define BOX_TL "╔"  // Top-left
#define BOX_TR "╗"  // Top-right
#define BOX_BL "╚"  // Bottom-left
#define BOX_BR "╝"  // Bottom-right
#define BOX_H  "═"  // Horizontal
#define BOX_V  "║"  // Vertical

// Progress bar characters
#define PROGRESS_FILLED "█"
#define PROGRESS_EMPTY  "░"

// Move cursor to position (1-indexed)
static void move_cursor(int x, int y) {
    printf("\033[%d;%dH", y, x);
}

void tui_init(void) {
    printf(SCREEN_CLEAR);
    printf(CURSOR_HIDE);
    fflush(stdout);
}

void tui_cleanup(void) {
    printf(CURSOR_SHOW);
    printf(COLOR_RESET);
    printf("\n");
    fflush(stdout);
}

void tui_draw_box(int x, int y, int width, int height, const char* title) {
    // Draw top border
    move_cursor(x, y);
    printf(COLOR_CYAN COLOR_BOLD);
    printf(BOX_TL);
    
    // Title in the top border
    if (title && strlen(title) > 0) {
        int title_len = strlen(title);
        int padding = (width - 2 - title_len - 2) / 2;  // -2 for corners, -2 for spaces around title
        
        for (int i = 0; i < padding; i++) {
            printf(BOX_H);
        }
        printf(" %s ", title);
        for (int i = 0; i < width - 2 - padding - title_len - 2; i++) {
            printf(BOX_H);
        }
    } else {
        for (int i = 0; i < width - 2; i++) {
            printf(BOX_H);
        }
    }
    printf(BOX_TR);
    
    // Draw sides
    for (int i = 1; i < height - 1; i++) {
        move_cursor(x, y + i);
        printf(BOX_V);
        move_cursor(x + width - 1, y + i);
        printf(BOX_V);
    }
    
    // Draw bottom border
    move_cursor(x, y + height - 1);
    printf(BOX_BL);
    for (int i = 0; i < width - 2; i++) {
        printf(BOX_H);
    }
    printf(BOX_BR);
    printf(COLOR_RESET);
    
    fflush(stdout);
}

void tui_progress_bar(int x, int y, int width, float progress) {
    // Clamp progress to 0.0-1.0
    if (progress < 0.0f) progress = 0.0f;
    if (progress > 1.0f) progress = 1.0f;
    
    int filled_width = (int)(progress * width);
    
    move_cursor(x, y);
    printf(COLOR_CYAN);
    
    // Draw filled portion
    for (int i = 0; i < filled_width; i++) {
        printf(PROGRESS_FILLED);
    }
    
    // Draw empty portion
    printf(COLOR_DIM);
    for (int i = filled_width; i < width; i++) {
        printf(PROGRESS_EMPTY);
    }
    
    printf(COLOR_RESET);
    
    // Show percentage
    printf(" %3.0f%%", progress * 100);
    
    fflush(stdout);
}

void tui_status(int x, int y, const char* message) {
    move_cursor(x, y);
    printf(COLOR_CYAN "%s" COLOR_RESET, message);
    // Clear to end of line to overwrite previous text
    printf("\033[K");
    fflush(stdout);
}

void tui_warning_box(const char** warnings, int count) {
    if (count == 0) return;
    
    printf("\n");
    printf(COLOR_YELLOW COLOR_BOLD);
    printf("╔════════════════════════════════════════════════════════════════╗\n");
    printf("║ ⚠  WARNINGS (%d)%*s║\n", count, 48 - (count >= 10 ? 2 : 1), "");
    printf("╚════════════════════════════════════════════════════════════════╝\n");
    printf(COLOR_RESET);
    
    for (int i = 0; i < count; i++) {
        printf(COLOR_YELLOW "  • %s\n" COLOR_RESET, warnings[i]);
    }
    printf("\n");
    fflush(stdout);
}

void tui_error_box(const char** errors, int count) {
    if (count == 0) return;
    
    printf("\n");
    printf(COLOR_RED COLOR_BOLD);
    printf("╔════════════════════════════════════════════════════════════════╗\n");
    printf("║ ✗  ERRORS (%d)%*s║\n", count, 50 - (count >= 10 ? 2 : 1), "");
    printf("╚════════════════════════════════════════════════════════════════╝\n");
    printf(COLOR_RESET);
    
    for (int i = 0; i < count; i++) {
        printf(COLOR_RED "  • %s\n" COLOR_RESET, errors[i]);
    }
    printf("\n");
    fflush(stdout);
}

void tui_summary(int files, int warnings, int errors, float time_sec) {
    printf("\n");
    
    if (errors == 0) {
        printf(COLOR_GREEN COLOR_BOLD);
        printf("╔════════════════════════════════════════════════════════════════╗\n");
        printf("║ ✓  BUILD SUCCESSFUL%*s║\n", 44, "");
        printf("╚════════════════════════════════════════════════════════════════╝\n");
        printf(COLOR_RESET);
    } else {
        printf(COLOR_RED COLOR_BOLD);
        printf("╔════════════════════════════════════════════════════════════════╗\n");
        printf("║ ✗  BUILD FAILED%*s║\n", 48, "");
        printf("╚════════════════════════════════════════════════════════════════╝\n");
        printf(COLOR_RESET);
    }
    
    printf("\n");
    printf(COLOR_CYAN "  Files compiled: " COLOR_RESET "%d\n", files);
    printf(COLOR_YELLOW "  Warnings:       " COLOR_RESET "%d\n", warnings);
    
    if (errors > 0) {
        printf(COLOR_RED "  Errors:         %d\n" COLOR_RESET, errors);
    } else {
        printf(COLOR_GREEN "  Errors:         %d\n" COLOR_RESET, errors);
    }
    
    printf(COLOR_CYAN "  Time:           " COLOR_RESET "%.2fs\n", time_sec);
    printf("\n");
    
    fflush(stdout);
}
