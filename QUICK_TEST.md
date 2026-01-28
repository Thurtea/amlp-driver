# Quick Test Instructions

The driver failed to start because there's a process holding port 3000. Here's how to fix it and test:

## Step 1: Kill Everything and Start Clean

```bash
# Kill any old drivers (force kill)
pkill -9 -f 'build/driver'
pkill -9 -f 'driver 3000'

# Wait for processes to die
sleep 2

# Verify port is free
ss -tln | grep :3000
# Should show nothing

# Start fresh
cd ~/amlp-driver
./build/driver 3000 2>&1 | tee /tmp/driver_output.log &

# Wait for startup
sleep 3

# Check if it's running
ps aux | grep 'build/driver' | grep -v grep
```

## Step 2: Check Startup Logs

```bash
# Show last 40 lines of startup
tail -40 /tmp/driver_output.log

# Look for these key messages:
# - "Master object initializing..."
# - "Loaded: /daemon/command"
# - "Server listening on port 3000"
```

## Step 3: Manual Test (Quick Verification)

```bash
telnet localhost 3000
```

Then type:
```
thurtea
[password]
[password again]
stats
cd /lib
pwd
ls
eval 2 + 2
quit
```

Expected results:
- `stats` shows `Privilege Level: 2 (Admin)`
- `cd /lib` shows "Changed directory to: /lib" (NOT "Unknown command")
- `pwd` shows "Current directory: /lib"
- `ls` shows directory contents
- `eval 2 + 2` shows "Result: int: 4"

## Step 4: Automated Test

```bash
cd ~/amlp-driver
python3 robust_mud_test.py 2>&1 | tee /tmp/auto_test.log
tail -60 /tmp/auto_test.log
```

## What We're Looking For

### ✅ SUCCESS looks like:
```
Wiztool attached.
Type 'wiz help' for wizard commands.

> pwd
Current directory: /

> cd /lib
Changed directory to: /lib

> eval 2 + 2
Result: int: 4
```

### ❌ FAILURE looks like:
```
> cd /lib
Unknown command: cd
Type 'help' for available commands.
```

## If It Still Fails

Share:
1. Output of `tail -40 /tmp/driver_output.log`
2. Output of the manual test (what happened when you typed `cd /lib`)
3. Contents of `/tmp/auto_test.log`

This will show us:
- Did the daemon load?
- Is wiztool being attached?
- Is process_input() routing correctly?

---

## Quick Diagnostic Commands

```bash
# Is driver running?
ps aux | grep 'build/driver' | grep -v grep

# What's on port 3000?
ss -tln | grep :3000

# Recent logs
tail -50 /tmp/driver_output.log

# Test connection
echo "quit" | telnet localhost 3000
```
