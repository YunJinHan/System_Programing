#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

int main(int argc,char **argv){
	if (mkdir(argv[1],0755)){
		perror("mkdir error");
		exit(1);
	}
	return 0;
}
