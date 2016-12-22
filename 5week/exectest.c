#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>

char *env_init[] = {"USER=unknown", "PATH=/tmp", NULL};

int main(void){
	pid_t pid;
	if ((pid= fork()) < 0) {
		perror("forkerror\n");
		exit(1);
	}
	else if (pid== 0) {/* specify pathname, specify environment */
		if (execle("/home/jinhan/Desktop/SystemPrograming/7/echoall", "echoall", "myarg1","MY AGR2", (char *) 0, env_init) < 0) {
			fprintf(stderr, "execl error\n");
			exit(1);
		}
	}
	if (waitpid(pid, NULL, 0) < 0) {
		perror("waiterror\n");
		exit(1);
	}
	else if (pid> 0) {/* specify filename, inhertenviroment*/
		if (execlp("echoall", "echoall", "only 1 arg",(char *) 0) < 0) {
			fprintf(stderr, "execlp error\n");
			exit(1);
		}
	}
	exit(0);
}
