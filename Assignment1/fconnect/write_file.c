#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "func.h"

// Tries to write the contents of in_fd to out_fd.
// Return 0 on success, 1 on failure.
int write_file(int out_fd, int in_fd) {
    char buff[BUFF_SIZE];
    ssize_t rcnt;
    for (;;){
        rcnt = read(in_fd, buff, BUFF_SIZE - 1);
        if (rcnt == 0) /* End-of-file */
            break;
        if (rcnt == -1){ /* error */
            perror("Error while reading input file");
            return 1;
        }
        doWrite(out_fd, buff, rcnt);
    }
    return 0;
}

