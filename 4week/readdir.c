#include <sys/types.h>
#include <dirent.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdlib.h>
#include <stdio.h>

int main(int argc,char **argv){
	char buf[255];
	if (chdir(argv[1])){
		perror("error");
		exit(1);
	}
	getcwd(buf,255);
	printf("Current Directory : %s\n",buf);
	exit(0);
}
