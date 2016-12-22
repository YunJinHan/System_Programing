/*
	2012036901 윤 진 한
	컴퓨터공학과
	시스템프로그래밍 - 이론 과제 HW1
	< fcd.c >
	2016.10.27
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>

#define SIZE 1024

char absolute_path[SIZE];
// 현재 directory 의 절대경로를 저장함.
char find_directory[SIZE];
// 검색할 directory 이름을 저장.
char find_directory_path_tmp[SIZE][SIZE];
// find_directory 와 같은 이름을 가진 directory 의 상대경로 를 저장.
char find_directory_path[SIZE][SIZE];
// find_directory 와 같은 이름을 가진 directory 의 절대경로 를 저장.
int find_cnt;
// 현재 찾은 directory 의 갯수.

void myftw(char *name);
// 해당 directory 안에 있는 File 들을 출력해주는 함수.

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
	
	int choice;
	// 찾으려는 directory 가 여러개 존재할때 해당 directory 들 중 한 가지를 선택할때 쓰이는 변수.
	
	printf("> Enter the name of directory : ");

	while(1){

		find_cnt = 0; // find_cnt 초기화.
		memset(find_directory_path_tmp,0,SIZE*SIZE);
		memset(find_directory_path,0,SIZE*SIZE);
		memset(absolute_path,0,SIZE);
		memset(find_directory,0,SIZE);
		// absolute_path / find_directory / find_directory_path / find_directory_path_tmp 배열 초기화.
		getcwd(absolute_path,SIZE);
		// 현재 directory 의 절대 경로를 구해줌.
		scanf("%s",find_directory);
		// 현재 경로에서 입력받은 find_directory 를 찾음.

		if (!strcmp(find_directory,".")){
			break;
		}// "." 입력시 프로그램 종료.

		myftw("."); // 현재 경로에서 모든 하위 file 을 탐색함.

		for (int i=0;i<find_cnt;i++){
			strcat(find_directory_path[i],absolute_path);
			strcat(find_directory_path[i],"/");
			strcat(find_directory_path[i],find_directory_path_tmp[i]+2);
		}// 해당 찾은 directory 들의 상대경로를 절대경로로 바꿔 준다.

		if (find_cnt == 0) {
			printf("> there is no such directory\n");
			break;
		}// 찾는 directory 가 없을때. 
		else if (find_cnt == 1){
			choice = 1;
			printf("> %s>",find_directory_path[choice-1]);
		}// 찾는 directory 가 한개일때. 
		else {
			for (int i=0;i<find_cnt;i++){
				printf("> [%d] %s\n",i+1,find_directory_path[i]);
			}
			while (1) {
				printf("> Which directory do you want ? : ");
				scanf("%d",&choice);
				if (choice >=1 && choice <= find_cnt) break;
				printf("> wrong input. try again.\n");
			}// 범위 밖의 choice 를 하였을 경우 다시 입력받음.
			printf("> %s>",find_directory_path[choice-1]);
		}// 찾는 directory 가 여러개일때.
		
		if (chdir(find_directory_path[choice-1]) < 0 ){
			fprintf(stderr,"> changing directory error\n");
			exit(1);
		}// 선택한 directory 로 경로를 바꾸어줌.
	}
	execl("/bin/bash","/bin/bash",NULL);
	// bash shell 을 실행시킴.
	return 0;
}

void myftw(char *dir_name){
	DIR *dp;
	struct stat statbuf;
	struct dirent *dent;
	char tmp[SIZE],pathname[SIZE];
	char *token = NULL;
	
	if (access(dir_name,F_OK) == -1){
		fprintf(stderr,"> no directory name is provided\n");
		exit(1);
	}// 해당 file 존재 여부 확인.
	if (lstat(dir_name,&statbuf) < 0){
		fprintf(stderr,"> stat error\n");
		exit(1);
	}// 해당 file 의 정보를 가져옴.
	if (!S_ISDIR(statbuf.st_mode)){
		fprintf(stderr,"> %s is not directory\n",dir_name);
		exit(1);
	}// 해당 file 이 directory 인지 확인.
	if ((dp = opendir(dir_name)) == NULL){
		fprintf(stderr, "> opendir error\n");
		exit(1);
	}// 해당 directory file 을 open 하여 directory pointer 를 가져옴.

	while ((dent = readdir(dp)) != NULL){

		sprintf(pathname,"%s/%s",dir_name,dent->d_name);
		// 해당 file 의 상대경로를 저장함.
		if (lstat(pathname,&statbuf) < 0){
			exit(1);
		}// 해당 file 의 정보를 가져옴.

		if (S_ISDIR(statbuf.st_mode)){

			strcpy(tmp,pathname);
			// pathname 복사.
			strrev(tmp);
			token = strtok(tmp,"/");
			// 해당 File (directory) Name 만 추출.
	        if (strcmp(token,".") && strcmp(token,"..")){

				if (!strcmp(find_directory,dent->d_name)){
					strcpy(find_directory_path_tmp[find_cnt++],pathname);
				}// 현재 File 이 directory 이면서 찾는 directory 와 이름이 같을때 해당 directory 의 path 를 저장해둠.
				myftw(pathname);
	        }
	        // 해당 directory File Name 이 "." or ".." 가 아니면 해당 directory 를 탐색한다.
		}
		// 해당 File 이 directory 이면 myftw() 로 다시 해당 File, 즉 해당 하위 directory 를 탐색한다.
	}
	// 해당 directory 안에 존재하는 모든 File 을 검색함.
	closedir(dp);
	// 해당 directory pointer 를 닫는다.
}