#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdlib.h>

void filestatus(int fd){
	int accmode,val;
	if ((val = fcntl(fd,F_GETFL,0)) < 0){
		perror("fcntl error for id");
		exit(1);
	}
	printf("fd = %d : ",fd);
	accmode = val & O_ACCMODE;
	if (accmode == O_RDONLY){
		printf("read only");
	} else if (accmode == O_WRONLY){
		printf("write only");
	} else if (accmode == O_RDWR){
		printf("read/write");
	} else {
		fprintf(stderr,"unknown access mode");
		exit(1);
	}
	if (val & O_APPEND){
		printf(", append\n");
	}
}

int main(int argc,char *argv[]){
	filestatus(open(argv[1],O_RDWR));
	filestatus(open(argv[1],O_RDONLY));
	filestatus(open(argv[1],O_WRONLY|O_APPEND));
	return 0;
}
