#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <assert.h>

#include <sys/wait.h>
#include <sys/types.h>

#include "proc-common.h"
#include "request.h"
#include "helper.h"

/* Compile-time parameters. */
#define SCHED_TQ_SEC 2                /* time quantum */
#define TASK_NAME_SZ 60               /* maximum size for a task's name */

process_list* p_list;

/*
 * SIGALRM handler
 */
static void
sigalrm_handler(int signum)
{
	printf("Going to stop process with cpid: %d\n", p_list->head->cpid);
	// assert(0 && "Please fill me!");
	kill(p_list->head->gpid, SIGSTOP);
}

/*
 * SIGCHLD handler
 */
static void
sigchld_handler(int signum)
{
    int status;
    pid_t pid;
    //TODO Why wait -1?
    pid = waitpid(-1, &status, WUNTRACED);

    // Check if head process changed status
	if (pid == p_list->head->gpid) {
		process *p;

        // Process has stopped
		if (WIFSTOPPED(status)) {
			printf ("Process name: %s  Cpid: %d has been stopped.\n",
                    p_list->head->name, p_list->head->cpid);

			p = get_next(p_list);

        // Process has exited
		} else if (WIFEXITED(status)) {
			printf("Process name: %s  Cpid %d has been exited.\n",
                    p_list->head->name, p_list->head->cpid);

			p = pop(p_list);
			free_process(p);
			if (empty(p_list)) {
				printf ("Process list is now empty...\n");
				exit(0);
			}
			p = p_list->head;
		}
		else {
			printf("Process name: %s  Cpid %d has exited unexpectedly.\n",
                    p_list->head->name, p_list->head->cpid);

			p = pop(p_list);
			free_process(p);
			if (empty(p_list)) {
				printf ("Process list is now empty...\n");
				exit(0);
			}
			p = p_list->head;
		}

		printf("Next process name: %s Cpid %d.\n",
                p->name, p->cpid);

        // It's the turn of next process to continue
		kill (p->gpid, SIGCONT);
		alarm (SCHED_TQ_SEC);
	} else {
        /* Handle the case that a different than the head process
         * has changed status
         */
		printf("A process other than the head has changed status.\n");
		process *pr = erase_proc_by_pid(p_list, pid);

		free_process(pr);
	}
}

/* Install two signal handlers.
 * One for SIGCHLD, one for SIGALRM.
 * Make sure both signals are masked when one of them is running.
 */
static void
install_signal_handlers(void)
{
	sigset_t sigset;
	struct sigaction sa;

	sa.sa_flags = SA_RESTART;

    // Specify signals to be blocked while the handling funvtion runs
	sigemptyset(&sigset);
	sigaddset(&sigset, SIGCHLD);
	sa.sa_mask = sigset;
	sa.sa_handler = sigchld_handler;

	if (sigaction(SIGCHLD, &sa, NULL) < 0) {
		perror("sigaction: sigchld");
		exit(1);
	}

    // TODO In exercise the sa handler was reassigned, does it work?
	// sa.sa_handler = sigalrm_handler;
	struct sigaction salarm;
	sigset_t alarm_sigset;
	sigemptyset(&alarm_sigset);
	salarm.sa_flags = SA_RESTART;
	sigaddset(&alarm_sigset, SIGALRM);
	salarm.sa_mask = sigset;
	salarm.sa_handler = sigalrm_handler;

	if (sigaction(SIGALRM, &salarm, NULL) < 0) {
		perror("sigaction: sigalrm");
		exit(1);
	}

	/*
	 * Ignore SIGPIPE, so that write()s to pipes
	 * with no reader do not result in us being killed,
	 * and write() returns EPIPE instead.
	 */
	if (signal(SIGPIPE, SIG_IGN) < 0) {
		perror("signal: sigpipe");
		exit(1);
	}
}

int main(int argc, char *argv[])
{
	int nproc;
	/*
	 * For each of argv[1] to argv[argc - 1],
	 * create a new child process, add it to the process list.
	 */

	nproc = argc - 1; /* number of proccesses goes here */
	if (nproc == 0) {
		fprintf(stderr, "Scheduler: No tasks. Exiting...\n");
		exit(1);

	}

    p_list = initialize_empty_list();

    int i;
	for (i = 1; i < argc; i++) {
		pid_t pid;
		pid = fork();
		if (pid < 0) {
			perror("fork");
			exit(1);
		}
		if (pid == 0) {
            printf("test");
			raise(SIGSTOP);
			char filepath[TASK_NAME_SZ];
			sprintf(filepath, "./%s", argv[i]);
            // TODO
			char* args[] = {filepath, NULL};
			if (execvp(filepath, args)) {
				perror("execvp");
				exit(1);
			}
		}

		process *p = process_create((long)pid, argv[i]);
		push(p_list, p);
		printf("Process name: %s Cpid: %d is created.\n",
            argv[i], p->cpid);
	}

	/* Wait for all children to raise SIGSTOP before exec()ing. */
	wait_for_ready_children(nproc);

	/* Install SIGALRM and SIGCHLD handlers. */
	install_signal_handlers();

	printf("Scheduler dispatching the first process...\n");
	kill(p_list->head->gpid, SIGCONT);
	alarm(SCHED_TQ_SEC);

	/* loop forever  until we exit from inside a signal handler. */
	while (pause())
		;

	/* Unreachable */
	fprintf(stderr, "Internal error: Reached unreachable point\n");
	return 1;
}
