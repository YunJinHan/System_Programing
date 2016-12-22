#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main(void){

	int fd;
	fd = open("test",O_TRUNC);

	return 0;
}
