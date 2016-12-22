#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>

// fork 2번 수행 -> process 4개 생성

int main(void){
	int fdes,pid,cflag,excode;
	long pos;
	fdes = open("openf.c",O_RDONLY);
	printf("fdes = %d\n",fdes);
	pos = lseek(fdes,20l,SEEK_SET);
	printf("Current postion before fork() is %ld\n",pos);

	if (!fork()){
		pos = lseek(fdes,40l,SEEK_SET);
		printf("Current postion in child after fork() is %ld\n",pos);
	}
	else {
		wait((int*)0);
		pos = lseek(fdes,0l,SEEK_CUR);
		printf("Current postion in parent after fork() is %ld\n",pos);
	}

	if (!fork()){
		execl("./openfexec",0);
		printf("It is an error to print this line out\n");
	}

	wait(&excode);
	pos = lseek(fdes,0l,SEEK_CUR);
	printf("Current pos in parent after exec() is %ld\n",pos);
	printf("Exitcode of a child = %d\n",WEXITSTATUS(excode));
	cflag = fcntl(fdes,F_GETFD,0);
	printf("close-on-exec flag = %d\n",cflag);
	fcntl(fdes,F_SETFD,1);

	if (pid = fork()){
		waitpid(pid,&excode,0);
		printf("Exitcode of a specific child = %d\n",WEXITSTATUS(excode));
		exit(0);
	}
	execl("./openfexec",0);
	printf("It is an error to print this line out\n");
}