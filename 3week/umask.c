#include <sys/types.h> 
#include <sys/stat.h> 
#include <fcntl.h> 
#include <unistd.h> 
#include <stdlib.h> 
#include <stdio.h>

int main(void) {
	int fd;
	mode_t old_mask, new_mask;
	
	old_mask = umask(022);
	
	fd = open("test.txt",O_CREAT|O_WRONLY,0666);
	printf("old_mask=%o\n",old_mask);

	new_mask = umask(old_mask);
	printf("new_maks=%o\n",new_mask);
	
	close(fd);
	return 0;
}
