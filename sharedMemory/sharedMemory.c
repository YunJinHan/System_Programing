/*
	2012036901 윤진한
	실습과제 # 3
	sharedMemory.c
	2016.12.03
*/

#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h> 
#include <signal.h>

int shmid;

void child_handler(int);

int main(void)
{
	int status;
	int pid,*cal_num;
	void *shared_memory = (void *)0;

	shmid = shmget((key_t)1234,sizeof(int),0666|IPC_CREAT);

	if (shmid == -1)
	{
		perror("shmget failed: ");
		exit(0);
	}
	shared_memory = shmat(shmid,(void *)0,0);
	if (shared_memory == (void *)-1)
	{
		perror("shmat failed: ");
		exit(0);
	}
	cal_num = (int *)shared_memory;
	if ((pid = fork()) == 0)
	{
		signal(SIGINT,(void *)child_handler);
		// SIGINT 시그널 받는 경우 child_handler 함수 실행.
		*cal_num = 1;
		while (1)
		{
			*cal_num = *cal_num + 1;
			printf("[CHILD] %d\n",*cal_num);
			sleep(1);
		}
	} 
	else if (pid > 0)
	{

		signal(SIGINT,SIG_IGN);
		// SIGINT 시그널 받는 경우 해당 시그널을 무시함.
		while (1)
		{
			sleep(1);
			printf("[PARENT] %d\n",*cal_num);
			
			if (waitpid( pid, &status, WNOHANG) > 0)
			{
				exit(0);
			}
			// 자식이 종료되었는지 확인하고 종료 되지 않았으면 다음 일을 수행함.  
			// 종료상태 확인 시 자식이 종료되었다면 부모도 종료됨.
		}
	}
	return 0;
}

void child_handler(int signum)
{
	shmctl(shmid,IPC_RMID,NULL);
	// SharedMemory 삭제.
	exit(0);
}
