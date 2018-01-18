#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "helper.h"

process *process_create(pid_t pid, const char *name) {
    static int id = 0;
    process* new_process = (process*) malloc(sizeof(process));
    if (new_process == NULL) {
        printf("Memory allocation failed\n");
        exit(1);
    }

    // Strdup allocates memory and copies string
    new_process->name = strdup(name);
    new_process->pid = pid;
    new_process->id = id++;
    new_process->next = NULL;
    return new_process;
}

int empty (process_list* list) {
    if (list->head == NULL) {
        return 1;
    } else {
        return 0;
    }
}

process* pop(process_list* list) {
    if(empty(list)) {
        printf("Empty list\n");
        exit(1);
    }

    process* temp;
    if(list->head == list->tail) {
        temp = list->head;
        list->head =  NULL;
        list->tail = NULL;
        list->size = 0;
        return temp;
    } else {
        temp = list->head;
        list->head = list->head->next;
        list->tail->next = list->head;
        list->size--;
        return temp;
    }
}

int  push(process_list* l, process* new_p) {
    if (empty(l)) {
        l->head = l->tail = new_p;
        l->head->next = l->head;
        l->size = 1;
        return 1;
    }

    new_p->next = l->head;
    l->tail->next = new_p;
    l->tail = new_p;
    l->size++;
    return 1;
}

process * erase_proc_by_id(process_list * l, int id) {
    if (empty(l))
        return NULL;

    if (l->head->id == id)
        return pop(l);

    process* temp;
    temp = l->head;
    temp = temp->next;

    while (temp->next->id != id && temp->next != l->tail)
        temp = temp->next;

    if (temp->next->id == id) {
        process* ret = temp->next;
        l->size--;
        temp->next = ret->next;
        if(ret == l->tail)
            l->tail = temp;
        return ret;
    }

    return NULL;
}

process* erase_proc_by_pid(process_list * l, int id) {
    if (empty(l))
        return NULL;

    if (l->head->pid == id)
        return pop(l);

    process * temp;
    temp = l->head;

    while (temp->next->pid != id && temp->next != l->tail)
        temp = temp->next;

    if (temp->next->pid == id) {
        process * ret = temp->next;
        l->size--;
        temp->next = temp->next->next;
        if(ret == l->tail)
            l->tail = temp;
        return ret;
    }

    return NULL;
}

process* get_proc_by_pid(process_list * l, int id) {
    if (empty(l))
        return NULL;

    if (l->head->id == id)
        return l->head;

    process * temp;
    temp = l->head;

    while (temp->next->pid != id && temp->next != l->tail)
        temp = temp->next;

    if (temp->next->pid == id) {
        return temp->next;
    }

    return NULL;
}

process* get_proc_by_id(process_list * l, int id) {
    if (empty(l))
        return NULL;

    if (l->head->id == id)
        return l->head;

    process * temp;
    temp = l->head;

    while (temp->next->id != id && temp->next != l->tail)
        temp = temp->next;

    if (temp->next->id == id) {
        return temp->next;
    }

    return NULL;
}


process* get_next(process_list * l) {
    if(empty(l)) {
        printf("Empty list\n");
        exit(1);
    }

    l->tail = l->head;
    l->head = l->head->next;
    return l->head;
}

// version to be compatible with multiple lists
process* my_get_next(process_list * l) {
    if(empty(l)) {
        printf("Empty list\n");
        return NULL;
    }

    l->tail = l->head;
    l->head = l->head->next;
    return l->head;
}

void clear(process_list * l) {
    process * i;
    while(!empty(l)) {
        i = pop(l);
        free(i->name);
        free(i);
    }
}

process_list* initialize_empty_list(void) {
    // Initialize an empty list
    process_list* p_list;
    p_list = (process_list *) malloc (sizeof(process_list));
    p_list->head = NULL;
    p_list->tail = NULL;
    p_list->size = 0;
    return p_list;
}

void free_process(process* p) {
    free(p->name);
    free(p);
}

void print_list(process_list* l) {
    process* tmp = l->head;
    printf("\n\nNOW PRINTING THE LIST");
    while (tmp != l->tail) {
        printf("--> pid: %ld, id: %d,  name: %s\n", (long)tmp->pid, tmp->id, tmp->name);
        printf("TAIL: %d", l->tail->pid);
        tmp = tmp->next;
    }
    if (tmp != NULL) {
        printf("--> pid: %ld, id: %d,  name: %s\n", (long)tmp->pid, tmp->id, tmp->name);
    }
    printf("END OF THE LIST\n\n");
}

process* get_head_of_lists(process_list* l, process_list* h) {
    if (!empty(h)) {
        return h->head;
    } else {
        return l->head;
    }
}

int empty_lists(process_list* l, process_list* h) {
    return (empty(l) && empty(h));
}

process* get_next_lists(process_list* l, process_list* h) {
    if (!empty(h)) {
        process* p = my_get_next(h);
        return p;
    } else {
        return my_get_next(l);
    }
}

process* pop_list(process_list* l, process_list* h) {
    if (!empty(h)) {
        return pop(h);
    } else {
        return pop(l);
    }
}

process* get_proc_by_pid_list(process_list* l, process_list* h, int id) {
    process* res =  get_proc_by_pid(h, id);
    if (res == NULL) {
        res = get_proc_by_pid(l, id);
    }
    return res;
}

process* get_proc_by_id_list(process_list* l, process_list* h, int id) {
    process* res =  get_proc_by_id(h, id);
    if (res == NULL) {
        res = get_proc_by_id(l, id);
    }
    return res;
}

process* erase_proc_by_id_list(process_list* l, process_list* h, int id) {
    process* res =  erase_proc_by_id(h, id);
    if (res == NULL) {
        res = erase_proc_by_id(l, id);
    }
    return res;
}

process* erase_proc_by_pid_list(process_list* l, process_list* h, int id) {
    process* res =  erase_proc_by_pid(h, id);
    if (res == NULL) {
        res = erase_proc_by_pid(l, id);
    }
    return res;
}

int move_from_to(process_list* a, process_list* b, int id) {
    process* res = erase_proc_by_id(a, id);
    if (res == NULL) {
        return 0; // FAIL
    }
    int status = push(b, res);
    return status;
}

void red () {
      printf("\033[1;31m");
}

void yellow () {
      printf("\033[1;33m");
}

void green () {
      printf("\033[0;32m");
}

void reset () {
      printf("\033[0m");
}
