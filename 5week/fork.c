#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char **argv) {
	int pid;
	pid= fork();
	if (pid> 0) { // getpid() 현재프로세스의프로세스 ID를되돌려준다
		printf("Parent process  %d : %d\n", getpid(), pid);
		return 0;
	}
	else if (pid== 0) {
		printf("Child process %d\n", getpid());
		return 0;
	}
	else if (pid== -1) {
		perror("fork error : ");
		exit(0);
	}
	return 0;
}
