/**
 * efun.c - External Functions (Efuns) Implementation
 * 
 * Built-in functions for LPC programs.
 * 
 * Phase 5 Implementation - January 22, 2026
 */

#include "efun.h"
#include "vm.h"
#include "array.h"
#include "mapping.h"
#include "object.h"
#include "compiler.h"
#include "program_loader.h"
#include "object.h"
#include "session.h"
#include <sys/stat.h>
#include <libgen.h>
#include <limits.h>
#include <dirent.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <time.h>

#define EFUN_INITIAL_CAPACITY 64

/* ========== Registry Management ========== */

EfunRegistry* efun_init(void) {
    EfunRegistry *registry = (EfunRegistry *)malloc(sizeof(EfunRegistry));
    if (!registry) return NULL;
    
    registry->efun_capacity = EFUN_INITIAL_CAPACITY;
    registry->efun_count = 0;
    registry->efuns = (EfunEntry *)malloc(sizeof(EfunEntry) * registry->efun_capacity);
    
    if (!registry->efuns) {
        free(registry);
        return NULL;
    }
    
    printf("[Efun] Initialized registry (capacity: %d)\n", registry->efun_capacity);
    
    return registry;
}

int efun_register(EfunRegistry *registry, const char *name, EfunCallback callback,
                  int min_args, int max_args, const char *signature) {
    if (!registry || !name || !callback) return -1;
    
    /* Check if we need to expand */
    if (registry->efun_count >= registry->efun_capacity) {
        registry->efun_capacity *= 2;
        registry->efuns = (EfunEntry *)realloc(registry->efuns, 
                                                sizeof(EfunEntry) * registry->efun_capacity);
        if (!registry->efuns) return -1;
    }
    
    /* Create efun entry */
    EfunEntry *entry = &registry->efuns[registry->efun_count];
    
    entry->name = (char *)malloc(strlen(name) + 1);
    strcpy(entry->name, name);
    
    entry->callback = callback;
    entry->min_args = min_args;
    entry->max_args = max_args;
    
    if (signature) {
        entry->signature = (char *)malloc(strlen(signature) + 1);
        strcpy(entry->signature, signature);
    } else {
        entry->signature = NULL;
    }
    
    registry->efun_count++;
    
    return 0;
}

EfunEntry* efun_find(EfunRegistry *registry, const char *name) {
    if (!registry || !name) return NULL;
    
    for (int i = 0; i < registry->efun_count; i++) {
        if (strcmp(registry->efuns[i].name, name) == 0) {
            return &registry->efuns[i];
        }
    }
    
    return NULL;
}

VMValue efun_call(EfunRegistry *registry, VirtualMachine *vm, 
                  const char *name, VMValue *args, int arg_count) {
    if (!registry || !name) {
        return vm_value_create_null();
    }
    
    EfunEntry *efun = efun_find(registry, name);
    if (!efun) {
        fprintf(stderr, "[Efun] Unknown efun: %s\n", name);
        return vm_value_create_null();
    }
    
    /* Check argument count */
    if (arg_count < efun->min_args) {
        fprintf(stderr, "[Efun] %s: too few arguments (got %d, need %d)\n",
                name, arg_count, efun->min_args);
        return vm_value_create_null();
    }
    
    if (efun->max_args >= 0 && arg_count > efun->max_args) {
        fprintf(stderr, "[Efun] %s: too many arguments (got %d, max %d)\n",
                name, arg_count, efun->max_args);
        return vm_value_create_null();
    }
    
    /* Call efun */
    return efun->callback(vm, args, arg_count);
}

void efun_free(EfunRegistry *registry) {
    if (!registry) return;
    
    if (registry->efuns) {
        for (int i = 0; i < registry->efun_count; i++) {
            if (registry->efuns[i].name) free(registry->efuns[i].name);
            if (registry->efuns[i].signature) free(registry->efuns[i].signature);
        }
        free(registry->efuns);
    }
    
    free(registry);
    printf("[Efun] Registry freed\n");
}

/* ========== String Functions ========== */

VMValue efun_strlen(VirtualMachine *vm, VMValue *args, int arg_count) {
    (void)vm;
    (void)arg_count;
    
    if (args[0].type != VALUE_STRING) {
        return vm_value_create_int(0);
    }
    
    return vm_value_create_int((long)strlen(args[0].data.string_value));
}

VMValue efun_substring(VirtualMachine *vm, VMValue *args, int arg_count) {
    (void)vm;
    
    if (args[0].type != VALUE_STRING || args[1].type != VALUE_INT) {
        return vm_value_create_null();
    }
    
    const char *str = args[0].data.string_value;
    int start = (int)args[1].data.int_value;
    int len = strlen(str);
    
    if (start < 0 || start >= len) {
        return vm_value_create_string("");
    }
    
    int end = len;
    if (arg_count >= 3 && args[2].type == VALUE_INT) {
        end = (int)args[2].data.int_value;
        if (end > len) end = len;
        if (end < start) end = start;
    }
    
    int substr_len = end - start;
    char *result = (char *)malloc(substr_len + 1);
    strncpy(result, str + start, substr_len);
    result[substr_len] = '\0';
    
    VMValue ret = vm_value_create_string(result);
    free(result);
    
    return ret;
}

VMValue efun_explode(VirtualMachine *vm, VMValue *args, int arg_count) {
    (void)arg_count;

    if (args[0].type != VALUE_STRING || args[1].type != VALUE_STRING) {
        return vm_value_create_null();
    }

    const char *str = args[0].data.string_value;
    const char *delim = args[1].data.string_value;
    size_t delim_len = strlen(delim);

    array_t *arr = array_new(vm->gc, 4);
    if (!arr) return vm_value_create_null();

    if (delim_len == 0) {
        array_push(arr, vm_value_create_string(str));
    } else {
        const char *start = str;
        const char *pos = NULL;
        while ((pos = strstr(start, delim)) != NULL) {
            size_t segment_len = (size_t)(pos - start);
            char *segment = (char *)malloc(segment_len + 1);
            strncpy(segment, start, segment_len);
            segment[segment_len] = '\0';
            array_push(arr, vm_value_create_string(segment));
            free(segment);
            start = pos + delim_len;
        }
        /* Remainder */
        array_push(arr, vm_value_create_string(start));
    }

    VMValue result;
    result.type = VALUE_ARRAY;
    result.data.array_value = arr;
    return result;
}

static char* value_to_cstring(VMValue v) {
    char buffer[64];
    const char *source = NULL;
    switch (v.type) {
        case VALUE_STRING:
            source = v.data.string_value ? v.data.string_value : "";
            break;
        case VALUE_INT:
            snprintf(buffer, sizeof(buffer), "%ld", v.data.int_value);
            source = buffer;
            break;
        case VALUE_FLOAT:
            snprintf(buffer, sizeof(buffer), "%g", v.data.float_value);
            source = buffer;
            break;
        case VALUE_NULL:
            source = "null";
            break;
        default:
            source = "";
            break;
    }
    size_t len = strlen(source);
    char *copy = (char *)malloc(len + 1);
    if (!copy) return NULL;
    memcpy(copy, source, len + 1);
    return copy;
}

VMValue efun_implode(VirtualMachine *vm, VMValue *args, int arg_count) {
    (void)vm;
    (void)arg_count;

    if (args[0].type != VALUE_ARRAY || args[1].type != VALUE_STRING) {
        return vm_value_create_string("");
    }

    array_t *arr = args[0].data.array_value;
    const char *delim = args[1].data.string_value;
    size_t delim_len = strlen(delim);

    size_t buffer_cap = 64;
    size_t used = 0;
    char *buffer = (char *)malloc(buffer_cap);
    if (!buffer) return vm_value_create_string("");

    for (size_t i = 0; i < array_length(arr); i++) {
        if (i > 0 && delim_len > 0) {
            if (used + delim_len + 1 > buffer_cap) {
                buffer_cap = (buffer_cap + delim_len + 1) * 2;
                buffer = (char *)realloc(buffer, buffer_cap);
            }
            memcpy(buffer + used, delim, delim_len);
            used += delim_len;
        }
        char *piece = value_to_cstring(array_get(arr, i));
        if (!piece) {
            piece = (char *)malloc(1);
            if (!piece) break;
            piece[0] = '\0';
        }
        size_t piece_len = strlen(piece);
        if (used + piece_len + 1 > buffer_cap) {
            buffer_cap = (buffer_cap + piece_len + 1) * 2;
            buffer = (char *)realloc(buffer, buffer_cap);
        }
        memcpy(buffer + used, piece, piece_len);
        used += piece_len;
        free(piece);
    }

    buffer[used] = '\0';
    VMValue ret = vm_value_create_string(buffer);
    free(buffer);
    return ret;
}

VMValue efun_upper_case(VirtualMachine *vm, VMValue *args, int arg_count) {
    (void)vm;
    (void)arg_count;
    
    if (args[0].type != VALUE_STRING) {
        return vm_value_create_null();
    }
    
    const char *str = args[0].data.string_value;
    char *result = (char *)malloc(strlen(str) + 1);
    
    for (int i = 0; str[i]; i++) {
        result[i] = (char)toupper(str[i]);
    }
    result[strlen(str)] = '\0';
    
    VMValue ret = vm_value_create_string(result);
    free(result);
    
    return ret;
}

VMValue efun_lower_case(VirtualMachine *vm, VMValue *args, int arg_count) {
    (void)vm;
    (void)arg_count;
    
    if (args[0].type != VALUE_STRING) {
        return vm_value_create_null();
    }
    
    const char *str = args[0].data.string_value;
    char *result = (char *)malloc(strlen(str) + 1);
    
    for (int i = 0; str[i]; i++) {
        result[i] = (char)tolower(str[i]);
    }
    result[strlen(str)] = '\0';
    
    VMValue ret = vm_value_create_string(result);
    free(result);
    
    return ret;
}

VMValue efun_trim(VirtualMachine *vm, VMValue *args, int arg_count) {
    (void)vm;
    (void)arg_count;
    
    if (args[0].type != VALUE_STRING) {
        return vm_value_create_null();
    }
    
    const char *str = args[0].data.string_value;
    int len = strlen(str);
    
    /* Find first non-whitespace */
    int start = 0;
    while (start < len && isspace(str[start])) start++;
    
    /* Find last non-whitespace */
    int end = len - 1;
    while (end >= start && isspace(str[end])) end--;
    
    int result_len = end - start + 1;
    char *result = (char *)malloc(result_len + 1);
    strncpy(result, str + start, result_len);
    result[result_len] = '\0';
    
    VMValue ret = vm_value_create_string(result);
    free(result);
    
    return ret;
}

/* ========== Array Functions ========== */

VMValue efun_sizeof(VirtualMachine *vm, VMValue *args, int arg_count) {
    (void)vm;
    (void)arg_count;

    if (args[0].type == VALUE_ARRAY) {
        return vm_value_create_int((long)array_length(args[0].data.array_value));
    } else if (args[0].type == VALUE_STRING) {
        return vm_value_create_int((long)strlen(args[0].data.string_value));
    } else if (args[0].type == VALUE_MAPPING) {
        return vm_value_create_int((long)mapping_size(args[0].data.mapping_value));
    }

    return vm_value_create_int(0);
}

VMValue efun_arrayp(VirtualMachine *vm, VMValue *args, int arg_count) {
    (void)vm;
    (void)arg_count;
    
    return vm_value_create_int(args[0].type == VALUE_ARRAY ? 1 : 0);
}

static int compare_values(const void *a, const void *b) {
    const VMValue *va = (const VMValue *)a;
    const VMValue *vb = (const VMValue *)b;
    if (va->type == VALUE_INT && vb->type == VALUE_INT) {
        long diff = va->data.int_value - vb->data.int_value;
        return diff < 0 ? -1 : (diff > 0 ? 1 : 0);
    }
    if (va->type == VALUE_STRING && vb->type == VALUE_STRING) {
        if (!va->data.string_value || !vb->data.string_value) return 0;
        return strcmp(va->data.string_value, vb->data.string_value);
    }
    return 0;
}

VMValue efun_sort_array(VirtualMachine *vm, VMValue *args, int arg_count) {
    (void)arg_count;

    if (args[0].type != VALUE_ARRAY) {
        return vm_value_create_null();
    }

    array_t *sorted = array_clone(args[0].data.array_value, vm->gc);
    if (!sorted) return vm_value_create_null();

    qsort(sorted->elements, sorted->length, sizeof(VMValue), compare_values);

    VMValue result;
    result.type = VALUE_ARRAY;
    result.data.array_value = sorted;
    return result;
}

VMValue efun_reverse_array(VirtualMachine *vm, VMValue *args, int arg_count) {
    (void)arg_count;

    if (args[0].type != VALUE_ARRAY) {
        return vm_value_create_null();
    }

    array_t *src = args[0].data.array_value;
    array_t *rev = array_new(vm->gc, src ? src->length : 0);
    if (!rev) return vm_value_create_null();

    if (src) {
        for (size_t i = array_length(src); i > 0; i--) {
            array_push(rev, vm_value_clone(array_get(src, i - 1)));
        }
    }

    VMValue result;
    result.type = VALUE_ARRAY;
    result.data.array_value = rev;
    return result;
}

/* ========== Math Functions ========== */

VMValue efun_abs(VirtualMachine *vm, VMValue *args, int arg_count) {
    (void)vm;
    (void)arg_count;
    
    if (args[0].type == VALUE_INT) {
        long val = args[0].data.int_value;
        return vm_value_create_int(val < 0 ? -val : val);
    } else if (args[0].type == VALUE_FLOAT) {
        return vm_value_create_float(fabs(args[0].data.float_value));
    }
    
    return vm_value_create_int(0);
}

VMValue efun_sqrt(VirtualMachine *vm, VMValue *args, int arg_count) {
    (void)vm;
    (void)arg_count;
    
    double val = 0.0;
    
    if (args[0].type == VALUE_INT) {
        val = (double)args[0].data.int_value;
    } else if (args[0].type == VALUE_FLOAT) {
        val = args[0].data.float_value;
    }
    
    return vm_value_create_float(sqrt(val));
}

VMValue efun_pow(VirtualMachine *vm, VMValue *args, int arg_count) {
    (void)vm;
    (void)arg_count;
    
    double base = 0.0, exponent = 0.0;
    
    if (args[0].type == VALUE_INT) {
        base = (double)args[0].data.int_value;
    } else if (args[0].type == VALUE_FLOAT) {
        base = args[0].data.float_value;
    }
    
    if (args[1].type == VALUE_INT) {
        exponent = (double)args[1].data.int_value;
    } else if (args[1].type == VALUE_FLOAT) {
        exponent = args[1].data.float_value;
    }
    
    return vm_value_create_float(pow(base, exponent));
}

VMValue efun_random(VirtualMachine *vm, VMValue *args, int arg_count) {
    (void)vm;
    (void)arg_count;
    
    static int seeded = 0;
    if (!seeded) {
        srand((unsigned int)time(NULL));
        seeded = 1;
    }
    
    if (args[0].type != VALUE_INT || args[0].data.int_value <= 0) {
        return vm_value_create_int(0);
    }
    
    long max = args[0].data.int_value;
    return vm_value_create_int(rand() % max);
}

VMValue efun_min(VirtualMachine *vm, VMValue *args, int arg_count) {
    (void)vm;
    (void)arg_count;
    
    if (args[0].type == VALUE_INT && args[1].type == VALUE_INT) {
        long a = args[0].data.int_value;
        long b = args[1].data.int_value;
        return vm_value_create_int(a < b ? a : b);
    }
    
    return args[0];
}

VMValue efun_max(VirtualMachine *vm, VMValue *args, int arg_count) {
    (void)vm;
    (void)arg_count;
    
    if (args[0].type == VALUE_INT && args[1].type == VALUE_INT) {
        long a = args[0].data.int_value;
        long b = args[1].data.int_value;
        return vm_value_create_int(a > b ? a : b);
    }
    
    return args[0];
}

/* ========== Type Checking Functions ========== */

VMValue efun_intp(VirtualMachine *vm, VMValue *args, int arg_count) {
    (void)vm;
    (void)arg_count;
    
    return vm_value_create_int(args[0].type == VALUE_INT ? 1 : 0);
}

VMValue efun_floatp(VirtualMachine *vm, VMValue *args, int arg_count) {
    (void)vm;
    (void)arg_count;
    
    return vm_value_create_int(args[0].type == VALUE_FLOAT ? 1 : 0);
}

VMValue efun_stringp(VirtualMachine *vm, VMValue *args, int arg_count) {
    (void)vm;
    (void)arg_count;
    
    return vm_value_create_int(args[0].type == VALUE_STRING ? 1 : 0);
}

VMValue efun_objectp(VirtualMachine *vm, VMValue *args, int arg_count) {
    (void)vm;
    (void)arg_count;
    
    return vm_value_create_int(args[0].type == VALUE_OBJECT ? 1 : 0);
}

VMValue efun_mappingp(VirtualMachine *vm, VMValue *args, int arg_count) {
    (void)vm;
    (void)arg_count;
    
    return vm_value_create_int(args[0].type == VALUE_MAPPING ? 1 : 0);
}

/* ========== I/O Functions ========== */

VMValue efun_write(VirtualMachine *vm, VMValue *args, int arg_count) {
    (void)vm;
    (void)arg_count;
    
    if (args[0].type == VALUE_STRING) {
        printf("%s", args[0].data.string_value);
    } else if (args[0].type == VALUE_INT) {
        printf("%ld", args[0].data.int_value);
    } else if (args[0].type == VALUE_FLOAT) {
        printf("%g", args[0].data.float_value);
    }
    
    fflush(stdout);
    
    return vm_value_create_int(1);
}

VMValue efun_printf(VirtualMachine *vm, VMValue *args, int arg_count) {
    (void)vm;
    (void)arg_count;
    
    /* Simple printf - just print format string for now */
    if (args[0].type == VALUE_STRING) {
        printf("%s\n", args[0].data.string_value);
        fflush(stdout);
    }
    
    return vm_value_create_int(1);
}

/* ========== Messaging Efuns ========== */

VMValue efun_tell_object(VirtualMachine *vm, VMValue *args, int arg_count) {
    (void)vm;
    (void)arg_count;

    if (arg_count < 2) {
        return vm_value_create_int(0);
    }

    if (args[0].type != VALUE_OBJECT) {
        return vm_value_create_int(0);
    }

    if (args[1].type != VALUE_STRING) {
        return vm_value_create_int(0);
    }

    obj_t *target = (obj_t *)args[0].data.object_value;
    const char *message = args[1].data.string_value;

    if (!target || !message) {
        return vm_value_create_int(0);
    }

    /* Call object's receive_message method if available */
    VMValue arg = vm_value_create_string(message);
    VMValue res = obj_call_method(vm, target, "receive_message", &arg, 1);
    vm_value_free(&arg);

    /* Free result if any to avoid leaking VMValue contents */
    if (res.type != VALUE_UNINITIALIZED) {
        vm_value_free(&res);
    }

    return vm_value_create_int(1);
}

/* ========== File path helpers ========== */

static int path_contains_parent(const char *p) {
    return strstr(p, "..") != NULL;
}

/* resolve a safe absolute path under MUD root into out (size PATH_MAX)
 * Returns 1 on success, 0 on failure */
static int resolve_safe_path(const char *path, char *out, size_t out_len) {
    char root[PATH_MAX];
    const char *env = getenv("AMLP_MUDLIB");
    if (env && *env) {
        if (!realpath(env, root)) return 0;
    } else {
        if (!getcwd(root, sizeof(root))) return 0;
    }

    if (!path || *path == '\0') return 0;
    if (path_contains_parent(path)) return 0;

    char candidate[PATH_MAX];
    if (path[0] == '/') {
        /* absolute path */
        strncpy(candidate, path, sizeof(candidate)-1);
        candidate[sizeof(candidate)-1] = '\0';
    } else {
        size_t root_len = strlen(root);
        size_t path_len = strlen(path);
        /* ensure we won't overflow candidate */
        if (root_len + 1 + path_len >= sizeof(candidate)) return 0;
        /* build path safely */
        strcpy(candidate, root);
        strcat(candidate, "/");
        strcat(candidate, path);
    }

    /* realpath on parent directory to support non-existent files */
    char tmp[PATH_MAX];
    strncpy(tmp, candidate, sizeof(tmp)-1);
    tmp[sizeof(tmp)-1] = '\0';
    char *parent = dirname(tmp);
    char parent_resolved[PATH_MAX];
    if (!realpath(parent, parent_resolved)) return 0;

    /* basename
     * basename may modify string, so use a copy */
    char cand_copy[PATH_MAX];
    strncpy(cand_copy, candidate, sizeof(cand_copy)-1);
    cand_copy[sizeof(cand_copy)-1] = '\0';
    char *base = basename(cand_copy);

    /* build final resolved path */
    if (snprintf(out, out_len, "%s/%s", parent_resolved, base) >= (int)out_len) return 0;

    /* ensure out is under root */
    size_t root_len = strlen(root);
    if (strncmp(out, root, root_len) != 0) return 0;

    return 1;
}

/* ========== File I/O efuns ========== */

VMValue efun_read_file(VirtualMachine *vm, VMValue *args, int arg_count) {
    (void)vm;

    if (arg_count < 1 || arg_count > 3) return vm_value_create_null();
    if (args[0].type != VALUE_STRING) return vm_value_create_null();

    const char *path = args[0].data.string_value;
    long start_line = 1;
    long num_lines = -1;
    if (arg_count >= 2 && args[1].type == VALUE_INT) start_line = args[1].data.int_value;
    if (arg_count >= 3 && args[2].type == VALUE_INT) num_lines = args[2].data.int_value;
    if (start_line < 1) start_line = 1;

    char resolved[PATH_MAX];
    if (!resolve_safe_path(path, resolved, sizeof(resolved))) {
        return vm_value_create_null();
    }

    FILE *fp = fopen(resolved, "r");
    if (!fp) return vm_value_create_null();

    const size_t MAX_READ = 1024 * 1024; /* 1MB */
    size_t alloc = 1024;
    char *result = malloc(alloc);
    if (!result) { fclose(fp); return vm_value_create_null(); }
    result[0] = '\0';
    size_t used = 0;

    char linebuf[4096];
    long lineno = 0;
    long lines_read = 0;
    while (fgets(linebuf, sizeof(linebuf), fp)) {
        lineno++;
        if (lineno < start_line) continue;
        if (num_lines != -1 && lines_read >= num_lines) break;

        size_t add = strlen(linebuf);
        if (used + add + 1 > alloc) {
            size_t new_alloc = alloc * 2;
            while (used + add + 1 > new_alloc) new_alloc *= 2;
            if (new_alloc > MAX_READ) { /* exceed limit */
                free(result); fclose(fp); return vm_value_create_null();
            }
            char *n = realloc(result, new_alloc);
            if (!n) { free(result); fclose(fp); return vm_value_create_null(); }
            result = n; alloc = new_alloc;
        }
        memcpy(result + used, linebuf, add);
        used += add;
        result[used] = '\0';
        lines_read++;
    }

    fclose(fp);

    if (!used) {
        /* return empty string rather than null */
        VMValue v = vm_value_create_string("");
        free(result);
        return v;
    }

    VMValue v = vm_value_create_string(result);
    free(result);
    return v;
}

VMValue efun_write_file(VirtualMachine *vm, VMValue *args, int arg_count) {
    (void)vm;

    if (arg_count != 2) return vm_value_create_int(0);
    if (args[0].type != VALUE_STRING || args[1].type != VALUE_STRING) return vm_value_create_int(0);

    const char *path = args[0].data.string_value;
    const char *content = args[1].data.string_value;

    char resolved[PATH_MAX];
    if (!resolve_safe_path(path, resolved, sizeof(resolved))) {
        return vm_value_create_int(0);
    }

    FILE *fp = fopen(resolved, "a");
    if (!fp) return vm_value_create_int(0);

    size_t len = strlen(content);
    size_t written = fwrite(content, 1, len, fp);
    fclose(fp);

    return vm_value_create_int(written == len ? 1 : 0);
}

VMValue efun_file_size(VirtualMachine *vm, VMValue *args, int arg_count) {
    (void)vm;

    if (arg_count != 1) return vm_value_create_int(0);
    if (args[0].type != VALUE_STRING) return vm_value_create_int(0);

    const char *path = args[0].data.string_value;
    char resolved[PATH_MAX];
    if (!resolve_safe_path(path, resolved, sizeof(resolved))) {
        return vm_value_create_int(0);
    }

    struct stat st;
    if (stat(resolved, &st) != 0) {
        return vm_value_create_int(0);
    }

    if (S_ISDIR(st.st_mode)) return vm_value_create_int(-2);
    if (S_ISREG(st.st_mode)) return vm_value_create_int(-1);
    return vm_value_create_int(0);
}

/* ========== Directory efuns ========== */

VMValue efun_get_dir(VirtualMachine *vm, VMValue *args, int arg_count) {
    (void)vm;

    if (arg_count != 1) return vm_value_create_null();
    if (args[0].type != VALUE_STRING) return vm_value_create_null();

    const char *path = args[0].data.string_value;
    char resolved[PATH_MAX];
    if (!resolve_safe_path(path, resolved, sizeof(resolved))) {
        return vm_value_create_null();
    }

    DIR *d = opendir(resolved);
    if (!d) return vm_value_create_null();

    array_t *arr = array_new(vm->gc, 8);
    if (!arr) { closedir(d); return vm_value_create_null(); }

    struct dirent *ent;
    while ((ent = readdir(d)) != NULL) {
        if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0) continue;
        VMValue v = vm_value_create_string(ent->d_name);
        array_push(arr, v);
    }

    closedir(d);

    VMValue out;
    out.type = VALUE_ARRAY;
    out.data.array_value = arr;
    return out;
}

VMValue efun_mkdir(VirtualMachine *vm, VMValue *args, int arg_count) {
    (void)vm;

    if (arg_count != 1) return vm_value_create_int(0);
    if (args[0].type != VALUE_STRING) return vm_value_create_int(0);

    const char *path = args[0].data.string_value;
    char resolved[PATH_MAX];
    if (!resolve_safe_path(path, resolved, sizeof(resolved))) {
        return vm_value_create_int(0);
    }

    int r = mkdir(resolved, 0755);
    if (r == 0) return vm_value_create_int(1);
    return vm_value_create_int(0);
}

VMValue efun_rm(VirtualMachine *vm, VMValue *args, int arg_count) {
    (void)vm;

    if (arg_count != 1) return vm_value_create_int(0);
    if (args[0].type != VALUE_STRING) return vm_value_create_int(0);

    const char *path = args[0].data.string_value;
    char resolved[PATH_MAX];
    if (!resolve_safe_path(path, resolved, sizeof(resolved))) {
        return vm_value_create_int(0);
    }

    int r = unlink(resolved);
    if (r == 0) return vm_value_create_int(1);
    return vm_value_create_int(0);
}

/* ========== Object/Player Efuns ========== */

static ObjManager *get_global_obj_manager(void) {
    static ObjManager *mgr = NULL;
    if (!mgr) mgr = obj_manager_init();
    return mgr;
}

VMValue efun_clone_object(VirtualMachine *vm, VMValue *args, int arg_count) {
    if (arg_count != 1 || args[0].type != VALUE_STRING) return vm_value_create_null();
    const char *lpc_path = args[0].data.string_value;
    if (!lpc_path) return vm_value_create_null();

    /* Map LPC path like "/std/wiztool" -> <MUDLIB>/lib/std/wiztool.lpc */
    char fs_path[PATH_MAX];
    const char *mudlib = getenv("AMLP_MUDLIB");
    if (!mudlib || !*mudlib) mudlib = "./lib";

    /* strip leading slash */
    const char *p = lpc_path[0] == '/' ? lpc_path + 1 : lpc_path;
    if (snprintf(fs_path, sizeof(fs_path), "%s/%s.lpc", mudlib, p) >= (int)sizeof(fs_path)) {
        return vm_value_create_null();
    }

    /* Compile source file */
    Program *prog = compiler_compile_file(fs_path);
    if (!prog) {
        return vm_value_create_null();
    }

    /* Load into VM (adds VMFunction entries) */
    if (program_loader_load(vm, prog) != 0) {
        program_free(prog);
        return vm_value_create_null();
    }

    /* Create object and register it */
    obj_t *o = obj_new(lpc_path);
    if (!o) {
        program_free(prog);
        return vm_value_create_null();
    }
    ObjManager *mgr = get_global_obj_manager();
    if (mgr) obj_manager_register(mgr, o);

    /* Attach functions from program to object by name lookup in VM */
    for (size_t fi = 0; fi < prog->function_count; fi++) {
        const char *fname = prog->functions[fi].name;
        if (!fname) continue;
        /* Find VMFunction pointer by name */
        for (int i = 0; i < vm->function_count; i++) {
            if (vm->functions[i] && strcmp(vm->functions[i]->name, fname) == 0) {
                obj_add_method(o, vm->functions[i]);
                break;
            }
        }
    }

    /* Debug: report which key methods were attached */
    {
        int has_process = obj_get_method(o, "process_command") ? 1 : 0;
        int has_setup = obj_get_method(o, "setup_player") ? 1 : 0;
        int has_save = obj_get_method(o, "save_me") ? 1 : 0;
        fprintf(stderr, "[Efun] clone_object: created '%s' methods=%d setup=%d save_me=%d\n",
                o->name ? o->name : "<noname>", o->method_count, has_setup, has_save);
    }

    /* Call create() on object if present */
    obj_call_method(vm, o, "create", NULL, 0);

    program_free(prog);

    VMValue v;
    v.type = VALUE_OBJECT;
    v.data.object_value = o;
    return v;
}

VMValue efun_find_object(VirtualMachine *vm, VMValue *args, int arg_count) {
    (void)vm;
    if (arg_count != 1 || args[0].type != VALUE_STRING) return vm_value_create_null();
    const char *path = args[0].data.string_value;
    if (!path) return vm_value_create_null();

    ObjManager *mgr = get_global_obj_manager();
    if (!mgr) return vm_value_create_null();

    obj_t *found = obj_manager_find(mgr, path);
    if (found) {
        VMValue v; v.type = VALUE_OBJECT; v.data.object_value = found; return v;
    }

    /* fallback: search by name equality */
    for (int i = 0; i < mgr->object_count; i++) {
        if (mgr->objects[i] && mgr->objects[i]->name && strcmp(mgr->objects[i]->name, path) == 0) {
            VMValue v; v.type = VALUE_OBJECT; v.data.object_value = mgr->objects[i]; return v;
        }
    }

    return vm_value_create_null();
}

VMValue efun_call_other(VirtualMachine *vm, VMValue *args, int arg_count) {
    if (arg_count < 2) return vm_value_create_null();

    obj_t *target = NULL;
    if (args[0].type == VALUE_OBJECT) {
        target = (obj_t *)args[0].data.object_value;
    } else if (args[0].type == VALUE_STRING) {
        /* find object by path/name */
        ObjManager *mgr = get_global_obj_manager();
        if (!mgr) return vm_value_create_null();
        for (int i = 0; i < mgr->object_count; i++) {
            if (mgr->objects[i] && mgr->objects[i]->name && strcmp(mgr->objects[i]->name, args[0].data.string_value) == 0) {
                target = mgr->objects[i]; break;
            }
        }
    }

    if (!target) return vm_value_create_null();
    if (args[1].type != VALUE_STRING) return vm_value_create_null();

    const char *method = args[1].data.string_value;
    int sub_args = arg_count - 2;
    VMValue *sub = NULL;
    if (sub_args > 0) sub = &args[2];

    return obj_call_method(vm, target, method, sub, sub_args);
}

VMValue efun_present(VirtualMachine *vm, VMValue *args, int arg_count) {
    (void)vm;
    if (arg_count < 1) return vm_value_create_null();
    if (args[0].type != VALUE_STRING) return vm_value_create_null();

    const char *id = args[0].data.string_value;
    obj_t *where = NULL;
    if (arg_count >= 2 && args[1].type == VALUE_OBJECT) where = (obj_t *)args[1].data.object_value;

    ObjManager *mgr = get_global_obj_manager();
    if (!mgr) return vm_value_create_null();

    for (int i = 0; i < mgr->object_count; i++) {
        obj_t *o = mgr->objects[i];
        if (!o) continue;
        /* If where is provided, ensure object's environment matches */
        if (where) {
            VMValue env = obj_get_prop(o, "environment");
            int match_env = 0;
            if (env.type == VALUE_OBJECT && env.data.object_value == where) match_env = 1;
            if (!match_env) continue;
        }

        /* match by object name */
        if (o->name && strcmp(o->name, id) == 0) {
            VMValue v; v.type = VALUE_OBJECT; v.data.object_value = o; return v;
        }

        /* match by id property */
        VMValue pid = obj_get_prop(o, "id");
        if (pid.type == VALUE_STRING && strcmp(pid.data.string_value, id) == 0) {
            VMValue v; v.type = VALUE_OBJECT; v.data.object_value = o; return v;
        }
    }

    return vm_value_create_null();
}

VMValue efun_environment(VirtualMachine *vm, VMValue *args, int arg_count) {
    (void)vm;
    if (arg_count != 1 || args[0].type != VALUE_OBJECT) return vm_value_create_null();
    obj_t *o = (obj_t *)args[0].data.object_value;
    if (!o) return vm_value_create_null();

    VMValue env = obj_get_prop(o, "environment");
    if (env.type == VALUE_UNINITIALIZED) return vm_value_create_null();
    return env;
}

VMValue efun_move_object(VirtualMachine *vm, VMValue *args, int arg_count) {
    (void)vm;
    if (arg_count != 2) return vm_value_create_int(0);
    if (args[0].type != VALUE_OBJECT || args[1].type != VALUE_OBJECT) return vm_value_create_int(0);

    obj_t *src = (obj_t *)args[0].data.object_value;
    obj_t *dst = (obj_t *)args[1].data.object_value;
    if (!src || !dst) return vm_value_create_int(0);

    VMValue v;
    v.type = VALUE_OBJECT;
    v.data.object_value = dst;
    if (obj_set_prop(src, "environment", v) != 0) return vm_value_create_int(0);
    return vm_value_create_int(1);
}

VMValue efun_this_player(VirtualMachine *vm, VMValue *args, int arg_count) {
    (void)vm; (void)args; (void)arg_count;
    void *po = get_current_player_object();
    if (!po) return vm_value_create_null();
    VMValue v;
    v.type = VALUE_OBJECT;
    v.data.object_value = po;
    return v;
}

VMValue efun_file_name(VirtualMachine *vm, VMValue *args, int arg_count) {
    (void)vm;
    if (arg_count != 1 || args[0].type != VALUE_OBJECT) return vm_value_create_string("");
    obj_t *o = (obj_t *)args[0].data.object_value;
    if (!o || !o->name) return vm_value_create_string("");
    return vm_value_create_string(o->name);
}

/* ========== Utility Functions ========== */

int efun_register_all(EfunRegistry *registry) {
    if (!registry) return 0;
    int before = registry->efun_count;
    int count = 0;
    
    /* String functions */
    efun_register(registry, "strlen", efun_strlen, 1, 1, "int strlen(string)");
    efun_register(registry, "substring", efun_substring, 2, 3, "string substring(string, int, int)");
    efun_register(registry, "explode", efun_explode, 2, 2, "string* explode(string, string)");
    efun_register(registry, "implode", efun_implode, 2, 2, "string implode(string*, string)");
    efun_register(registry, "upper_case", efun_upper_case, 1, 1, "string upper_case(string)");
    efun_register(registry, "lower_case", efun_lower_case, 1, 1, "string lower_case(string)");
    efun_register(registry, "trim", efun_trim, 1, 1, "string trim(string)");
    count += 7;
    
    /* Array functions */
    efun_register(registry, "sizeof", efun_sizeof, 1, 1, "int sizeof(mixed)");
    efun_register(registry, "arrayp", efun_arrayp, 1, 1, "int arrayp(mixed)");
    efun_register(registry, "sort_array", efun_sort_array, 1, 1, "mixed* sort_array(mixed*)");
    efun_register(registry, "reverse_array", efun_reverse_array, 1, 1, "mixed* reverse_array(mixed*)");
    count += 4;
    
    /* Math functions */
    efun_register(registry, "abs", efun_abs, 1, 1, "mixed abs(mixed)");
    efun_register(registry, "sqrt", efun_sqrt, 1, 1, "float sqrt(mixed)");
    efun_register(registry, "pow", efun_pow, 2, 2, "float pow(mixed, mixed)");
    efun_register(registry, "random", efun_random, 1, 1, "int random(int)");
    efun_register(registry, "min", efun_min, 2, 2, "mixed min(mixed, mixed)");
    efun_register(registry, "max", efun_max, 2, 2, "mixed max(mixed, mixed)");
    count += 6;
    
    /* Type checking */
    efun_register(registry, "intp", efun_intp, 1, 1, "int intp(mixed)");
    efun_register(registry, "floatp", efun_floatp, 1, 1, "int floatp(mixed)");
    efun_register(registry, "stringp", efun_stringp, 1, 1, "int stringp(mixed)");
    efun_register(registry, "objectp", efun_objectp, 1, 1, "int objectp(mixed)");
    efun_register(registry, "mappingp", efun_mappingp, 1, 1, "int mappingp(mixed)");
    count += 5;
    
    /* I/O functions */
    efun_register(registry, "tell_object", efun_tell_object, 2, 2, "int tell_object(object, string)");
    efun_register(registry, "read_file", efun_read_file, 1, 3, "string read_file(string, int, int)");
    efun_register(registry, "write_file", efun_write_file, 2, 2, "int write_file(string, string)");
    efun_register(registry, "file_size", efun_file_size, 1, 1, "int file_size(string)");
    efun_register(registry, "get_dir", efun_get_dir, 1, 1, "string* get_dir(string)");
    efun_register(registry, "mkdir", efun_mkdir, 1, 1, "int mkdir(string)");
    efun_register(registry, "rm", efun_rm, 1, 1, "int rm(string)");
    /* Object/player efuns */
    efun_register(registry, "call_other", efun_call_other, 2, -1, "mixed call_other(object|string, string, ...)");
    efun_register(registry, "clone_object", efun_clone_object, 1, 1, "object clone_object(string)");
    efun_register(registry, "find_object", efun_find_object, 1, 1, "object find_object(string)");
    efun_register(registry, "present", efun_present, 1, 2, "object present(string, object)");
    efun_register(registry, "environment", efun_environment, 1, 1, "object environment(object)");
    efun_register(registry, "move_object", efun_move_object, 2, 2, "int move_object(object, object)");
    efun_register(registry, "this_player", efun_this_player, 0, 0, "object this_player()");
    efun_register(registry, "file_name", efun_file_name, 1, 1, "string file_name(object)");
    efun_register(registry, "write", efun_write, 1, 1, "int write(mixed)");
    efun_register(registry, "printf", efun_printf, 1, -1, "int printf(string, ...)");
    count += 2;
    
    int registered = registry->efun_count - before;
    printf("[Efun] Registered %d standard efuns\n", registered);

    return registered;
}

void efun_print_all(EfunRegistry *registry) {
    if (!registry) return;
    
    printf("\n[Efun] Registered efuns (%d):\n", registry->efun_count);
    
    for (int i = 0; i < registry->efun_count; i++) {
        EfunEntry *e = &registry->efuns[i];
        printf("  [%2d] %s: %s\n", i, e->name, 
               e->signature ? e->signature : "no signature");
    }
    
    printf("\n");
}

int efun_get_count(EfunRegistry *registry) {
    return registry ? registry->efun_count : 0;
}
