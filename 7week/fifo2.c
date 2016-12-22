#include <stdio.h>
#include <errno.h>
#include <fcntl.h>

#define MSGSIZE 80

char *fifo = "test7";

int main(int argc,char **argv){

	int fd;
	char msgbuf[MSGSIZE];
	bzero(msgbuf,MSGSIZE);

	if (mkfifo(fifo,0666) == -1){
		if (errno != EEXIST){
			perror("receiver : mkfifo");
		}
	}
	if ( (fd = open(fifo,O_RDWR)) < 0){
		perror("fifo open failed");
	}
	for (;;){
		if (read(fd,msgbuf,MSGSIZE) < 0){
			perror("message read failed");
		}
		printf("message received : %s\n",msgbuf);
	}
	return 0;
}
