#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "proc-common.h"

#define SLEEP_PROC_SEC  10
#define SLEEP_TREE_SEC  3

/*
 * Create this process tree:
 * A-+-B---D
 *   `-C
 */
void fork_procs(void)
{
	/*
	 * initial process is A.
	 */
    int status;

	change_pname("A");
    printf("A: Process A created succesfully...\n");
	printf("A: Ready to create child B..\n");

    // Forking in order to create B
	pid_t p = fork();
	if (p < 0) {
		/* fork failed */
		perror("Forking failed");
		exit(1);
	}
	if (p == 0) {
		/*Child  B process */
		change_pname("B");
        printf("B: I was created succesfully...\n");
	    printf("B: Ready to create child D..\n");

        // Forking in order to create D
        p = fork();
        if (p < 0) {
            /* fork failed */
            perror("Forking failed");
            exit(1);
        }
        if (p == 0) {
            /*Child  D process */
            change_pname("D");
            printf("D: I was created succesfully...\n");
	        printf("D: Sleeping...\n");
	        sleep(SLEEP_PROC_SEC);
	        printf("D: Exiting...\n");
            exit(13);
        }
        p = wait(&status); //Node B waiting
	    explain_wait_status(p, status);
	    printf("B: Exiting...\n");
		exit(19);
	}
    // Forking in order to create C
    p = fork();
    if (p < 0) {
        /* fork failed */
        perror("Forking failed");
        exit(1);
    }
    if (p == 0) {
        /*Child  C process */
        change_pname("C");
        printf("C: I was created succesfully...\n");
        printf("C: Sleeping...\n");
        sleep(SLEEP_PROC_SEC);
        printf("C: Exiting...\n");
        exit(17);
    }

    //Wait for 2 children to terminate
    p = wait(&status);
	explain_wait_status(p, status);

    p = wait(&status);
	explain_wait_status(p, status);

	printf("A: Exiting...\n");
	exit(16);
}

/*
 * The initial process forks the root of the process tree,
 * waits for the process tree to be completely created,
 * then takes a photo of it using show_pstree().
 *
 * How to wait for the process tree to be ready?
 * In ask2-{fork, tree}:
 *      wait for a few seconds, hope for the best.
 * In ask2-signals:
 *      use wait_for_ready_children() to wait until
 *      the first process raises SIGSTOP.
 */
int main(void)
{
	pid_t pid;
	int status;

	/* Fork root of process tree */
    pid = fork();
    if (pid < 0) {
        perror("main: fork");
        exit(1);
    }
    if (pid == 0) {
        /* Child */
        fork_procs();
        exit(1);
    }
	/*
	 * Father
	 */
	/* for ask2-signals */
	/* wait_for_ready_children(1); */

	/* for ask2-{fork, tree} */
	sleep(SLEEP_TREE_SEC);

	/* Print the process tree root at pid */
	show_pstree(pid);

	/* for ask2-signals */
	/* kill(pid, SIGCONT); */

	/* Wait for the root of the process tree to terminate */
	pid = wait(&status);
	explain_wait_status(pid, status);
	return 0;
}
