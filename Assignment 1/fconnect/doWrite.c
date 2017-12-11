#include <stdio.h>
#include <unistd.h>

//Writes the buffer from index 0 till length to
//the output file descriptor fd.
int doWrite(int fd, char buff[], int len){
    ssize_t wcnt;
    ssize_t idx = 0;
    do {
        wcnt = write(fd, buff + idx, len - idx);
        if (wcnt == -1) { //error
            perror("write");
            return 1;
        }
        idx += wcnt;
    } while (idx < len);
    return 0;
}

