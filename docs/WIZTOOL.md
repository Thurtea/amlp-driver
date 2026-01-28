# Wiztool - In-Game Building and Administration Tool

The wiztool is a powerful in-game object that allows administrators and wizards to create, modify, and manage game content without using external editors. This is the classic MUD building approach where all world creation happens from within the game.

## Loading the Wiztool

First-time setup:

```
> wiztool
You load your wiztool. Type 'wiz help' for commands.
```

The wiztool is now loaded into your inventory and ready to use.

## Basic Usage

Once loaded, use the `wiz` command followed by a wiztool command:

```
wiz <command> [arguments]
```

Example:
```
> wiz help
> wiz clone /std/object
> wiz cd /domains/start/room
```

## Available Commands

### Object Management

**clone <path>**
Clone an object from a file and create a new instance.

```
> wiz clone /std/object
Cloning object: /std/object.lpc
Object created: /std/object#123

> wiz clone sword
Cloning object: /domains/start/room/sword.lpc
Object created: sword#456
```

**load <path>**
Alias for clone. Loads an object from a file.

**dest <object>**
Destruct (destroy) an object.

```
> wiz dest sword
Destructing object: sword

> wiz dest /std/object#123
Destructing object: /std/object#123
```

**update <object>**
Reload an object from disk, applying any code changes.

```
> wiz update /std/room
Updating object: /std/room
Object reloaded from disk.
```

**objstat <object>**
View detailed statistics about an object.

```
> wiz objstat me
Object Statistics for: me
----------------------------------------
File: /std/player.lpc
ID: player#789
Type: Player Object
Properties: [list properties]
```

**objects**
List all currently loaded objects in memory.

```
> wiz objects
Loaded Objects:
----------------------------------------
  /std/object#123
  /std/room#456
  /std/player#789
```

### Code Execution

**eval <code>**
Evaluate LPC code expressions directly.

```
> wiz eval 1 + 1
Result: 2

> wiz eval query_name()
Result: "admin"

> wiz eval sqrt(16)
Result: 4
```

**code <object> <code>**
Execute LPC code on a specific object.

```
> wiz code me set_short("the almighty admin")
Executing code on object

> wiz code sword set_value(100)
Executing code on object
```

### File Management

**cd <directory>**
Change the current working directory.

```
> wiz cd /domains/start/room
Changed directory to: /domains/start/room

> wiz cd ..
Changed to: /domains/start

> wiz cd /
Changed to root directory: /
```

**pwd**
Print the current working directory.

```
> wiz pwd
Current directory: /domains/start/room
```

**ls [path]**
List contents of a directory.

```
> wiz ls
Directory listing for: /domains/start/room
  void.lpc
  welcome.lpc
  documentation.lpc

> wiz ls /std
Directory listing for: /std
  object.lpc
  room.lpc
  living.lpc
  player.lpc
```

**cat <file>**
Display the contents of a file.

```
> wiz cat void.lpc
Contents of: /domains/start/room/void.lpc
----------------------------------------
inherit "/std/room";

void create() {
    ::create();
    set_short("The Void");
    set_long("An endless dark void.");
}
----------------------------------------
```

**mkdir <directory>**
Create a new directory.

```
> wiz mkdir /domains/myworld
Creating directory: /domains/myworld

> wiz mkdir rooms
Creating directory: /domains/start/rooms
```

**touch <file>**
Create a new empty file.

```
> wiz touch newroom.lpc
Creating file: /domains/start/room/newroom.lpc

> wiz touch /domains/myworld/entrance.lpc
Creating file: /domains/myworld/entrance.lpc
```

**ed <file>**
Edit a file (currently opens external editor or shows path).

```
> wiz ed /std/room.lpc
Opening editor for: /std/room.lpc
Use an external editor to modify: lib/std/room.lpc
```

### Navigation

**goto <path>**
Teleport to a room.

```
> wiz goto /domains/start/room/void
Teleporting to: /domains/start/room/void
You arrive in the Void.

> wiz goto welcome.lpc
Teleporting to: /domains/start/room/welcome.lpc
```

## Workflow Examples

### Creating a New Room

1. Navigate to your domain:
```
> wiz cd /domains/myworld
```

2. Create the room directory:
```
> wiz mkdir rooms
```

3. Create a new room file:
```
> wiz touch rooms/entrance.lpc
```

4. Edit the file externally (for now):
```
Edit: lib/domains/myworld/rooms/entrance.lpc
```

5. Load and test the room:
```
> wiz goto /domains/myworld/rooms/entrance
```

### Cloning Objects

1. Clone a standard object:
```
> wiz clone /std/object
```

2. Modify it with code:
```
> wiz code object#123 set_id("sword")
> wiz code object#123 set_short("a sharp sword")
> wiz code object#123 set_long("This is a finely crafted sword.")
```

3. Move it to your location or inventory:
```
> wiz code object#123 move(me)
```

### Working with Relative Paths

The wiztool tracks your current directory:

```
> wiz cd /domains/start/room
> wiz clone void.lpc              (clones /domains/start/room/void.lpc)
> wiz cat welcome.lpc              (shows /domains/start/room/welcome.lpc)
> wiz goto documentation.lpc       (goes to /domains/start/room/documentation.lpc)
```

### Evaluating Complex Code

```
> wiz eval sizeof(query_inventory(environment(me)))
Result: 5

> wiz eval explode("/domains/start/room/void", "/")
Result: ({ "domains", "start", "room", "void" })
```

## Privilege Requirements

- **PLAYER_LEVEL (0)**: Cannot use wiztool
- **WIZARD_LEVEL (1)**: Full wiztool access for building
- **ADMIN_LEVEL (2)**: Full wiztool access for building

To grant wizard privileges:
```
> promote <username> 1
```

## Tips and Best Practices

1. **Work in your own domain**: Create `/domains/yourname/` for your building projects

2. **Use relative paths**: Navigate with `cd` and use relative filenames to save typing

3. **Test frequently**: Clone and test objects as you build them

4. **Update objects**: Use `wiz update` after modifying code to see changes

5. **Check your work**: Use `wiz objstat` and `wiz objects` to inspect loaded objects

6. **Keep backups**: The wiztool operates on live files, so keep external backups

## Limitations in Current Implementation

The wiztool provides the interface and command structure. Some features require driver support:

- **eval**: Needs runtime code compilation
- **ed**: Needs in-game line editor
- **File operations**: Need driver file I/O support
- **Object operations**: Need full object system integration

These are marked with `[not yet fully implemented]` messages. The infrastructure is in place for future enhancement.

## Troubleshooting

**"You need to load your wiztool first"**
- Type `wiztool` to load it into your inventory

**"You don't have permission to use the wiztool"**
- Ask an admin to promote you: `promote yourname 1`

**"Unknown wiztool command"**
- Type `wiz help` to see all available commands
- Check command spelling

**Changes not taking effect**
- Use `wiz update <object>` to reload modified code
- Restart the server if needed

## See Also

- [Administrator Guide](ADMIN_GUIDE.md) - Server management
- [Development Guide](DEVELOPMENT.md) - Driver internals
- `/lib/std/wiztool.lpc` - Wiztool implementation
- `/lib/cmds/admin/wiz.lpc` - Wiz command wrapper

---

**Version:** 1.0  
**Last Updated:** January 27, 2026
