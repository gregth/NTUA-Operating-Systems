#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

#include "tree.h"
#include "proc-common.h"

#define SLEEP_PROC_SEC  10
#define SLEEP_TREE_SEC  3

void fork_procs(struct tree_node *root, int write_fd) {
    char *name = root->name;
	change_pname(name);
    printf("PID: %ld (%s): I was created succesfully...\n",
        (long)getpid(), root->name);
    //If process is a leaf node
    if (root->nr_children == 0) {
        int value = atoi(root->name);
        if(write(write_fd, &value, sizeof(value)) != sizeof(value)) {
            perror("Pipe Write Failed\n");
            exit(1);
        }
        printf("PID: %ld (%s): Wrote Value:%d on Pipe %d and is Exiting...\n",
            (long)getpid(), name, value, write_fd);
        sleep(SLEEP_PROC_SEC);
        exit(getpid());
    }

    pid_t p;
    // Iterate for every child
    // If node is not a leaf
    int i;
    int fd[2];
    if (pipe(fd) < 0) {
        perror("Pipe Creation Failed");
        exit(1);
    }
    printf("PID: %ld (%s): Pipe [%d, %d] to children created successfully\n",
            (long)getpid(), root->name, fd[0], fd[1]);

    for (i = 0; i < 2; i++) {
        printf("PID: %ld (%s): Ready to create child %s...\n",
                (long)getpid(), name, (root->children + i)->name);
        p = fork();
        if (p < 0) {
            /* fork failed */
            perror("Forking failed");
            exit(1);
        }

        if (p == 0) {
            /*Child  process */
            fork_procs(root->children + i, fd[1]);
            printf("\nWTF\n");
        }
    }

    int operands[2];
    int value;

    for (i=0; i < 2; i++) {
        if (read(fd[0], &value, sizeof(value)) != sizeof(value)) {
            perror("Read from Pipe Failed");
            exit(1);
        }
        printf("PID: %ld (%s): Reading from pipe value no. %d : %d\n",
            (long)getpid(), root->name, i, value);
        operands[i] = value;

        // p = wait(&status); //Node B waiting
        //explain_wait_status(p, status);
    }
    int result;

    if (strcmp(root->name, "+") == 0)
        result = operands[0] + operands[1];
    else
        result = operands[0] * operands[1];
    if (write(write_fd, &result, sizeof(result)) != sizeof(result)) {
        perror("Pipe Write Failed");
        exit(1);
    }
    printf("PID: %ld (%s): Wrote calculated result (%d) to Pipe %d\n",
            (long)getpid(), name, result, write_fd);
    sleep(SLEEP_PROC_SEC);
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
    int fd[2];

    if (pipe(fd) < 0) {
        perror("Pipe Creation Failed");
        exit(1);
    }
	/* Fork root of process tree */
    pid = fork();
    if (pid < 0) {
        perror("main: fork");
        exit(1);
    }
    if (pid == 0) {
        /* Child */
        fork_procs(root, fd[1]);
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

    int final_value;
	/* for ask2-signals */
	/* kill(pid, SIGCONT); */
    if(read(fd[0], &final_value, sizeof(final_value))
            != sizeof(final_value)) {
        perror("Pipe Read Failed");
        exit(1);
    }
    printf("The result is %d\n", final_value);
    return 0;
}
