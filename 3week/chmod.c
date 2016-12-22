#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>

int main(int argc,char **argv){
	if (chmod(argv[1],S_IRUSR | S_IWUSR) < 0){
		perror("chmod error");
		exit(0);
	}
	return 0;
}
