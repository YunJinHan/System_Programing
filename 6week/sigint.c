#include <signal.h>
#include <stdio.h>

void signal_handler(int signo){
	printf("CATCHINT : signo = %d\n",signo);
	printf("CATCHINT : returning\n\n");
}

int main(void){
	static struct sigaction act;
	act.sa_handler = signal_handler;
	sigfillset(&(act.sa_mask));
	sigaction(SIGINT,&act,NULL);
	printf("sleepcall #1\n"); sleep(1);
	printf("sleepcall #2\n"); sleep(1);
	printf("sleepcall #3\n"); sleep(1);
	printf("sleepcall #4\n"); sleep(1);
	printf("Exiting\n");
	return 0;
}
