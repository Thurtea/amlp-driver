# AMLP Library

Standard LPC library for the AMLP Driver, providing the foundation for MUD development with a complete inheritance hierarchy and domain structure.

## Overview

AMLP Library is a comprehensive LPC mudlib designed for the AMLP Driver. It provides standard objects, room templates, character creation systems, and organizational structures for building immersive multi-user environments.

**Key Features:**
- Complete inheritance hierarchy (object → item → living → player)
- Room and area management system
- Character creation with races and occupations
- Security and permission management
- Skills, spells, and psionic systems

## Quick Start

The library is automatically loaded by the AMLP Driver when using the default configuration. All files use the `.lpc` extension for proper syntax highlighting.

**Directory Structure:**
```
amlp-library/
├── master.lpc        # Master object (driver control)
├── std/              # Standard objects
│   ├── object.lpc    # Base object
│   ├── room.lpc      # Room base
│   ├── living.lpc    # Living entities
│   └── player.lpc    # Player object
├── domains/          # World areas
├── cmds/             # Player commands
├── daemon/           # System services
├── races/            # Character races
└── occs/             # Character occupations
```

## Standard Objects

### Base Inheritance

```
object.lpc          # Root of all objects
  ├── room.lpc      # Locations and areas
  ├── container.lpc # Storage objects
  └── living.lpc    # Living entities
        └── player.lpc  # Player characters
```

### Creating a Room

```lpc
// domains/myworld/rooms/start.lpc
inherit "/std/room";

void create() {
    ::create();
    set_short("The Starting Room");
    set_long("You are in a cozy starting room. Exits lead in all directions.");
    add_exit("north", "/domains/myworld/rooms/hall");
    add_exit("south", "/domains/myworld/rooms/garden");
}
```

### Creating an NPC

```lpc
// domains/myworld/npc/guard.lpc
inherit "/std/living";

void create() {
    ::create();
    set_name("guard");
    set_short("a vigilant guard");
    set_long("This guard watches the area carefully.");
    set_race("human");
    set_level(5);
}
```

## Character System

The library includes a comprehensive character creation system:

**Races:**
- 52+ races including human, elf, dwarf, dragon, and Rifts-specific races
- Each race has unique stat modifiers and abilities

**Occupations (OCCs):**
- 48+ character classes including warriors, mages, psychics, and technology users
- Palladium Rifts RPG inspired occupational character classes

## Commands

Player commands are located in `/cmds/` and are automatically loaded:
- **Movement:** go, look, exits
- **Communication:** say, tell, chat, whisper, shout
- **Inventory:** get, drop, give, inventory, equipment
- **Character:** score, stats, skills
- **Admin:** shutdown, goto, clone, stat (wizard only)

## Development

### Adding a New Domain

1. Create domain directory:
   ```bash
   mkdir -p domains/myworld/{rooms,npc,obj}
   ```

2. Create entry room:
   ```lpc
   // domains/myworld/rooms/entry.lpc
   inherit "/std/room";
   
   void create() {
       ::create();
       set_short("Domain Entry");
       set_long("The entrance to your domain.");
   }
   ```

3. Link from existing area

### LPC Syntax Highlighting

For proper IDE support, use the [AMLP LPC Extension](../lpc-extension) for VS Code:

```bash
cp -r ../lpc-extension ~/.vscode/extensions/
# Restart VS Code
```

## Master Object

The master object (`master.lpc`) controls driver initialization and security:
- User authentication
- Object compilation
- Security permissions
- Error handling

## Integration with AMLP Driver

This library is designed specifically for the AMLP Driver:
- Automatic loading on driver startup
- Real-time LPC compilation
- Object cloning and inheritance
- Built-in function support

**Configuration** (in driver's `config/runtime.conf`):
```conf
mudlib_dir = "../amlp-library"
master_file = "/master.lpc"
```

## Contributing

Contributions welcome for:
- New standard objects
- Additional domains and areas
- Character races and occupations
- Commands and features

## License

MIT License - See LICENSE for details

---

**Version:** 0.7.0 | **Status:** Active Development | **Driver:** AMLP Driver v0.7.0
