// AMLP-MUD Global Definitions

#ifndef __GLOBALS_H__
#define __GLOBALS_H__

// Directory paths
#define DIR_STD     "/std"
#define DIR_CMDS    "/cmds"
#define DIR_CLONE   "/clone"
#define DIR_SINGLE  "/single"
#define DIR_DAEMON  "/daemon"
#define DIR_DOMAINS "/domains"
#define DIR_SECURE  "/secure"
#define DIR_SAVE    "/save"
#define DIR_LOG     "/log"
#define DIR_DATA    "/data"
#define DIR_ADMINCMDS "/cmds/admin"

// Core objects
#define LOGIN_OB   "/clone/login"
#define USER_OB    "/clone/user"
#define VOID_OB    "/domains/start/room/void"
#define START_ROOM "/domains/start/room/welcome"

// Security UIDs
#define ROOT_UID     "Root"
#define BACKBONE_UID "Backbone"

// Simul efuns
#define SIMUL_EFUN_FILE "/single/simul_efun"

// Command prefix
#define CMD_PREFIX "/cmds/"
#define ADMIN_CMD_PREFIX "/cmds/admin/"

// Message classes
#define M_STATUS "status"
#define M_SAY    "say"
#define M_SHOUT  "shout"

// Wizard levels
#define PLAYER_LEVEL 0
#define WIZARD_LEVEL 1
#define ADMIN_LEVEL  2

// Default values
#define DEFAULT_RACE "human"
#define DEFAULT_LANGUAGE "american"
#define DEFAULT_FLUENCY 98

#endif
