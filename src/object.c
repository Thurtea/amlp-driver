/**
 * object.c - LPC Object System Implementation
 * 
 * Implements the object type system for the AMLP LPC driver.
 * Provides object creation, property/method management, and inheritance.
 * 
 * Phase 4 Implementation - January 22, 2026
 */

#include "object.h"
#include "vm.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* ========== Property Hash Functions ========== */

/**
 * Hash function for property names
 * Simple polynomial rolling hash
 */
static int property_hash(const char *name, int capacity) {
    unsigned int hash = 0;
    for (const char *p = name; *p; p++) {
        hash = ((hash << 5) + hash) ^ (*p);
    }
    return hash % capacity;
}

/**
 * Find property in hash table (this object only, no inheritance)
 * Returns pointer to property entry, or NULL if not found
 */
static ObjProperty* find_property(obj_t *obj, const char *prop_name) {
    if (!obj || !prop_name || !obj->properties) return NULL;
    
    int hash = property_hash(prop_name, obj->property_capacity);
    ObjProperty *entry = obj->properties[hash];
    
    while (entry) {
        if (strcmp(entry->name, prop_name) == 0) {
            return entry;
        }
        entry = entry->next;
    }
    
    return NULL;
}

/* ========== Object Lifecycle Functions ========== */

obj_t* obj_new(const char *name) {
    if (!name) return NULL;
    
    obj_t *obj = (obj_t *)malloc(sizeof(obj_t));
    if (!obj) return NULL;
    
    /* Copy name */
    obj->name = (char *)malloc(strlen(name) + 1);
    strcpy(obj->name, name);
    
    /* Initialize prototype */
    obj->proto = NULL;
    
    /* Initialize properties hash table */
    obj->property_capacity = OBJ_PROPERTY_HASH_SIZE;
    obj->property_count = 0;
    obj->properties = (ObjProperty **)malloc(sizeof(ObjProperty *) * obj->property_capacity);
    for (int i = 0; i < obj->property_capacity; i++) {
        obj->properties[i] = NULL;
    }
    
    /* Initialize methods array */
    obj->method_capacity = OBJ_INITIAL_METHOD_CAPACITY;
    obj->method_count = 0;
    obj->methods = (VMFunction **)malloc(sizeof(VMFunction *) * obj->method_capacity);
    
    /* Initialize state */
    obj->ref_count = 1;
    obj->is_destroyed = 0;
    
    return obj;
}

obj_t* obj_clone(obj_t *original) {
    if (!original) return NULL;
    
    /* Create new object with modified name */
    char clone_name[256];
    snprintf(clone_name, sizeof(clone_name), "%s#clone", original->name);
    
    obj_t *clone = obj_new(clone_name);
    if (!clone) return NULL;
    
    /* Set original as prototype for inheritance */
    clone->proto = original;
    original->ref_count++;
    
    return clone;
}

void obj_destroy(obj_t *obj) {
    if (!obj || obj->is_destroyed) return;
    
    obj->is_destroyed = 1;
    
    printf("[Object] Destroyed object '%s'\n", obj->name);
}

void obj_free(obj_t *obj) {
    if (!obj) return;
    
    /* Free name */
    if (obj->name) {
        free(obj->name);
        obj->name = NULL;
    }
    
    /* Free properties */
    if (obj->properties) {
        for (int i = 0; i < obj->property_capacity; i++) {
            ObjProperty *entry = obj->properties[i];
            while (entry) {
                ObjProperty *next = entry->next;
                if (entry->name) free(entry->name);
                vm_value_free(&entry->value);
                free(entry);
                entry = next;
            }
        }
        free(obj->properties);
        obj->properties = NULL;
    }
    
    /* Free methods array (but not the functions themselves - managed by VM) */
    if (obj->methods) {
        free(obj->methods);
        obj->methods = NULL;
    }
    
    /* Decrement prototype reference */
    if (obj->proto) {
        obj->proto->ref_count--;
    }
    
    free(obj);
}

/* ========== Property Access Functions ========== */

VMValue obj_get_prop(obj_t *obj, const char *prop_name) {
    if (!obj || !prop_name) {
        return vm_value_create_null();
    }
    
    /* Search this object */
    ObjProperty *prop = find_property(obj, prop_name);
    if (prop) {
        return prop->value;
    }
    
    /* Search prototype chain */
    if (obj->proto) {
        return obj_get_prop(obj->proto, prop_name);
    }
    
    /* Not found */
    return vm_value_create_null();
}

int obj_set_prop(obj_t *obj, const char *prop_name, VMValue value) {
    if (!obj || !prop_name) return -1;
    
    /* Check if property already exists */
    ObjProperty *prop = find_property(obj, prop_name);
    if (prop) {
        /* Update existing property */
        vm_value_free(&prop->value);
        prop->value = value;
        return 0;
    }
    
    /* Create new property */
    int hash = property_hash(prop_name, obj->property_capacity);
    
    ObjProperty *new_prop = (ObjProperty *)malloc(sizeof(ObjProperty));
    if (!new_prop) return -1;
    
    new_prop->name = (char *)malloc(strlen(prop_name) + 1);
    strcpy(new_prop->name, prop_name);
    new_prop->value = value;
    new_prop->next = obj->properties[hash];
    
    obj->properties[hash] = new_prop;
    obj->property_count++;
    
    return 0;
}

int obj_has_prop(obj_t *obj, const char *prop_name) {
    if (!obj || !prop_name) return 0;
    
    /* Check this object */
    if (find_property(obj, prop_name) != NULL) {
        return 1;
    }
    
    /* Check prototype chain */
    if (obj->proto) {
        return obj_has_prop(obj->proto, prop_name);
    }
    
    return 0;
}

int obj_delete_prop(obj_t *obj, const char *prop_name) {
    if (!obj || !prop_name || !obj->properties) return -1;
    
    int hash = property_hash(prop_name, obj->property_capacity);
    ObjProperty **indirect = &obj->properties[hash];
    
    while (*indirect) {
        ObjProperty *entry = *indirect;
        if (strcmp(entry->name, prop_name) == 0) {
            /* Found it - remove from chain */
            *indirect = entry->next;
            if (entry->name) free(entry->name);
            vm_value_free(&entry->value);
            free(entry);
            obj->property_count--;
            return 0;
        }
        indirect = &entry->next;
    }
    
    return -1;  /* Not found */
}

/* ========== Method Management Functions ========== */

int obj_add_method(obj_t *obj, VMFunction *method) {
    if (!obj || !method) return -1;
    
    /* Check if we need to expand methods array */
    if (obj->method_count >= obj->method_capacity) {
        obj->method_capacity *= 2;
        obj->methods = (VMFunction **)realloc(obj->methods, 
                                               sizeof(VMFunction *) * obj->method_capacity);
        if (!obj->methods) return -1;
    }
    
    /* Add method */
    int index = obj->method_count;
    obj->methods[index] = method;
    obj->method_count++;
    
    return index;
}

VMFunction* obj_get_method(obj_t *obj, const char *method_name) {
    if (!obj || !method_name) return NULL;
    
    /* Search this object's methods */
    for (int i = 0; i < obj->method_count; i++) {
        if (obj->methods[i] && strcmp(obj->methods[i]->name, method_name) == 0) {
            return obj->methods[i];
        }
    }
    
    /* Search prototype chain */
    if (obj->proto) {
        return obj_get_method(obj->proto, method_name);
    }
    
    return NULL;
}

VMValue obj_call_method(VirtualMachine *vm, obj_t *obj, const char *method_name,
                        VMValue *args, int arg_count) {
    if (!vm || !obj || !method_name) {
        return vm_value_create_null();
    }
    
    /* Find the method */
    VMFunction *method = obj_get_method(obj, method_name);
    if (!method) {
        fprintf(stderr, "[Object] Method '%s' not found in object '%s'\n", 
                method_name, obj->name);
        return vm_value_create_null();
    }
    
    /* Verify argument count */
    if (arg_count != method->param_count) {
        fprintf(stderr, "[Object] Method '%s' expects %d arguments, got %d\n",
                method_name, method->param_count, arg_count);
        return vm_value_create_null();
    }
    
    /* Preserve VM state to avoid leaking stack growth into caller */
    int saved_top = vm->stack ? vm->stack->top : 0;
    int saved_running = vm->running;

    /* Push arguments onto stack (in call order) */
    for (int i = 0; i < arg_count; i++) {
        fprintf(stderr, "[Object] ARG %d: type=%d", i, args[i].type);
        if (args[i].type == VALUE_STRING) {
            fprintf(stderr, " ptr=%p len=%zu", 
                    (void*)args[i].data.string_value,
                    args[i].data.string_value ? strlen(args[i].data.string_value) : 0);
            if (args[i].data.string_value) {
                fprintf(stderr, " value='%s'", args[i].data.string_value);
            }
        }
        fprintf(stderr, "\n");
        
        vm_push_value(vm, args[i]);
        
        /* Verify it was pushed correctly */
        if (vm->stack->top > 0) {
            VMValue pushed = vm->stack->values[vm->stack->top - 1];
            fprintf(stderr, "[Object]   Pushed to stack[%d]: type=%d ptr=%p\n", 
                    vm->stack->top - 1, pushed.type,
                    pushed.type == VALUE_STRING ? (void*)pushed.data.string_value : NULL);
        }
    }
    
    /* Find method index in VM's function table */
    int method_idx = -1;
    for (int i = 0; i < vm->function_count; i++) {
        if (vm->functions[i] == method) {
            method_idx = i;
            break;
        }
    }
    
    if (method_idx == -1) {
        fprintf(stderr, "[Object] Method '%s' not found in VM function table\n", method_name);
        return vm_value_create_null();
    }
    
    /* Call the method through VM */
    vm_call_function(vm, method_idx, arg_count);

    /* Capture return value if one was produced */
    VMValue result = vm_value_create_null();
    if (vm->stack && vm->stack->top > saved_top) {
        result = vm_pop_value(vm);
    }

    /* Release all arguments that were pushed on the stack
     * The vm_pop_value calls above only adjust stack->top, they don't
     * decrement refcounts. We must explicitly release string argument
     * references before abandoning them.
     */
    while (vm->stack && vm->stack->top > saved_top) {
        VMValue arg = vm->stack->values[--vm->stack->top];
        vm_value_release(&arg);
    }

    /* Restore stack and running state */
    if (vm->stack) {
        vm->stack->top = saved_top;
    }
    vm->running = saved_running;

    return result;
}

/* ========== Inheritance Functions ========== */

void obj_set_proto(obj_t *obj, obj_t *proto) {
    if (!obj) return;
    
    /* Decrement old prototype's ref count */
    if (obj->proto) {
        obj->proto->ref_count--;
    }
    
    /* Set new prototype */
    obj->proto = proto;
    
    /* Increment new prototype's ref count */
    if (proto) {
        proto->ref_count++;
    }
}

obj_t* obj_get_proto(obj_t *obj) {
    return obj ? obj->proto : NULL;
}

/* ========== Object Manager Functions ========== */

ObjManager* obj_manager_init(void) {
    ObjManager *manager = (ObjManager *)malloc(sizeof(ObjManager));
    if (!manager) return NULL;
    
    manager->object_capacity = OBJ_MANAGER_INITIAL_CAPACITY;
    manager->object_count = 0;
    manager->objects = (obj_t **)malloc(sizeof(obj_t *) * manager->object_capacity);
    
    if (!manager->objects) {
        free(manager);
        return NULL;
    }
    
    return manager;
}

int obj_manager_register(ObjManager *manager, obj_t *obj) {
    if (!manager || !obj) return -1;
    
    /* Check if we need to expand array */
    if (manager->object_count >= manager->object_capacity) {
        manager->object_capacity *= 2;
        manager->objects = (obj_t **)realloc(manager->objects,
                                              sizeof(obj_t *) * manager->object_capacity);
        if (!manager->objects) return -1;
    }
    
    /* Add object */
    manager->objects[manager->object_count] = obj;
    manager->object_count++;
    
    return 0;
}

int obj_manager_unregister(ObjManager *manager, obj_t *obj) {
    if (!manager || !obj) return -1;
    
    /* Find and remove object */
    for (int i = 0; i < manager->object_count; i++) {
        if (manager->objects[i] == obj) {
            /* Shift remaining objects down */
            for (int j = i; j < manager->object_count - 1; j++) {
                manager->objects[j] = manager->objects[j + 1];
            }
            manager->object_count--;
            return 0;
        }
    }
    
    return -1;  /* Not found */
}

obj_t* obj_manager_find(ObjManager *manager, const char *name) {
    if (!manager || !name) return NULL;
    
    for (int i = 0; i < manager->object_count; i++) {
        if (manager->objects[i] && strcmp(manager->objects[i]->name, name) == 0) {
            return manager->objects[i];
        }
    }
    
    return NULL;
}

void obj_manager_free(ObjManager *manager) {
    if (!manager) return;
    
    /* Free all objects */
    if (manager->objects) {
        for (int i = 0; i < manager->object_count; i++) {
            if (manager->objects[i]) {
                obj_free(manager->objects[i]);
            }
        }
        free(manager->objects);
    }
    
    free(manager);
    printf("[Object Manager] Freed\n");
}

/* ========== Utility Functions ========== */

void obj_print(obj_t *obj) {
    if (!obj) {
        printf("NULL object\n");
        return;
    }
    
    printf("Object: %s\n", obj->name);
    printf("  Prototype: %s\n", obj->proto ? obj->proto->name : "NULL");
    printf("  Properties: %d\n", obj->property_count);
    
    /* Print properties */
    if (obj->property_count > 0) {
        for (int i = 0; i < obj->property_capacity; i++) {
            ObjProperty *entry = obj->properties[i];
            while (entry) {
                char *value_str = vm_value_to_string(entry->value);
                printf("    %s: %s\n", entry->name, value_str);
                entry = entry->next;
            }
        }
    }
    
    printf("  Methods: %d\n", obj->method_count);
    for (int i = 0; i < obj->method_count; i++) {
        if (obj->methods[i]) {
            printf("    %s(%d params)\n", obj->methods[i]->name, obj->methods[i]->param_count);
        }
    }
    
    printf("  Ref count: %d\n", obj->ref_count);
    printf("  Destroyed: %s\n", obj->is_destroyed ? "yes" : "no");
}

int obj_get_property_count(obj_t *obj) {
    if (!obj) return 0;
    
    int count = obj->property_count;
    
    /* Add inherited properties */
    if (obj->proto) {
        count += obj_get_property_count(obj->proto);
    }
    
    return count;
}

int obj_get_method_count(obj_t *obj) {
    if (!obj) return 0;
    
    int count = obj->method_count;
    
    /* Add inherited methods */
    if (obj->proto) {
        count += obj_get_method_count(obj->proto);
    }
    
    return count;
}
