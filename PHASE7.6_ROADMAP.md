# Phase 7.6: VM/Executor Implementation Roadmap

## Overview
Implement bytecode interpreter and object runtime to run the MUD online.

## Components

### 1. VM Core (src/vm.c, src/vm.h)
**Bytecode Interpreter:**
- [ ] Stack-based VM with 1024-element stack
- [ ] Instruction dispatcher (switch/case for all opcodes)
- [ ] Arithmetic ops (ADD, SUB, MUL, DIV, MOD)
- [ ] Comparison ops (EQ, NE, LT, GT, LE, GE)
- [ ] Logic ops (AND, OR, NOT)
- [ ] Stack ops (PUSH, POP, DUP, SWAP)
- [ ] Control flow (JMP, JZ, JNZ, CALL, RET)
- [ ] Variable access (LOAD_LOCAL, STORE_LOCAL, LOAD_GLOBAL, STORE_GLOBAL)

**Estimated:** 3-5 days

### 2. Object Runtime (src/object_runtime.c, src/object_runtime.h)
**Object Lifecycle:**
- [ ] Object loader (read compiled .o files)
- [ ] create() method auto-call on load
- [ ] destruct() method call on cleanup
- [ ] Object table/cache (hash map of loaded objects)
- [ ] Inherit chain resolution
- [ ] Function lookup across inheritance

**Estimated:** 2-3 days

### 3. Apply Method Dispatcher (src/apply.c, src/apply.h)
**Master Object Integration:**
- [ ] apply() function to call object methods
- [ ] Return value handling
- [ ] Error handling (catch runtime errors)
- [ ] Master object apply methods:
  - [ ] connect() - new player connections
  - [ ] compile_object() - lazy loading
  - [ ] log_error() - error reporting

**Estimated:** 1-2 days

### 4. Network Layer (src/network.c, src/network.h)
**Telnet Server:**
- [ ] TCP listener on port 4000
- [ ] Accept new connections
- [ ] Read player input (line-buffered)
- [ ] Send output to players
- [ ] Connection object creation (call master->connect())
- [ ] Command parsing and routing

**Estimated:** 2-3 days

### 5. Integration & Testing
- [ ] Link all components together
- [ ] Test VM with simple bytecode
- [ ] Test object loading and create()
- [ ] Test inheritance resolution
- [ ] Test network connections (telnet localhost 4000)
- [ ] Load tutorial area and test gameplay

**Estimated:** 2-3 days

## Total Estimate: 10-16 days

## Success Criteria
```bash
# Start the MUD
./build/driver run --mudlib /home/thurtea/amlp-library --port 4000

# In another terminal, connect
telnet localhost 4000

# Should see:
# "Welcome to the tutorial area!"
# Player can look, move, interact with objects
```
