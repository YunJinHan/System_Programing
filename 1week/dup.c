#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define BSIZE 80

int main(void){
	int fd,newfd,n;
	char buf1[BSIZE],buf2[BSIZE];
	memset(buf2,'k',BSIZE);
	fd = open("test",O_RDONLY);
	
	n = read(fd,buf1,BSIZE);
	write(STDOUT_FILENO,buf1,n);

	n = read(newfd,buf2,BSIZE);
	write(STDOUT_FILENO,buf2,n);
	
	close(fd);
	
	n = read(newfd,buf2,BSIZE);
	write(STDOUT_FILENO,buf2,n);	
	close(newfd);
	return 0;
}
