#include <stdio.h>
#include <sys/types.h> 
#include <sys/stat.h> 
#include <fcntl.h>

int main(void) {
	umask(044);
	if(creat("foo",S_IRUSR|S_IWUSR|S_IRGRP |S_IWGRP|S_IROTH|S_IWOTH) < 0) {
		perror("creat error for foo");
		exit(1); 
	}
	umask(S_IRGRP|S_IROTH);
	if(creat("poo", S_IRUSR|S_IWUSR|S_IRGRP |S_IWGRP|S_IROTH|S_IWOTH) <0){
		perror("creat error for poo");
		exit(1); 
	}
	return 0;
 }
