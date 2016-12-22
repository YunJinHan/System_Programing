#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int glob = 6;
char buf[] = "a write to stdout\n";

int main(void){
	int var;
	pid_t pid;
	var = 88;
	if (write(STDOUT_FILENO, buf,sizeof(buf)-1) != sizeof(buf)-1) {
		perror("write error");
		exit(1);
	}
	printf("before fork\n");
	/* we don't flush stdout*/
	if ((pid= fork()) < 0) {
		perror("fork error");
		exit(1);
	}
	else if (pid== 0) { /* child */
		glob++; /* modify variables */
		var++;
		printf("child pid= %d, glob = %d, var=%d\n", getpid(), glob, var);
	} else {
		sleep(2); /* parent */
		printf("parent pid= %d, glob = %d, var=%d\n", getpid(), glob, var);
	}
	exit(0);
}
