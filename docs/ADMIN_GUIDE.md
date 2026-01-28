# AMLP Driver - Administrator Guide

Complete guide for setting up and administering an AMLP MUD server.

## Table of Contents

1. [Server Setup](#server-setup)
2. [Port Configuration](#port-configuration)
3. [Privilege System](#privilege-system)
4. [First Player Admin Grant](#first-player-admin-grant)
5. [Managing Privileges](#managing-privileges)
6. [Connecting to the MUD](#connecting-to-the-mud)
7. [Server Management](#server-management)
8. [Troubleshooting](#troubleshooting)

## Server Setup

### Prerequisites

- GCC 4.9+ or Clang
- GNU Make
- Standard C library (POSIX)
- Network access (port 3000 by default)

### Building the Server

```bash
cd /path/to/amlp-driver
make clean
make all
```

This builds:
- `build/driver` - Main driver executable with integrated network server
- `build/test_*` - Test suites

### Initial Configuration

The server expects the LPC library to be in the `lib/` directory:

```
amlp-driver/
├── build/driver        # Server binary
├── lib/                # LPC mudlib
│   ├── secure/         # Master object and security
│   ├── std/            # Standard inheritance objects
│   ├── domains/        # Game world areas
│   └── cmds/           # Player and admin commands
├── config/             # Configuration files
└── logs/               # Server logs
```

### Starting the Server

Use the control script:

```bash
./mud.ctl start
```

The server will:
1. Start on port 3000 (default)
2. Load the master object from `lib/secure/master.c`
3. Initialize the virtual machine
4. Begin accepting connections
5. Log output to `lib/log/server.log`

Verify the server is running:

```bash
./mud.ctl status
```

## Port Configuration

### Changing the Default Port

The default port is defined in `src/driver.c`:

```c
#define DEFAULT_PORT 3000
```

To change the port:

1. Edit `src/driver.c`:

```c
#define DEFAULT_PORT 4000  // Change to desired port
```

2. Rebuild the server:

```bash
make clean
make all
```

3. Update `mud.ctl` script:

```bash
DEFAULT_PORT=4000  # Change line 7 to match
```

4. Restart the server:

```bash
./mud.ctl restart
```

### Runtime Port Override

You can specify a port when starting the server directly:

```bash
./build/driver 4000
```

Note: The `mud.ctl` script uses the port defined in its `DEFAULT_PORT` variable.

### Firewall Configuration

Ensure the port is accessible:

```bash
# For firewalld (Fedora/RHEL)
sudo firewall-cmd --add-port=3000/tcp --permanent
sudo firewall-cmd --reload

# For ufw (Ubuntu/Debian)
sudo ufw allow 3000/tcp

# For iptables
sudo iptables -A INPUT -p tcp --dport 3000 -j ACCEPT
sudo iptables-save
```

## Privilege System

The AMLP Driver implements a three-tier privilege system defined in `lib/include/globals.h`:

### Privilege Levels

```c
#define PLAYER_LEVEL 0    // Regular players
#define WIZARD_LEVEL 1    // Builders and moderators
#define ADMIN_LEVEL  2    // Full administrators
```

### Level Capabilities

#### PLAYER_LEVEL (0) - Regular Players

**Permissions:**
- Basic movement and interaction commands
- Communication (say, tell, shout)
- Standard game commands (look, get, drop, etc.)
- Personal inventory management

**Restrictions:**
- Cannot use admin commands
- Cannot promote other players
- Cannot modify game objects
- Cannot access restricted areas

#### WIZARD_LEVEL (1) - Wizards/Builders

**Permissions (in addition to PLAYER_LEVEL):**
- Object creation and modification
- Access to wizard commands
- Room and domain building
- Advanced debugging tools
- Access to wizard areas

**Restrictions:**
- Cannot promote players to wizard or admin
- Cannot use admin-only commands
- Cannot modify core system objects

#### ADMIN_LEVEL (2) - Administrators

**Permissions (all privileges):**
- Server management commands
- Player promotion and demotion
- System-wide modifications
- Access to all areas and commands
- Server shutdown capability
- User management

### Commands by Privilege Level

| Command | Player | Wizard | Admin | Description |
|---------|--------|--------|-------|-------------|
| look    | Yes    | Yes    | Yes   | Examine surroundings |
| say     | Yes    | Yes    | Yes   | Speak to others in room |
| go      | Yes    | Yes    | Yes   | Move between rooms |
| who     | Yes    | Yes    | Yes   | List online players |
| quit    | Yes    | Yes    | Yes   | Disconnect from MUD |
| stat    | No     | Yes    | Yes   | View object/player stats |
| goto    | No     | Yes    | Yes   | Teleport to locations |
| clone   | No     | Yes    | Yes   | Create object instances |
| promote | No     | No     | Yes   | Change player privileges |
| shutdown| No     | No     | Yes   | Stop the server |

## First Player Admin Grant

### Automatic Admin Promotion

The first player to create a character automatically receives Admin privileges (level 2).

**Implementation:** Located in `src/driver.c` lines 686-707:

```c
/* First player becomes admin */
if (!first_player_created) {
    session->privilege_level = 2;  /* Admin */
    first_player_created = 1;
    fprintf(stderr, "[Server] First player created: %s (privilege: Admin)\n",
           session->username);
    
    send_to_player(session, 
        "\r\nCharacter created successfully!\r\n"
        "As the first player, you have been granted Admin privileges.\r\n"
        "You materialize in the starting room.\r\n");
} else {
    session->privilege_level = 0;  /* Regular player */
    // ... regular player creation
}
```

### First Login Process

1. **Connect to the server:**
   ```bash
   telnet localhost 3000
   ```

2. **Enter a username** when prompted:
   ```
   Welcome to AMLP-MUD!
   
   Enter username: admin
   ```

3. **Create a new character:**
   ```
   User 'admin' not found.
   Create new character? (yes/no): yes
   ```

4. **Set a password:**
   ```
   Enter password: ********
   Confirm password: ********
   ```

5. **Receive admin confirmation:**
   ```
   Character created successfully!
   As the first player, you have been granted Admin privileges.
   You materialize in the starting room.
   ```

### Verification

Check your privilege level:
```
> stat me
Player: admin
Level: 2 (Admin)
Location: Void Room
```

### Persistence Note

The first player flag is set when the server starts and resets on server restart. To maintain persistent admin status:

1. Grant admin to your account immediately after first login
2. The privilege is stored in the player session
3. For persistent storage, implement player save files in `lib/save/players/`

## Managing Privileges

### The Promote Command

Admins can change any player's privilege level using the `promote` command.

**Syntax:**
```
promote <username> <level>
```

**Parameters:**
- `username`: Target player's name (must be online)
- `level`: New privilege level (0=player, 1=wizard, 2=admin)

### Promoting Players

#### Making a Wizard

```
> promote alice 1
Alice has been promoted to Wizard.
```

Alice will see:
```
You have been promoted to Wizard by admin!
```

#### Making an Admin

```
> promote bob 2
Bob has been promoted to Admin.
```

Bob will see:
```
You have been promoted to Admin by admin!
```

#### Demoting Players

```
> promote charlie 0
Charlie has been demoted to Player.
```

### Promote Command Examples

**Promote to wizard:**
```
> promote alice 1
Alice has been promoted to Wizard.
```

**Promote to admin:**
```
> promote bob 2
Bob has been promoted to Admin.
```

**Demote to player:**
```
> promote charlie 0
Charlie has been demoted to Player.
```

**Invalid usage:**
```
> promote alice
Usage: promote <player> <level>
Levels: 0=player, 1=wizard, 2=admin

> promote alice 5
Invalid level. Use 0 (player), 1 (wizard), or 2 (admin).

> promote unknownuser 1
Player 'unknownuser' not found online.
```

### Privilege Management Best Practices

1. **Admin Accounts:**
   - Create separate admin accounts for each administrator
   - Use strong passwords
   - Document admin account holders

2. **Wizard Accounts:**
   - Grant wizard privileges to trusted builders
   - Review wizard activities periodically
   - Provide wizard documentation

3. **Security:**
   - Never share admin credentials
   - Regularly audit privilege levels
   - Implement activity logging for admin commands

## Connecting to the MUD

### Using Telnet

Telnet is the simplest way to connect.

#### Linux/Mac

```bash
telnet localhost 3000
```

Or connect to a remote server:
```bash
telnet mudserver.example.com 3000
```

#### Windows

1. Enable Telnet Client:
   - Control Panel > Programs > Turn Windows features on or off
   - Check "Telnet Client"
   - Click OK

2. Connect:
   ```cmd
   telnet localhost 3000
   ```

#### Telnet Commands

While connected:
- **Exit:** Press `Ctrl+]` then type `quit`
- **Suspend:** Press `Ctrl+Z` (Unix)
- **Paste:** Right-click (Windows) or Shift+Insert

### Using Mudlet

Mudlet is a modern MUD client with scripting, triggers, and enhanced features.

#### Installation

**Linux:**
```bash
# Ubuntu/Debian
sudo apt install mudlet

# Fedora
sudo dnf install mudlet

# Arch
sudo pacman -S mudlet
```

**macOS:**
```bash
brew install --cask mudlet
```

**Windows:**
- Download from https://www.mudlet.org/download/
- Run installer
- Launch Mudlet

#### Connecting with Mudlet

1. **Launch Mudlet**

2. **Create New Profile:**
   - Click "New"
   - Profile name: `AMLP-MUD`
   - Server address: `localhost` (or your server IP)
   - Port: `3000`
   - Click "Connect"

3. **Configure Settings:**
   - **General:** Set character name
   - **Display:** Adjust font size and colors
   - **Special Options:** 
     - Enable "Force GA/EOR for prompt detection"
     - Enable "Force MXP off"

4. **Save Profile:**
   - Mudlet saves your connection settings
   - Next time, select profile and click "Connect"

#### Mudlet Features

**Aliases:**
Create shortcuts for common commands:
```lua
-- Example: Type "k" to "kill"
lua k="kill"
```

**Triggers:**
Automatically respond to game text:
```lua
-- Example: Auto-greet new players
-- Pattern: ^(\w+) has entered the game
-- Response: say Hello, %1!
```

**Timers:**
Execute commands periodically:
```lua
tempTimer(60, [[send("save")]])  -- Auto-save every 60 seconds
```

**Buttons:**
Create clickable command buttons in the UI.

#### Advanced Mudlet Setup

**Connection Profile Example:**

```lua
-- File: ~/.config/mudlet/profiles/AMLP-MUD/config.lua
{
  name = "AMLP-MUD",
  host = "localhost",
  port = 3000,
  login = "your_username",
  password = "",  -- Leave blank for security
}
```

**Auto-login Script:**

```lua
-- Trigger on "Enter username:"
send("admin")

-- Trigger on "Enter password:"
sendPassword("your_password_here")
```

### Other MUD Clients

- **TinTin++** (Linux/Mac): Command-line client with scripting
- **MUSHclient** (Windows): Feature-rich with plugins
- **zmud/cmud** (Windows): Commercial client with advanced features
- **BeipMU** (Windows): Modern, lightweight client

## Server Management

### Control Script Commands

The `mud.ctl` script provides complete server management.

#### Start Server

```bash
./mud.ctl start
```

Output:
```
Starting AMLP-MUD server on port 3000...
Server started successfully (PID: 12345)
Log file: /path/to/amlp-driver/lib/log/server.log
```

#### Stop Server

```bash
./mud.ctl stop
```

Performs graceful shutdown:
1. Sends SIGTERM to server process
2. Waits up to 10 seconds for graceful shutdown
3. Forces SIGKILL if necessary
4. Cleans up PID file

#### Restart Server

```bash
./mud.ctl restart
```

Equivalent to stop + start with 2-second delay.

#### Check Status

```bash
./mud.ctl status
```

Output when running:
```
Server is running (PID: 12345)
Port: 3000
Uptime: 2-04:15:23
```

Output when stopped:
```
Server is not running
```

#### View Logs

```bash
./mud.ctl log
```

Tails the server log file in real-time. Press Ctrl+C to exit.

### Manual Server Management

#### Start Manually

```bash
cd /path/to/amlp-driver
./build/driver 3000 > lib/log/server.log 2>&1 &
echo $! > mud.pid
```

#### Stop Manually

```bash
kill $(cat mud.pid)
rm mud.pid
```

#### Monitor Process

```bash
ps aux | grep driver
```

#### Check Port

```bash
netstat -tlnp | grep 3000
# or
ss -tlnp | grep 3000
```

### Log Management

#### Log File Location

```
lib/log/server.log
```

#### Viewing Logs

```bash
# View entire log
cat lib/log/server.log

# View last 50 lines
tail -n 50 lib/log/server.log

# Follow log in real-time
tail -f lib/log/server.log

# Search logs
grep "ERROR" lib/log/server.log
```

#### Log Rotation

Manual rotation:
```bash
mv lib/log/server.log lib/log/server.log.$(date +%Y%m%d)
./mud.ctl restart
```

Automatic rotation with logrotate:
```bash
# Create /etc/logrotate.d/amlp-mud
/path/to/amlp-driver/lib/log/server.log {
    daily
    rotate 7
    compress
    missingok
    notifempty
    postrotate
        /path/to/amlp-driver/mud.ctl reload > /dev/null
    endscript
}
```

### In-Game Admin Commands

Once connected as an admin:

#### Load Wiztool

```
> wiztool
You load your wiztool. Type 'wiz help' for commands.
```

The wiztool is the primary in-game building tool. It allows you to:
- Create and clone objects without external editors
- Evaluate LPC code in real-time
- Navigate and manage files and directories
- Build rooms and objects from within the game
- Inspect and modify loaded objects

See [WIZTOOL.md](WIZTOOL.md) for complete wiztool documentation.

#### Common Wiztool Commands

```
> wiz help                    Show all wiztool commands
> wiz clone /std/object       Clone an object
> wiz cd /domains/start       Change directory
> wiz ls                      List current directory
> wiz goto /path/to/room      Teleport to room
> wiz eval 1 + 1              Evaluate LPC code
```

#### Shutdown Server

```
> shutdown
Server shutting down...
```

Gracefully disconnects all players and stops the server.

#### View Online Players

```
> who
Online players:
  admin [Admin]
  alice [Wiz]
  bob
  charlie

Total: 4 players
```

#### Teleport to Location

```
> goto /domains/start/room/void
You teleport to the Void Room.
```

#### View Player Stats

```
> stat alice
Player: alice
Level: 1 (Wizard)
Location: Welcome Room
Connected: 15 minutes
IP: 192.168.1.100
```

## Troubleshooting

### Server Won't Start

**Error: "Server binary not found"**

Solution:
```bash
make clean
make all
```

**Error: "Address already in use"**

The port is already bound. Find and kill the process:
```bash
# Find process using port 3000
lsof -i :3000
# or
netstat -tlnp | grep 3000

# Kill the process
kill -9 <PID>

# Start server
./mud.ctl start
```

**Error: "Permission denied on port"**

Ports below 1024 require root access. Either:
1. Use a port >= 1024 (recommended)
2. Run with sudo (not recommended)
3. Grant capability: `sudo setcap 'cap_net_bind_service=+ep' build/driver`

### Cannot Connect

**Connection refused:**

1. Verify server is running:
   ```bash
   ./mud.ctl status
   ```

2. Check port is listening:
   ```bash
   netstat -tlnp | grep 3000
   ```

3. Check firewall:
   ```bash
   sudo firewall-cmd --list-ports
   ```

4. Test local connection:
   ```bash
   telnet localhost 3000
   ```

**Connection times out:**

- Check firewall rules
- Verify server is listening on correct interface
- Test from same machine first, then remote

### Stale PID File

**Error: "Server not running (stale PID)"**

Solution:
```bash
rm mud.pid
./mud.ctl start
```

### Orphaned Processes

If server is running but not responding:

```bash
# Find all driver processes
ps aux | grep driver

# Kill orphaned processes
pkill -9 driver

# Clean up
rm mud.pid
./mud.ctl start
```

### First Player Already Created

If you need to reset the first player flag:

1. Restart the server:
   ```bash
   ./mud.ctl restart
   ```

2. The `first_player_created` flag resets on server startup

3. First player to connect gets admin privileges

Note: For persistent admin privileges, implement player save functionality.

### Player Data Not Saving

Current implementation stores data in memory only. To implement persistence:

1. Create `lib/save/players/` directory
2. Implement save/restore functions in player object
3. Call save on disconnect and periodically
4. Load player data on login

### Log File Too Large

Rotate logs regularly:

```bash
# Manual rotation
mv lib/log/server.log lib/log/server.log.old
./mud.ctl reload

# Or use logrotate (see Log Management section)
```

### Memory Leaks

Monitor server memory usage:

```bash
ps aux | grep driver
```

If memory grows unbounded:
1. Check for memory leaks in custom code
2. Restart server periodically
3. Review object creation/destruction

### Network Performance

If experiencing lag:

1. Check server load:
   ```bash
   top -p $(cat mud.pid)
   ```

2. Monitor network connections:
   ```bash
   netstat -an | grep 3000 | wc -l
   ```

3. Review MAX_CLIENTS in `src/driver.c` (default: 100)

4. Optimize command processing loops

## Security Considerations

### Password Security

1. **Never log passwords** in plain text
2. Implement password hashing (bcrypt/scrypt recommended)
3. Enforce minimum password length (8+ characters)
4. Consider password complexity requirements

### Admin Account Security

1. **Strong passwords** for all admin accounts
2. **Limit admin accounts** to trusted individuals only
3. **Audit admin actions** via logging
4. **Separate admin and player accounts**

### Network Security

1. **Firewall configuration** - Only expose necessary ports
2. **Rate limiting** - Prevent connection flooding
3. **IP filtering** - Whitelist/blacklist as needed
4. **DDoS protection** - Use reverse proxy if public

### Code Security

1. **Input validation** - Sanitize all player input
2. **Command injection** - Avoid shell execution
3. **Buffer overflows** - Check array bounds
4. **Regular updates** - Keep codebase current

## Additional Resources

### Documentation

- [README.md](../README.md) - Project overview
- [DEVELOPMENT.md](DEVELOPMENT.md) - Technical implementation details
- [BUILD_SUMMARY.md](BUILD_SUMMARY.md) - Build system documentation

### Support

- GitHub Issues: https://github.com/Thurtea/amlp-driver/issues
- Project Repository: https://github.com/Thurtea/amlp-driver

### LPC Resources

- LPC Language Reference (link to documentation)
- MUD Building Guides
- Community Forums

---

**Document Version:** 1.0  
**Last Updated:** January 27, 2026  
**Driver Version:** 0.6.0 (Phase 6 Complete)
