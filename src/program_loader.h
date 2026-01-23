/*
 * program_loader.h - Program Loader
 * 
 * This module bridges the compiler and VM by converting a compiled
 * Program (with bytecode, function offsets, and metadata) into
 * VM-executable structures (VMInstructions, VMFunctions).
 * 
 * Responsibilities:
 * - Parse bytecode array into VMInstruction stream
 * - Create VMFunction structures from function table
 * - Load constants and globals into VM
 * - Map bytecode offsets to VM instruction pointers
 */

#ifndef PROGRAM_LOADER_H
#define PROGRAM_LOADER_H

#include "compiler.h"
#include "vm.h"

/**
 * program_loader_load - Load a compiled program into the VM
 * @vm: Target virtual machine
 * @program: Compiled program with bytecode and metadata
 * 
 * Converts the Program's bytecode array into VMInstructions,
 * creates VMFunction structures for each function in the
 * function table, and loads globals/constants into the VM.
 * 
 * Returns: 0 on success, -1 on error
 */
int program_loader_load(VirtualMachine *vm, Program *program);

/**
 * program_loader_find_entry - Find entry point function in loaded program
 * @vm: Virtual machine with loaded program
 * @program: Program to search
 * @entry_name: Name of entry function (default: "create")
 * 
 * Locates the entry point function (typically "create") in the
 * loaded program and returns its VM function index.
 * 
 * Returns: Function index on success, -1 if not found
 */
int program_loader_find_entry(VirtualMachine *vm, Program *program, const char *entry_name);

/**
 * program_loader_decode_instruction - Decode single bytecode instruction
 * @bytecode: Pointer to bytecode array
 * @offset: Current offset in bytecode
 * @instr: Output VMInstruction structure
 * 
 * Internal helper to decode one bytecode instruction at the given
 * offset into a VMInstruction structure.
 * 
 * Returns: Number of bytes consumed, or -1 on error
 */
int program_loader_decode_instruction(const uint8_t *bytecode, size_t offset, VMInstruction *instr);

#endif
