/*
 * vm_trace.c - VM tracing and debugging utilities
 *
 * Provides instruction-level tracing, call stack dumps, bytecode viewers,
 * and lightweight memory profiling for VMValue allocations.
 */

#include "vm.h"
#include <stdlib.h>
#include <string.h>

static VirtualMachine *vm_profile_owner = NULL;

static const char *vm_debug_opcode_name(OpCode opcode) {
    switch (opcode) {
        case OP_PUSH_INT: return "PUSH_INT";
        case OP_PUSH_FLOAT: return "PUSH_FLOAT";
        case OP_PUSH_STRING: return "PUSH_STRING";
        case OP_PUSH_NULL: return "PUSH_NULL";
        case OP_POP: return "POP";
        case OP_DUP: return "DUP";
        case OP_LOAD_LOCAL: return "LOAD_LOCAL";
        case OP_STORE_LOCAL: return "STORE_LOCAL";
        case OP_LOAD_GLOBAL: return "LOAD_GLOBAL";
        case OP_STORE_GLOBAL: return "STORE_GLOBAL";
        case OP_ADD: return "ADD";
        case OP_SUB: return "SUB";
        case OP_MUL: return "MUL";
        case OP_DIV: return "DIV";
        case OP_MOD: return "MOD";
        case OP_NEG: return "NEG";
        case OP_EQ: return "EQ";
        case OP_NE: return "NE";
        case OP_LT: return "LT";
        case OP_LE: return "LE";
        case OP_GT: return "GT";
        case OP_GE: return "GE";
        case OP_AND: return "AND";
        case OP_OR: return "OR";
        case OP_NOT: return "NOT";
        case OP_BIT_AND: return "BIT_AND";
        case OP_BIT_OR: return "BIT_OR";
        case OP_BIT_XOR: return "BIT_XOR";
        case OP_BIT_NOT: return "BIT_NOT";
        case OP_LSHIFT: return "LSHIFT";
        case OP_RSHIFT: return "RSHIFT";
        case OP_JUMP: return "JUMP";
        case OP_JUMP_IF_FALSE: return "JUMP_IF_FALSE";
        case OP_JUMP_IF_TRUE: return "JUMP_IF_TRUE";
        case OP_CALL: return "CALL";
        case OP_RETURN: return "RETURN";
        case OP_MAKE_ARRAY: return "MAKE_ARRAY";
        case OP_INDEX_ARRAY: return "INDEX_ARRAY";
        case OP_STORE_ARRAY: return "STORE_ARRAY";
        case OP_MAKE_MAPPING: return "MAKE_MAPPING";
        case OP_INDEX_MAPPING: return "INDEX_MAPPING";
        case OP_STORE_MAPPING: return "STORE_MAPPING";
        case OP_CALL_METHOD: return "CALL_METHOD";
        case OP_HALT: return "HALT";
        case OP_PRINT: return "PRINT";
        default: return "UNKNOWN";
    }
}

static FILE *vm_trace_output(VirtualMachine *vm) {
    if (vm && vm->trace_output) {
        return vm->trace_output;
    }
    return stderr;
}

static int vm_trace_line_for_frame(CallFrame *frame, int instruction_index) {
    if (!frame || !frame->function || !frame->function->line_map) {
        return -1;
    }

    if (instruction_index < 0 || instruction_index >= frame->function->line_map_count) {
        return -1;
    }

    return frame->function->line_map[instruction_index];
}

static void vm_trace_dump_stack(VirtualMachine *vm, FILE *out) {
    if (!vm || !vm->stack || !out) return;

    fprintf(out, "  Stack[%d]:", vm->stack->top);
    for (int i = 0; i < vm->stack->top; i++) {
        char *str = vm_value_to_string(vm->stack->values[i]);
        fprintf(out, " [%d]=%s", i, str ? str : "(null)");
        if (str) free(str);
    }
    fprintf(out, "\n");
}

static void vm_trace_dump_locals(CallFrame *frame, FILE *out) {
    if (!frame || !frame->function || !frame->local_variables || !out) return;

    fprintf(out, "  Locals(%d):", frame->function->local_var_count);
    for (int i = 0; i < frame->function->local_var_count; i++) {
        char *str = vm_value_to_string(frame->local_variables[i]);
        fprintf(out, " [%d]=%s", i, str ? str : "(null)");
        if (str) free(str);
    }
    fprintf(out, "\n");
}

void vm_debug_init(VirtualMachine *vm) {
    if (!vm) return;

    vm_profile_owner = vm;

    const char *trace = getenv("AMLP_VM_TRACE");
    const char *trace_stack = getenv("AMLP_VM_TRACE_STACK");
    const char *trace_locals = getenv("AMLP_VM_TRACE_LOCALS");
    const char *callstack = getenv("AMLP_VM_CALLSTACK");
    const char *memprofile = getenv("AMLP_VM_MEMPROFILE");
    const char *bytecode = getenv("AMLP_VM_BYTECODE");
    const char *trace_file = getenv("AMLP_VM_TRACE_FILE");

    if (trace && strcmp(trace, "0") != 0) {
        vm->debug_flags |= VM_DEBUG_TRACE;
    }
    if (trace_stack && strcmp(trace_stack, "0") != 0) {
        vm->debug_flags |= VM_DEBUG_TRACE_STACK;
    }
    if (trace_locals && strcmp(trace_locals, "0") != 0) {
        vm->debug_flags |= VM_DEBUG_TRACE_LOCALS;
    }
    if (callstack && strcmp(callstack, "0") != 0) {
        vm->debug_flags |= VM_DEBUG_CALLSTACK;
    }
    if (memprofile && strcmp(memprofile, "0") != 0) {
        vm->debug_flags |= VM_DEBUG_MEMPROFILE;
    }
    if (bytecode && strcmp(bytecode, "0") != 0) {
        vm->debug_flags |= VM_DEBUG_BYTECODE;
    }

    if (trace_file && *trace_file) {
        FILE *out = fopen(trace_file, "a");
        if (out) {
            vm->trace_output = out;
        }
    }
}

void vm_debug_set_flags(VirtualMachine *vm, unsigned int flags) {
    if (!vm) return;
    vm->debug_flags = flags;
}

unsigned int vm_debug_get_flags(VirtualMachine *vm) {
    if (!vm) return 0;
    return vm->debug_flags;
}

void vm_debug_enable(VirtualMachine *vm, unsigned int flags) {
    if (!vm) return;
    vm->debug_flags |= flags;
}

void vm_debug_disable(VirtualMachine *vm, unsigned int flags) {
    if (!vm) return;
    vm->debug_flags &= ~flags;
}

void vm_trace_instruction(VirtualMachine *vm, CallFrame *frame,
                          VMInstruction *instr, int instruction_index) {
    if (!vm || !instr) return;

    FILE *out = vm_trace_output(vm);
    const char *func_name = frame && frame->function ? frame->function->name : "<toplevel>";
    const char *source_file = frame && frame->function && frame->function->source_file
        ? frame->function->source_file
        : "<unknown>";
    int line = vm_trace_line_for_frame(frame, instruction_index);

    fprintf(out, "[VM TRACE] %s ip=%d op=%s", func_name, instruction_index,
            vm_debug_opcode_name(instr->opcode));
    if (line >= 0) {
        fprintf(out, " line=%d file=%s", line, source_file);
    }
    fprintf(out, "\n");

    if (vm->debug_flags & VM_DEBUG_TRACE_STACK) {
        vm_trace_dump_stack(vm, out);
    }
    if ((vm->debug_flags & VM_DEBUG_TRACE_LOCALS) && frame) {
        vm_trace_dump_locals(frame, out);
    }
}

void vm_trace_dump_call_stack(VirtualMachine *vm, const char *reason) {
    if (!vm) return;

    FILE *out = vm_trace_output(vm);
    fprintf(out, "[VM CALLSTACK] %s\n", reason ? reason : "(no reason)");

    CallFrame *frame = vm->current_frame;
    int depth = 0;
    while (frame) {
        const char *func_name = frame->function ? frame->function->name : "<unknown>";
        const char *source_file = frame->function && frame->function->source_file
            ? frame->function->source_file
            : "<unknown>";
        int ip = frame->instruction_pointer - 1;
        int line = vm_trace_line_for_frame(frame, ip);

        fprintf(out, "  #%d %s ip=%d", depth, func_name, ip);
        if (line >= 0) {
            fprintf(out, " line=%d file=%s", line, source_file);
        }
        fprintf(out, "\n");

        if (frame->function && frame->function->param_count > 0 && frame->local_variables) {
            fprintf(out, "    params:");
            for (int i = 0; i < frame->function->param_count; i++) {
                char *str = vm_value_to_string(frame->local_variables[i]);
                fprintf(out, " [%d]=%s", i, str ? str : "(null)");
                if (str) free(str);
            }
            fprintf(out, "\n");
        }

        frame = frame->prev;
        depth++;
    }
}

void vm_trace_dump_function(VirtualMachine *vm, VMFunction *function, FILE *out) {
    (void)vm;
    if (!function || !out) return;

    const char *source_file = function->source_file ? function->source_file : "<unknown>";
    fprintf(out, "\n[BYTECODE] Function: %s (params=%d locals=%d)\n",
            function->name, function->param_count, function->local_var_count);
    fprintf(out, "[BYTECODE] Source: %s\n", source_file);

    if (function->local_var_count > 0) {
        fprintf(out, "[BYTECODE] Locals layout:\n");
        for (int i = 0; i < function->local_var_count; i++) {
            const char *kind = (i < function->param_count) ? "param" : "local";
            fprintf(out, "  [%d] %s\n", i, kind);
        }
    }

    fprintf(out, "[BYTECODE] Instructions:\n");
    for (int i = 0; i < function->instruction_count; i++) {
        VMInstruction instr = function->instructions[i];
        int line = (function->line_map && i < function->line_map_count)
            ? function->line_map[i]
            : -1;
        if (line >= 0) {
            fprintf(out, "  [%04d] line=%d op=%s", i, line, vm_debug_opcode_name(instr.opcode));
        } else {
            fprintf(out, "  [%04d] op=%s", i, vm_debug_opcode_name(instr.opcode));
        }

        switch (instr.opcode) {
            case OP_PUSH_INT:
                fprintf(out, " %ld", instr.operand.int_operand);
                break;
            case OP_PUSH_FLOAT:
                fprintf(out, " %g", instr.operand.float_operand);
                break;
            case OP_PUSH_STRING:
                fprintf(out, " \"%s\"", instr.operand.string_operand ? instr.operand.string_operand : "");
                break;
            case OP_JUMP:
            case OP_JUMP_IF_FALSE:
            case OP_JUMP_IF_TRUE:
                fprintf(out, " -> %d", instr.operand.address_operand);
                break;
            case OP_CALL:
                fprintf(out, " func=%s args=%d",
                        instr.operand.call_operand.name ? instr.operand.call_operand.name : "<index>",
                        instr.operand.call_operand.arg_count);
                break;
            case OP_LOAD_LOCAL:
            case OP_STORE_LOCAL:
            case OP_LOAD_GLOBAL:
            case OP_STORE_GLOBAL:
            case OP_MAKE_ARRAY:
            case OP_MAKE_MAPPING:
            case OP_CALL_METHOD:
                fprintf(out, " %ld", instr.operand.int_operand);
                break;
            default:
                break;
        }
        fprintf(out, "\n");
    }
    fprintf(out, "\n");
}

void vm_profile_note_create(VMValue value, size_t bytes) {
    if (!vm_profile_owner) return;

    if (value.type >= 0 && value.type <= VM_VALUE_TYPE_MAX) {
        vm_profile_owner->profile.create_count[value.type]++;
    }

    if (value.type == VALUE_STRING) {
        vm_profile_owner->profile.string_allocs++;
        vm_profile_owner->profile.string_bytes_alloc += bytes;
    }
}

void vm_profile_note_free(VMValue value, size_t bytes) {
    if (!vm_profile_owner) return;

    if (value.type >= 0 && value.type <= VM_VALUE_TYPE_MAX) {
        vm_profile_owner->profile.free_count[value.type]++;
    }

    if (value.type == VALUE_STRING) {
        vm_profile_owner->profile.string_frees++;
        vm_profile_owner->profile.string_bytes_free += bytes;
    }
}

void vm_profile_report(FILE *out) {
    if (!vm_profile_owner) return;

    if (!out) out = stderr;

    fprintf(out, "[VM PROFILE] VMValue allocations\n");
    for (int i = 0; i <= VM_VALUE_TYPE_MAX; i++) {
        unsigned long created = vm_profile_owner->profile.create_count[i];
        unsigned long freed = vm_profile_owner->profile.free_count[i];
        if (created == 0 && freed == 0) continue;
        fprintf(out, "  type=%d created=%lu freed=%lu outstanding=%ld\n",
                i, created, freed, (long)(created - freed));
    }

    fprintf(out, "[VM PROFILE] Strings\n");
    fprintf(out, "  allocs=%lu frees=%lu outstanding=%ld\n",
            vm_profile_owner->profile.string_allocs,
            vm_profile_owner->profile.string_frees,
            (long)(vm_profile_owner->profile.string_allocs - vm_profile_owner->profile.string_frees));
    fprintf(out, "  bytes_alloc=%zu bytes_free=%zu bytes_outstanding=%ld\n",
            vm_profile_owner->profile.string_bytes_alloc,
            vm_profile_owner->profile.string_bytes_free,
            (long)(vm_profile_owner->profile.string_bytes_alloc - vm_profile_owner->profile.string_bytes_free));
}
