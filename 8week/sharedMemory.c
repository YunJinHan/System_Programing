#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h> 

#include <signal.h>

int shmid;

int main(void){
	int pid,*cal_num;
	void *shared_memory = (void *)0;

	shmid = shmget((key_t)1234,sizeof(int),0666|IPC_CREAT);

	if (shmid == -1){
		perror("shmget failed: ");
		exit(0);
	}

	shared_memory = shmat(shmid,(void *)0,0);
	if (shared_memory == (void *)-1){
		perror("shmat failed: ");
		exit(0);
	}
	cal_num = (int *)shared_memory;
	if ((pid = fork()) == 0){
		*cal_num = 1;
		while (1){
			*cal_num = *cal_num + 1;
			printf("[CHILD] %d\n",*cal_num);
			sleep(1);
		}
	} else if (pid > 0){
		while (1){
			sleep(1);
			printf("[PARENT] %d\n",*cal_num);
		}
	}
	return 0;
}