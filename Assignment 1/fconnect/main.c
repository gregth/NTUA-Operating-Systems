#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include "func.h"

int main(int argc, char **argv) {
	int i;
	if (argc < 3 || argc > 4) {
        printf("Usage: .fconc inFile1 inFile2 [outFile (default:fconc.out)]\n");
        return 1;
    }

    //Open Input File 1
    int fd1 = open(argv[1], O_RDONLY);
    if (fd1 == -1) {
        perror("Error while opening inFile1");
        return 2;
    }

    //Open Input File 2
    int fd2 = open(argv[2], O_RDONLY);
    if (fd2 == -1) {
        perror("Error while opening inFile2");
        return 2;
    }

    int fd3;
    // Determine output file name
    if (argc == 4) {
        if (strcmp(argv[3], argv[1]) == 0
            || strcmp(argv[3], argv[2]) == 0) {
            printf("ERROR: OutFile name must be different than input file name.\n");
            return 2;
        }
        else {
            fd3 = open(argv[3], O_WRONLY | O_CREAT | O_TRUNC, 0666);
        }
    }
    else {
        if (strcmp("fconc.out", argv[1]) == 0
            || strcmp("fconc.out", argv[2]) == 0) {
            printf("ERROR: File fconc.out must not be used as input.\n");
            return 2;
        }
        else {
            fd3 = open("fconc.out", O_WRONLY | O_CREAT | O_TRUNC, 0666);
        }
    }

	if (fd3 == -1){
			perror("Error opening/creating the outFile");
			return 3;
	}

    //Write inFile1 to OutFile
    i = write_file(fd3, fd1);
    if (i == 1) {
            perror("Write inFile1 to outFile Failed.");
            return 4;
    }

    //Write inFile1 to OutFile
    i = write_file(fd3, fd2);
    if (i == 1){
            perror("Write inFile2 to outFile Failed.");
            return 4;
    }

    return 0;
}

