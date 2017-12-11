#ifndef FUNC_H__
#define FUNC_H__

#define BUFF_SIZE 1024

void doWrite(int fd, char buff[], int len);

int write_file(int out_fd, int in_fd);

#endif
