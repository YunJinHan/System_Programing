#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>

int main(void){
	int fd;
	char buf1[] = "abcde";
	char buf2[] = "ABCDE";

	if ((fd = creat("file.hole",0640)) < 0 ){
		perror("create error");
		exit(1);
	}
	if (write(fd,buf1,5) != 5){
		perror("buf1 write error");
		exit(1);
	}
	lseek(fd,35,SEEK_SET);
	/*if (lseek(fd,35,SEEK_SET) == -1){
		perror("seek error");
		exit(1);
	}
	if (write(fd,buf2,5) != 5){
		perror("buf2 write error");
		exit(1);
	}
	*/
	return 0;
}
