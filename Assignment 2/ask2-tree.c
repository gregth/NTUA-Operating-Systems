#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "tree.h"
#include "proc-common.h"

#define SLEEP_PROC_SEC  10
#define SLEEP_TREE_SEC  3

void fork_procs(struct tree_node *root) {
    char *name = root->name;
	change_pname(name);
    int status;

    //If process is a leaf node
    if (root->nr_children == 0) {
        printf("%s: Sleeping...\n", name);
        sleep(SLEEP_PROC_SEC);
        printf("%s: Exiting...\n", name);
        exit(getpid());
    }

    pid_t p;
    // Iterate for every child
    int i;
    for (i=0; i < root->nr_children; i++) {
        printf("%s: Ready to create child %s...\n", name,
                (root->children + i)->name);
        p = fork();
        if (p < 0) {
            /* fork failed */
            perror("Forking failed");
            exit(1);
        }

        if (p == 0) {
            /*Child  process */
            printf("%s: I was created succesfully...\n",
                    (root->children + i)->name);
            fork_procs(root->children + i);
        }
    }
    for (i=0; i < root->nr_children; i++) {
        p = wait(&status); //Node B waiting
        explain_wait_status(p, status);
    }
    printf("%s: Exiting...\n", name);
    exit(getpid());
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
int main(int argc, char *argv[])
{
	struct tree_node *root;

	if (argc != 2) {
		fprintf(stderr, "Usage: %s <input_tree_file>\n\n", argv[0]);
		exit(1);
	}

	root = get_tree_from_file(argv[1]);
	print_tree(root);

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
        fork_procs(root);
        exit(getpid());
    }
	/*
	 * Father
	 */
	/* for ask2-signals */
	/* wait_for_ready_children(1); */

	/* for ask2-{fork, tree} */
	sleep(SLEEP_TREE_SEC);

	/* Print the process tree root at pid */
	show_pstree(getpid());

	/* for ask2-signals */
	/* kill(pid, SIGCONT); */

	/* Wait for the root of the process tree to terminate */
	pid = wait(&status);
	explain_wait_status(pid, status);
	return 0;
}
