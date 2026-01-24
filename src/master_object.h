#ifndef MASTER_OBJECT_H
#define MASTER_OBJECT_H

#include "compiler.h"
#include "vm.h"

int master_object_init(const char *master_path, VirtualMachine *vm);
VMValue master_call(const char *func_name, VMValue *args, int arg_count);
void master_object_free(void);
Program* master_get_program(void);
int master_is_loaded(void);

#endif

#include "compiler.h"
#include "vm.h"

int master_object_init(const char *master_path, VirtualMachine *vm);
VMValue master_call(const char *func_name, VMValue *args, int arg_count);
void master_object_free(void);
Program* master_get_program(void);
int master_is_loaded(void);

#ifndef MASTER_OBJECT_H
#define MASTER_OBJECT_H

#include "compiler.h"
#include "vm.h"

int master_object_init(const char *master_path, VirtualMachine *vm);
VMValue master_call(const char *func_name, VMValue *args, int arg_count);
void master_object_free(void);
Program* master_get_program(void);
int master_is_loaded(void);

#endif
