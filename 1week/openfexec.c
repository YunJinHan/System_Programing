#include <stdio.h>

int main(void){
	int fd = 3;
	long pos = lseek(fd,0l,SEEK_CUR);
	printf("\tPosin openfexec(): is %ld\n",pos);
	pos = lseek(fd,50l,SEEK_CUR);
	printf("\tNewpos after lseek() in openfexec() is %ld\n",pos);
	exit(pos < 0 ? !0 : 0);
}