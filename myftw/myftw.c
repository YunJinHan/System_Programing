/*
	2012036901 윤 진 한
	컴퓨터공학과
	시스템프로그래밍 - 실습 과제 HW1
	< myftw.c >
	2016.10.26
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>

#define PRINT_TAP for (int i=0;i<cnt;i++) printf("     ");
#define SIZE 1024

int cnt = 0;
// 현재 몇번이나 Sub Directory 로 들어갔는지 알려줌.

void access_perm(char *perm,mode_t mode);
// 해당 File permission check 함수.

void myftw(char *name);
// 해당 Directory 안에 있는 File 들을 출력해주는 함수.

void strrev(char *word){
	int len = strlen(word);
	char tmp;

	for (int i=0;i<len/2;i++){
		tmp = *(word+i);
		*(word+i) = *(word+len-i-1);
		*(word+len-i-1) = tmp;
	}
}
// "." or ".." File 을 check 하기 위하여 사용한 문자열 역순으로 바꿔주는 함수.

int main(int argc,char **argv){
	printf("================== < START > ==================\n");
	myftw(argv[1]);
	printf("==================  < END >  ==================\n");
	return 0;
}

void access_perm(char *perm,mode_t mode){
	int i;
	char permchar[] = "rwx";
	memset(perm,'-',10);
	perm[10] = '\0';

	if (S_ISDIR(mode)) perm[0] = 'd';
	else if (S_ISCHR(mode)) perm[0] = 'c';
	else if (S_ISBLK(mode)) perm[0] = 'b';
	else if (S_ISFIFO(mode)) perm[0] = 'p';
	else if (S_ISLNK(mode)) perm[0] = 'l';
	// 해당 File 이 어떤 종류의 File 인지 check.

	for (i=0;i<9;i++){
		if ((mode << i) & 0x100){
			perm[i+1] = permchar[i%3];
		}
	}
	// 해당 File 의 권한 check.
}

void myftw(char *dir_name){
	DIR *dp;
	struct stat statbuf;
	struct dirent *dent;
	char perm[11],pathname[SIZE],tmp[SIZE];
	char *token = NULL;
	
	if (access(dir_name,F_OK) == -1){
		fprintf(stderr,"no directory name is provided\n");
		exit(1);
	}// 해당 file 존재 여부 확인.
	if (lstat(dir_name,&statbuf) < 0){
		fprintf(stderr,"stat error\n");
		exit(1);
	}// 해당 file 의 정보를 가져옴.
	if (!S_ISDIR(statbuf.st_mode)){
		fprintf(stderr,"%s is not directory\n",dir_name);
		exit(1);
	}// 해당 file 이 directory 인지 확인.
	if ((dp = opendir(dir_name)) == NULL){
		fprintf(stderr, "opendir error\n");
		exit(1);
	}// 해당 directory file 을 open 하여 directory pointer 를 가져옴.

    PRINT_TAP
	printf("--- Lists of (%s) Directory ---\n",dir_name);
	
	while ((dent = readdir(dp)) != NULL){
		sprintf(pathname,"%s/%s",dir_name,dent->d_name);
		// 해당 file 의 상대경로를 저장함.
		if (lstat(pathname,&statbuf) < 0){
			exit(1);
		}// 해당 file 의 정보를 가져옴.
		access_perm(perm,statbuf.st_mode);
		// 해당 File 의 permission 을 불러와 perm 배열에 저장함.
		PRINT_TAP
		printf("%s %8lld %s\n",perm,statbuf.st_size,dent->d_name);
		// 해당 File Permission 및 File Name 출력.

		if (S_ISDIR(statbuf.st_mode)){
			strcpy(tmp,pathname);
			// pathname 복사.
			strrev(tmp);
			token = strtok(tmp,"/");
			// 해당 File (Directory) Name 만 추출.
	        if (strcmp(token,".") && strcmp(token,"..")){
	        	cnt++;
	        	PRINT_TAP
	        	printf("================== < START > ==================\n");
	        	PRINT_TAP
				printf("Sub_Directory Count : %d\n",cnt);
				myftw(pathname);
				PRINT_TAP
				printf("==================  < END >  ==================\n");
				cnt--;
	        }
	        // 해당 Directory File Name 이 "." or ".." 가 아니면 해당 Directory 를 탐색한다.
		}
		// 해당 File 이 Directory 이면 myftw() 로 다시 해당 File, 즉 해당 Directory 를 탐색한다.
	}
	// 해당 Directory 안에 존재하는 모든 File 을 검색함.
	closedir(dp);
	// 해당 Directory pointer 를 닫는다.
}