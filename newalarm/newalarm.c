#include <unistd.h>
#include <signal.h>
#include <stdio.h>

void newalarm(int sec){
	pid_t pid = fork();
	if (pid == 0){
		sleep(sec);
		kill(getppid(),SIGALRM);
		raise(SIGKILL);
	}
}

void myalarm(void){
	printf("ding dong dang\n");
}

int main(void){
	int i = 0;
	printf("alaram setting\n");

	signal(SIGALRM,myalarm);

	newalarm(1);

	while (i < 5){
		printf("ok\n");
		pause();
		newalarm(2);
		i++;
	}

	return 0;
}