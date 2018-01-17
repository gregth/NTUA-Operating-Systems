#ifndef helper_h
#define helper_h

#include <unistd.h>
#include <stddef.h>
typedef struct process_type {
    pid_t gpid;
    int cpid;
    char * name;
    struct process_type * next;
} process;

typedef struct process_list_type {
    process * head;
    process * tail;
    size_t size;
} process_list;

process * process_create(pid_t pid, const char * name);

int empty(process_list * l);

process * pop(process_list * l);

void push(process_list * l, process * n);

process * erase_proc_by_id(process_list * l, int id);

process * erase_proc_by_pid(process_list * l, int id);

process * get_next(process_list *l);

void clear(process_list * l);

process_list* initialize_empty_list(void);

void free_process(process* p);

void print_list(process_list* l);
#endif
