#include <stdio.h> 
#include <sys/types.h> 
#include <sys/stat.h>

int main(void) {
	struct stat statbuf;
	if (stat("foo", &statbuf) < 0) { 
		perror("stat error for foo");
		 exit(1);
	}
	if (chmod("foo",~S_IXUSR) < 0){
		perror("chmod error for foo");
		exit(1);
	}
	exit(0);
 }
