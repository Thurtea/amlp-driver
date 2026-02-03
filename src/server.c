/*
 * server.c - Filesystem Commands for AMLP MUD Driver
 * 
 * Provides ls, cd, pwd, cat commands for wizard/admin users
 */

#define _GNU_SOURCE  /* for strdup() */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>

#include "session_internal.h"  /* Provides PlayerSession struct definition */

#define BUFFER_SIZE 4096

/* External function to send text to player */
extern void send_to_player(PlayerSession *session, const char *format, ...);

/* ==========================================================================
 * Filesystem Commands for Wizards/Admins
 * ========================================================================== */

/* ls - List directory contents */
int cmd_ls_filesystem(PlayerSession *session, const char *args) {
    char path[512];
    char full_path[1024];
    
    /* Determine path */
    if (args && *args != '\0') {
        if (args[0] == '/') {
            snprintf(path, sizeof(path), "%s", args);
        } else {
            if (session->current_dir && strlen(session->current_dir) > 0) {
                snprintf(path, sizeof(path), "%s/%s", session->current_dir, args);
            } else {
                snprintf(path, sizeof(path), "/%s", args);
            }
        }
    } else {
        if (session->current_dir && strlen(session->current_dir) > 0) {
            snprintf(path, sizeof(path), "%s", session->current_dir);
        } else {
            snprintf(path, sizeof(path), "/");
        }
    }
    
    /* Build full filesystem path */
    snprintf(full_path, sizeof(full_path), "lib%s", path);
    
    DIR *dir = opendir(full_path);
    if (!dir) {
        send_to_player(session, "Cannot open directory: %s\r\n", path);
        return 0;
    }
    
    send_to_player(session, "Directory listing of %s:\r\n", path);
    
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        /* Skip hidden files */
        if (entry->d_name[0] == '.') continue;
        
        /* Check if directory or file */
        char item_path[1024];
        snprintf(item_path, sizeof(item_path), "%s/%s", full_path, entry->d_name);
        
        struct stat st;
        if (stat(item_path, &st) == 0) {
            char line[256];
            if (S_ISDIR(st.st_mode)) {
                snprintf(line, sizeof(line), "%-30s/\r\n", entry->d_name);
            } else {
                snprintf(line, sizeof(line), "%-30s\r\n", entry->d_name);
            }
            send_to_player(session, "%s", line);
        }
    }
    
    closedir(dir);
    return 1;
}

/* cd - Change directory */
int cmd_cd_filesystem(PlayerSession *session, const char *args) {
    char new_dir[512];
    char full_path[1024];
    
    /* Handle no argument - go to root */
    if (!args || *args == '\0') {
        if (session->current_dir) {
            free(session->current_dir);
        }
        session->current_dir = strdup("");
        send_to_player(session, "Changed to: /\r\n");
        return 1;
    }
    
    /* Handle absolute path */
    if (args[0] == '/') {
        snprintf(new_dir, sizeof(new_dir), "%s", args);
    }
    /* Handle .. */
    else if (strcmp(args, "..") == 0) {
        if (!session->current_dir || strlen(session->current_dir) == 0) {
            send_to_player(session, "Already at root directory\r\n");
            return 1;
        }
        
        /* Find last / and truncate */
        strncpy(new_dir, session->current_dir, sizeof(new_dir) - 1);
        char *last_slash = strrchr(new_dir, '/');
        if (last_slash) {
            *last_slash = '\0';
        } else {
            snprintf(new_dir, sizeof(new_dir), "");
        }
    }
    /* Handle relative path */
    else {
        if (session->current_dir && strlen(session->current_dir) > 0) {
            snprintf(new_dir, sizeof(new_dir), "%s/%s", session->current_dir, args);
        } else {
            snprintf(new_dir, sizeof(new_dir), "/%s", args);
        }
    }
    
    /* Verify directory exists */
    snprintf(full_path, sizeof(full_path), "lib%s", new_dir);
   
    struct stat st;
    if (stat(full_path, &st) != 0 || !S_ISDIR(st.st_mode)) {
        send_to_player(session, "Directory does not exist: %s\r\n", new_dir);
        return 0;
    }
    
    /* Update current directory */
    if (session->current_dir) {
        free(session->current_dir);
    }
    session->current_dir = strdup(new_dir);
    
    send_to_player(session, "Changed to: %s\r\n", new_dir);
    return 1;
}

/* pwd - Print working directory */
int cmd_pwd_filesystem(PlayerSession *session) {
    if (!session->current_dir || strlen(session->current_dir) == 0) {
        send_to_player(session, "Current directory: /\r\n");
    } else {
        send_to_player(session, "Current directory: %s\r\n", session->current_dir);
    }
    return 1;
}

/* cat - Display file contents */
int cmd_cat_filesystem(PlayerSession *session, const char *args) {
    if (!args || *args == '\0') {
        send_to_player(session, "Usage: cat <filename>\r\n");
        return 0;
    }
    
    char path[512];
    char full_path[1024];
    
    /* Build path */
    if (args[0] == '/') {
        snprintf(path, sizeof(path), "%s", args);
    } else {
        if (session->current_dir && strlen(session->current_dir) > 0) {
            snprintf(path, sizeof(path), "%s/%s", session->current_dir, args);
        } else {
            snprintf(path, sizeof(path), "/%s", args);
        }
    }
    
    snprintf(full_path, sizeof(full_path), "lib%s", path);
    
    FILE *f = fopen(full_path, "r");
    if (!f) {
        send_to_player(session, "Cannot open file: %s\r\n", path);
        return 0;
    }
    
    send_to_player(session, "File: %s\r\n", path);
    send_to_player(session, "----------------------------------------\r\n");
    
    char line[512];
    int line_count = 0;
    while (fgets(line, sizeof(line), f) && line_count < 1000) {
        /* Convert LF to CRLF for telnet */
        char *lf = strchr(line, '\n');
        if (lf) {
            *lf = '\0';
            send_to_player(session, "%s\r\n", line);
        } else {
            send_to_player(session, "%s", line);
        }
        line_count++;
    }
    
    if (line_count >= 1000) {
        send_to_player(session, "\r\n[File truncated at 1000 lines]\r\n");
    }
    
    fclose(f);
    return 1;
}
