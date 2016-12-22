#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>

#define MSGSIZE 80

char *fifo = "test7";

int main(int argc,char **argv){
	int fd,j,nwrite;
	char msgbuf[MSGSIZE];
	if (argc < 2){
		fprintf(stderr,"Usage : send message msg..\n");
		exit(1);
	}
	if ((fd=open(fifo,O_WRONLY )) < 0){
		perror("fifo open failed");
	}
	for (j = 1;j <argc;j++){
		if (strlen(argv[j]) > MSGSIZE){
			fprintf(stderr,"message too long %s\n",argv[j]);
			continue;
		}
		strcpy(msgbuf,argv[j]);
		if ((nwrite = write(fd,msgbuf,MSGSIZE)) == -1){
			perror("message write failed");
		}
	}
	exit(0);
}
