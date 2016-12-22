#include <unistd.h> 
#include <stdio.h> 
#include <stdlib.h>

int main(int argc, char **argv) {
	if ( argc != 3) {
		printf ("Usage: ./link [original file] [copy file]\n");
		return -1; 
	}
	if(access(argv[1], F_OK) != 0) {
		printf("original file not exists\n");
		return -1; 
	}
	if(access(argv[2], F_OK) == 0) {
	printf("file already exists \n");
	return -1;
	 }
	if (link(argv[1], argv[2]) == -1) perror("link error ");
	return 0;
}
