# Next Session Checklist
**Date:** January 23, 2026  
**Focus:** amlp-library Creation (Phase 7.5)

## Prerequisites [DONE]
- [x] Driver compilation pipeline complete
- [x] All 500+ tests passing
- [x] CLI interface functional
- [x] Documentation committed to git

## Session Goals

### 1. Create amlp-library Repository Structure
```bash
cd /home/thurtea
mkdir -p amlp-library/{secure,std,domains/tutorial/{rooms,objects},docs}
cd amlp-library
git init
```

### 2. Implement Core Files

#### master.c (Required)
- [ ] `void create()` - Initialize master object
- [ ] `object connect()` - Handle player connections
- [ ] `void compile_object(string file)` - Pre-compilation hook
- [ ] `string *epilog(string file)` - Simul efun loading
- [ ] `void log_error(string file, string message)` - Error logging

#### secure/simul_efun.c
- [ ] String utilities: `capitalize()`, `proper_name()`
- [ ] Object utilities: `present()`, `environment()`
- [ ] Array utilities: `filter_array()`, `map_array()`

#### std/object.c (Base Object)
- [ ] `void create()` - Base initialization
- [ ] `void setup()` - Object setup hook
- [ ] `string query_name()` - Get object name
- [ ] `string query_short()` - Short description
- [ ] `string query_long()` - Long description
- [ ] `void set_name(string n)` - Set object name

#### std/living.c (inherits std/object.c)
- [ ] `int query_hp()` - Get hit points
- [ ] `void set_hp(int hp)` - Set hit points
- [ ] `void move(object destination)` - Move living thing

#### std/room.c (inherits std/object.c)
- [ ] `void set_exit(string dir, string dest)` - Add exit
- [ ] `mapping query_exits()` - Get all exits
- [ ] `string *query_exit_dirs()` - Get exit directions

### 3. Create Test Content

#### domains/tutorial/rooms/start.c
- [ ] Simple starting room
- [ ] Basic description
- [ ] One or two exits

#### domains/tutorial/objects/sword.c
- [ ] Simple weapon object
- [ ] Weight and value
- [ ] Basic description

### 4. Compilation Testing

```bash
# Test each file individually
cd /home/thurtea
./amlp-driver/build/driver compile amlp-library/master.c
./amlp-driver/build/driver compile amlp-library/secure/simul_efun.c
./amlp-driver/build/driver compile amlp-library/std/object.c
./amlp-driver/build/driver compile amlp-library/std/living.c
./amlp-driver/build/driver compile amlp-library/std/room.c
./amlp-driver/build/driver compile amlp-library/domains/tutorial/rooms/start.c

# Test all files at once
find amlp-library -name "*.c" -exec ./amlp-driver/build/driver compile {} \; | grep -E "(FAIL|Error)"
```

### 5. Documentation

#### amlp-library/README.md
- [ ] Purpose and structure
- [ ] Compilation instructions
- [ ] Directory layout
- [ ] Next steps

#### amlp-library/docs/ARCHITECTURE.md
- [ ] Master object flow
- [ ] Inheritance hierarchy
- [ ] Object lifecycle
- [ ] Simul efun system

#### amlp-library/docs/INHERITANCE_MAP.md
- [ ] Visual inheritance tree
- [ ] Method inheritance chains
- [ ] Override patterns

## Files Needed for Reference

Copy these to reference location:
```bash
cp /home/thurtea/amlp-driver/src/efun.h /tmp/
cp /home/thurtea/amlp-driver/PHASE7.2_QUICK_REFERENCE.md /tmp/
cp /home/thurtea/mud-references/all_efuns_final.json /tmp/
```

## Success Criteria

- [ ] All library files compile without errors
- [ ] Master object loads successfully
- [ ] Base inheritance hierarchy works
- [ ] Test room and object compile
- [ ] Documentation complete
- [ ] Git repository initialized and committed

## Integration Preparation

After library is working:
- [ ] Document file paths for Windows assistant
- [ ] Create compilation test script
- [ ] Write WSL integration guide
- [ ] Test cross-platform file access

## Notes

**LPC Language Subset Supported:**
- Variables: `int`, `string`, `object`, `mapping`, `mixed`, `void`
- Arrays: `int *`, `string *`, `object *`
- Control flow: `if/else`, `while`, `for`, `return`
- Functions: declaration and calls
- Object: `this_object()`
- Efuns: See efun.h for available functions

**Not Yet Supported:**
- Classes/structs
- Switch statements
- Advanced preprocessor directives
- Multiple inheritance
- Closures/function pointers
