#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "helper.h"

process *process_create(long pid, const char *name) {
	static int id = 0;
	process* new_process = (process*) malloc(sizeof(process));
	if (new_process == NULL) {
		printf("Memory allocation failed\n");
		exit(1);
	}

	// Strdup allocates memory and copies string
	new_process->name = strdup(name);
	new_process->gpid = pid;
	new_process->cpid = id++;
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

void push(process_list* l, process* new_p) {
	if (empty(l)) {
		l->head = l->tail = new_p;
		l->head->next = l->head;
		l->size = 1;
		return;
	}

	new_p->next = l->head;
	l->tail->next = new_p;
	l->tail = new_p;
	l->size++;
	return;
}

process * erase_proc_by_id(process_list * l, int id) {
	if (empty(l))
		return NULL;

	if (l->head->cpid == id)
		return pop(l);

	process* temp;
	temp = l->head;
	temp = temp->next;

	while (temp->cpid != id && temp != l->tail)
		 temp = temp->next;

	if (temp->cpid == id) {
		process * ret = temp;
		l->size--;
		temp = temp->next;
		if(ret == l->tail)
			 l->tail = temp;
		return ret;
	}

	return NULL;
}

process* erase_proc_by_pid(process_list * l, int id) {
	if (empty(l))
		return NULL;

	if (l->head->gpid == id)
		 return pop(l);

	process * temp;
	temp = l->head;

	while (temp->next->gpid != id && temp->next != l->tail)
		 temp = temp->next;

	if (temp->next->gpid == id) {
		process * ret = temp->next;
		l->size--;
		temp->next = temp->next->next;
		if(ret == l->tail)
			 l->tail = temp;
		return ret;
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
    printf("\n\nNOW PRINTING THE LIST\n");
    while (tmp != NULL) {
        printf("--> pid: %ld, name: %s", tmp->gpid, tmp->name);
        tmp = tmp->next;
    }
}