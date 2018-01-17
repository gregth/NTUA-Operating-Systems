#ifndef helper_h
#define helper_h

#include <stddef.h>
typedef struct process_type {
    int gpid;
    int cpid;
    char * name;
    struct process_type * next;
} process;

typedef struct process_list_type {
    process * head;
    process * tail;
    size_t size;
} process_list;

process * process_create(int pid, const char * name);

int empty(process_list * l);

process * pop(process_list * l);

void push(process_list * l, process * n);

process * erase_proc_by_id(process_list * l, int id);

process * erase_proc_by_pid(process_list * l, int id);

process * next_p(process_list *l);

void clear(process_list * l);

#endif
