// /lib/include/globals.h
// Global definitions for AMLP MUD

#ifndef __GLOBALS_H__
#define __GLOBALS_H__

// === DAEMON SHORTCUTS ===
#define COMMAND_D       "/daemon/command"
#define LOGIN_D         "/daemon/login"
#define CHARGEN_D       "/daemon/chargen"
#define SKILLS_D        "/daemon/skills"
#define LANGUAGES_D     "/daemon/languages"
#define PSIONICS_D      "/daemon/psionics"
#define SPELLS_D        "/daemon/spells"

// === STANDARD PATHS ===
#define DAEMON          "/std/daemon"
#define ROOM            "/std/room"
#define OBJECT          "/std/object"
#define LIVING          "/std/living"
#define PLAYER          "/std/player"
#define WIZTOOL         "/std/wiztool"

// === PRIVILEGE LEVELS ===
#define PRIV_PLAYER     0
#define PRIV_WIZARD     1
#define PRIV_ADMIN      2
#define PRIV_ROOT       3

// === COLOR CODES (ANSI) ===
#define ANSI_RESET      "\x1b[0m"
#define ANSI_BOLD       "\x1b[1m"
#define ANSI_DIM        "\x1b[2m"
#define ANSI_UNDERLINE  "\x1b[4m"

#define ANSI_BLACK      "\x1b[30m"
#define ANSI_RED        "\x1b[31m"
#define ANSI_GREEN      "\x1b[32m"
#define ANSI_YELLOW     "\x1b[33m"
#define ANSI_BLUE       "\x1b[34m"
#define ANSI_MAGENTA    "\x1b[35m"
#define ANSI_CYAN       "\x1b[36m"
#define ANSI_WHITE      "\x1b[37m"

// Color shortcuts (MUD style)
// Note: original tokens like %^RESET%^ are not valid C preprocessor identifiers
// and break the LPC parser used during compilation. Provide safer aliases
// if code needs them, or comment out the invalid forms.
/*
#define %^RESET%^       ANSI_RESET
#define %^BOLD%^        ANSI_BOLD
#define %^RED%^         ANSI_RED
#define %^GREEN%^       ANSI_GREEN
#define %^YELLOW%^      ANSI_YELLOW
#define %^BLUE%^        ANSI_BLUE
#define %^MAGENTA%^     ANSI_MAGENTA
#define %^CYAN%^        ANSI_CYAN
#define %^WHITE%^       ANSI_WHITE
*/

/* Safer symbolic aliases (use these in LPC where needed) */
#define MUD_CLR_RESET   "%^RESET%^"
#define MUD_CLR_BOLD    "%^BOLD%^"
#define MUD_CLR_RED     "%^RED%^"
#define MUD_CLR_GREEN   "%^GREEN%^"
#define MUD_CLR_YELLOW  "%^YELLOW%^"
#define MUD_CLR_BLUE    "%^BLUE%^"
#define MUD_CLR_MAGENTA "%^MAGENTA%^"
#define MUD_CLR_CYAN    "%^CYAN%^"
#define MUD_CLR_WHITE   "%^WHITE%^"

// === RETURN CODES ===
#define SUCCESS         1
#define FAILURE         0

// === OBJECT TYPES ===
#define OBJ_ROOM        "room"
#define OBJ_LIVING      "living"
#define OBJ_ITEM        "item"
#define OBJ_WEAPON      "weapon"
#define OBJ_ARMOR       "armor"
#define OBJ_CONTAINER   "container"

// === DIRECTORY STRUCTURE ===
#define DIR_SAVE        "/save"
#define DIR_PLAYERS     "/save/players"
#define DIR_LOG         "/log"
#define DIR_DATA        "/data"

#endif // __GLOBALS_H__
