#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#define BSIZE 1024
#define FPERM 0644

void copy(char *src,char *dest){
	int fd1,fd2,n;
	char buf[BSIZE];

	if ((fd1 = open(src,O_RDONLY)) < 0){
		perror("file open error");
		exit(1);
	}
	if ((fd2 = open(dest,O_CREAT|O_WRONLY|O_TRUNC,FPERM)) < 0){
		perror("file creation error");
		exit(1);
	}
	while ((n = read(fd1,buf,BSIZE)) > 0){
		write(fd2,buf,n);
	}
	close(fd1);
	close(fd2);
	printf("Program Exit\n");
}

int main(int argc,char *argv[]){
	if (argc == 3){
		printf("Program starts!\n");
		copy(argv[1],argv[2]);
	} else {
		printf("Program Error!\n");
	}
	return 0;
}
