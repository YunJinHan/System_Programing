#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include <string.h>

off_t getFileSize(int fd){
	off_t filesize = lseek(fd,(off_t)0,SEEK_END);
	lseek(fd,(off_t)0,SEEK_SET);
	return filesize;
}

void writeFile(int fd1,int fd2){
	long filesize = getFileSize(fd2);
	char *filedata = (char *)malloc(sizeof(char)*filesize);
	bzero(filedata,filesize);
	read(fd2,filedata,filesize);
	lseek(fd1,(off_t)0,SEEK_END);
	write(fd1,filedata,filesize);	
}

int main(int argc,char *argv[]){
	int fd1,fd2;
	if (strcmp(argv[1],"1") == 0){
		if ((fd1 = open(argv[2],O_RDONLY)) < 0){
			perror("file open error");
			exit(1);
		}
		printf("%s size : %ld\n",argv[2],getFileSize(fd1));
	} else if (strcmp(argv[1],"2") == 0){
		if ((fd1 = open(argv[2],O_RDWR)) < 0){
			perror("file open error");
			exit(1);
		}
		if ((fd2 = open(argv[3],O_RDWR)) < 0){
			perror("file open error");
			exit(1);
		}
		printf("Before %s size : %ld\n",argv[2],getFileSize(fd1));
		writeFile(fd1,fd2);
		printf("After %s size : %ld\n",argv[2],getFileSize(fd1));
	}
	return 0;
}
