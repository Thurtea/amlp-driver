## File Placement Rules

### Driver Files (amlp-driver)
- **C source files (.c):** /home/thurtea/amlp-driver/src/
  - Example: vm.c, compiler.c, efun.c
  - Naming: lowercase, underscores for word separation
  
- **C header files (.h):** /home/thurtea/amlp-driver/src/
  - Example: vm.h, compiler.h, efun.h
  - Must match corresponding .c file names

- **Protected files** (do NOT overwrite):
  - src/*.c.backup - Any backup files
  - Makefile, README.md, *.md documentation
  - logs/*, data/*, save/* - Runtime data
  - .git/*, .gitignore - Version control

### Library Files (amlp-library)
- **Standard library (.c):** /home/thurtea/amlp-library/std/
  - Example: object.c, living.c, room.c
  - Base classes and shared functionality

- **Secure/privileged (.c):** /home/thurtea/amlp-library/secure/
  - Example: simul_efun.c, master.c
  - System-level objects requiring special permissions

- **Domain-specific objects (.c):** /home/thurtea/amlp-library/domains/<domain>/<category>/
  - Categories: rooms/, objects/, npc/
  - Example: /home/thurtea/amlp-library/domains/tutorial/rooms/start.c
  - Create missing directories automatically

- **Protected files** (do NOT overwrite without explicit confirmation):
  - master.c - Main master object
  - secure/simul_efun.c - Core simulated efuns
  - Makefile, documentation files
  - .git/* - Version control

### File Writing Safety Protocol
1. Check if target file exists and is not in protected list
2. Create parent directories with `mkdir -p` if needed
3. Write to temporary file: `<target>.tmp`
4. Verify write succeeded
5. Atomic rename: `mv <target>.tmp <target>`
6. Set permissions to 0644 (rw-r--r--)

### Path Examples
```
Driver C file: /home/thurtea/amlp-driver/src/new_module.c
Driver header: /home/thurtea/amlp-driver/src/new_module.h
Library std: /home/thurtea/amlp-library/std/container.c
Library domain: /home/thurtea/amlp-library/domains/tutorial/rooms/training_hall.c
Library secure: /home/thurtea/amlp-library/secure/permissions.c
```

### Notes for the Windows application
- Use WSL-aware path mapping (e.g., `\\wsl$\<distro>\...` or `wsl` to write into `/home/thurtea/...`).
- Prefer atomic writes via temporary files and rename to avoid corrupting runtime files.
- Do not overwrite protected files unless the user explicitly confirms.
- Ensure the process performing writes maps to the WSL user `thurtea`, or runs `wsl` to perform writes as that user.

---
Generated placement guide for saving files from Windows applications into the WSL workspace.
