#include <stdio.h>
#include <unistd.h>
#include <string.h>

void zing(void) {
	char *s;
	s= getlogin();
	printf("And the user is %s\n", s);
}

