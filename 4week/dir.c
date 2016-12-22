#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc,char **argv){
	DIR* dirptr;
	struct dirent* dir;
	int i;
	if ((dirptr = opendir(argv[1])) == NULL){
		printf("No such a dir\n");
		exit(1);
	}
	for (i=0;i<2;i++){
		while (dir=readdir(dirptr)){
			if (dir->d_ino !=0){
				printf("%s\n",dir->d_name);
			}
		}
		rewinddir(dirptr);
		printf("============\n");
	}
	closedir(dirptr);
	return 0;
}
