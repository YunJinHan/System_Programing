#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
	if (access(argv[1], F_OK) != 0){
		perror("File not exist");
	} else {
		printf("File exist\n");
	}
	return 0;
}

