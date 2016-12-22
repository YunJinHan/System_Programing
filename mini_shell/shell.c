/*
	2012036901 윤 진 한
	컴퓨터공학과
	시스템프로그래밍 - 이론 과제 HW2
	< Simple Linux Shell - smsh.c >
	2016.12.01
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <ctype.h>
#include <errno.h>

#define INPUT       STDIN_FILENO
#define OUTPUT      STDOUT_FILENO

#define HISTORYSIZE 10
#define INPUTSIZE   1024
#define INPUTSIZE2  256
#define ARGSIZE     32
#define COMMANDCNT  64
#define PATHSIZE    256
#define TRUE        1
#define FALSE       0

typedef unsigned char bool;

struct sigaction new;						// background process signal handler 함수.

char commandLine[INPUTSIZE];				// read 로 받은 명령어.
char commandSemi[COMMANDCNT][INPUTSIZE];	// commandLine 을 ";" 으로 파싱.
char commandBg[COMMANDCNT][INPUTSIZE2];		// commandLine 을 각각 Background 로 파싱.
char commandLineArg[COMMANDCNT][ARGSIZE];	// commandLine 을 각각 Argument 로 파싱.
char history[HISTORYSIZE][INPUTSIZE];		// 이전 검색 내용 저장.
char currentPath[PATHSIZE];					// 현재 경로저장.
char tmpBuffer[INPUTSIZE];					// 파싱 할때 쓰이는 공동변수.
int backgroundPid[INPUTSIZE2];				// background process pid 저장.
int backgroundCnt;							// background process count.
bool background;							// background 실행 여부.

void mainLoopStart(void);					// shell loop 함수.
bool parseCommandSemicolon(int *); 			// ";" 파싱 함수.
void parseCommandArg(char *,int *);			// command 옵션 단위 파싱 함수.
void parseBackground(char *,int *,bool *);	// background 파싱 함수.
void printCurrentPath(void); 				// 현재 경로 출력 함수.
void insertHistory(int *); 					// history 에 현재 commandLine 삽입 함수.
void showHistory(int *);					// history 출력 함수.
void processCommand(char *);				// command 를 실행시켜주는 함수.
void changeDirectory(int);					// Directory 를 변경시켜주는 함수.
void inoutFileRedirection(int);				// ">"/"<"/">>"/">!" redirection 처리 함수.
void pipeFunction(void);					// pipe 실행 함수.
void multipipeFunction(int);				// multipipe 실행 함수.
void backgroundDone(void);					// background process 종료.
void handler(int);							// background process 종료하기 위한 시그널 핸들러.
void error(char *s);						// error 발생시 실행 함수.
char *lightTrim(char *); 					// 문자열 좌측 공백 제거 함수.
char *rightTrim(char *); 					// 문자열 우측 공백 제거 함수.
char *trim (char *); 						// 문자열 좌우 공백 모두 제거 함수.
void deleteSpaceOrTab(char *,char);			// 문장에서 Space 나 Tab 을 제거하는 함수.

int main(void){

	new.sa_handler = handler;
	new.sa_flags = SA_NOCLDWAIT;

	sigemptyset(&new.sa_mask);

	sigaction(SIGCHLD,&new,NULL);
	// backgroud child process 가 SIGCHLD 를 보냈을때 new sigaction 동작.
	mainLoopStart();
	// mainLoopStart 함수 실행.
	return 0;
}

void mainLoopStart(void){

	int n,historyCnt = -1;
	int commandLineSemiCnt,cnt,cnt2;
	bool isLastBackground = FALSE;

	/*----- start of while shell loop. -----*/
	while (printCurrentPath(),read(INPUT, commandLine, INPUTSIZE)) {
		
		if ((n==1 && commandLine[0] == '\n') || (n==2 && commandLine[0] == '\t')) continue;
		// 입력 받은 문자가 엔터나 tab 인 경우.
		commandLine[strlen(commandLine) - 1] = '\0';
		// read 로 입력 받을시 마지막에 '\n' 이 입력되어 null 값으로 바꾸어줌.
		commandLineSemiCnt = 0,cnt = 0;
		insertHistory(&historyCnt);
		// 입력받은 명령어는 Queue 형태의 history 에 저장해줌.
		char *buffer = (char *)malloc(sizeof(char)*INPUTSIZE);
		bzero(buffer,INPUTSIZE);
		strcpy(buffer,commandLine);
		deleteSpaceOrTab(buffer,' ');
		if (!strlen(buffer)) {
			free(buffer);
			continue;
			// 입력받은 문자에 스페이스만 있는 경우.
		} else {
			free(buffer);
		}
		if (!parseCommandSemicolon(&commandLineSemiCnt)){
			bzero(commandLine,INPUTSIZE);
			bzero(commandSemi,COMMANDCNT*INPUTSIZE);
			continue;
		} // 입력받은 명령어를 ';' 으로 파싱함.
		if (!commandLineSemiCnt){
			commandLineSemiCnt++;
			strcpy(commandSemi[0],commandLine);
		}
		while (commandLineSemiCnt--){
			if (!strcmp(commandSemi[cnt],"exit")) exit(0);
			if (!strcmp(commandSemi[cnt],"clear")) printf("\033[2J\033[1H");
			if (!strcmp(commandSemi[cnt],"history")) showHistory(&historyCnt);
			else {
				cnt2 = 0;
				background = FALSE;
				isLastBackground = FALSE;
				bzero(commandBg,COMMANDCNT*INPUTSIZE2);
				parseBackground(commandSemi[cnt],&cnt2,&isLastBackground);
				// 각각 '&' 실행인지 확인하고 명령어를 실행할 준비를 한다.
				if (cnt2 <= 1){
					processCommand(commandSemi[cnt]); // 백그라운드가 아니거나 백그라운드 명령어가 한개일때.
				} else {
					for (int i = 0; i < cnt2; i ++){
						if (i == cnt2 - 1 && !isLastBackground) background = FALSE;
						processCommand(commandBg[i]);
					}
				} // 백그라운드 명령어가 두개 이상인 경우.
			}
			cnt++;
			bzero(commandLineArg,COMMANDCNT*ARGSIZE);
		}
		bzero(commandLine,INPUTSIZE);
		bzero(commandSemi,COMMANDCNT*INPUTSIZE);
		backgroundDone();
	}
	/*----- end of while shell loop. -----*/
}

bool parseCommandSemicolon(int *commandLineSemiCnt){
	bool successParsing = TRUE;
	bool lastSemi = FALSE;
	char *tmp = (char *)malloc(sizeof(char)*INPUTSIZE);
	bzero(tmp,INPUTSIZE);
	strcpy(tmp,commandLine);

	if (strchr(commandLine,';') && commandLine[0] != ';'){
		char *token;
		if (commandLine[strlen(commandLine) - 2] == ';') lastSemi = TRUE;
		token = (char *)strtok(commandLine,";");
		while (token != NULL){
			strcpy(commandSemi[*commandLineSemiCnt],trim(token));
			token = (char *)strtok(NULL,";");
			*commandLineSemiCnt += 1;
		}
	} // 입력받은 명령어를 ';' 으로 파싱해줌.
	if (lastSemi) *commandLineSemiCnt -= 1;

	deleteSpaceOrTab(tmp,' ');
	deleteSpaceOrTab(tmp,'\t');
	// 스페이스나 tab 을 모두 제거함.

	if (strstr(tmp,";&") || strstr(tmp,"|&")) {
		printf("-bash: syntax error near unexpected token '&'\n");
		successParsing = FALSE;
	} else if (strstr(tmp,">!;")) {
		printf("-bash: !: event not found\n");
		successParsing = FALSE;
	} else if (strstr(tmp,";|") || strstr(tmp,"&|")) {
		printf("-bash: syntax error near unexpected token '|'\n");
		successParsing = FALSE;
	} else if (strstr(tmp,"&;") || strstr(tmp,">;") || strstr(tmp,">>;") ||
			   strstr(tmp,"<;") || strstr(tmp,"<<;")|| strstr(tmp,";;")  || strstr(tmp,"|;")) {
		printf("-bash: syntax error near unexpected token ';'\n");
		successParsing = FALSE;
	}
	// 명령어 상에서의 문법 체크.
	free(tmp);
	return successParsing;
}

void parseCommandArg(char *commandSemi,int *argCnt){
	char *token;
	for (int i = 0; i < strlen(commandSemi); i ++){
		if (commandSemi[i]=='\t') commandSemi[i] = ' ';
	}
	token = (char *)strtok(commandSemi," ");
	while (token != NULL){
		strcpy(commandLineArg[*argCnt],token);
		token = (char *)strtok(NULL," ");
		*argCnt += 1;
	}
	//';' 으로 나누어진 각각의 명령에서 다시 argument 단위로 파싱함.
}

void parseBackground(char *commandSemi,int *cnt2,bool *isLastBackground){

	*isLastBackground = ( commandSemi[strlen(commandSemi) - 1] == '&' ? TRUE : FALSE );
	// 마지막 명령어가 백그라운드 실행인지 확인해줌.
	if (strstr(commandSemi,"&")){
		char *token;
		background = TRUE; // background 실행 여부.
		token = (char *)strtok(commandSemi,"&");
		while (token != NULL){
			strcpy(commandBg[*cnt2],token);
			token = (char *)strtok(NULL,"&");
			*cnt2 += 1;
		}
	} else {
		return ;
	}// 백그라운드 실행 명령어 갯수 세어줌.
}

void printCurrentPath(void){
	
	char *token,*ptr;
	getcwd(currentPath,PATHSIZE);
	if (strcmp(currentPath,"/")){
		token = strtok(currentPath,"/");
		while (token != NULL){
			ptr = token;
			token = strtok(NULL,"/");
		}
		strcpy(currentPath,ptr);
	} // 현재 경로에서의 마지막 디렉토리만 출력해준다.
	printf("JinHan-MiniShell:%s %s$ ",currentPath,getlogin());
	fflush(stdout);
}

void insertHistory(int *historyCnt){
	if (strcmp(commandLine,"\n")){
		if (*historyCnt >= HISTORYSIZE - 1) {
			*historyCnt = HISTORYSIZE - 1;
			for (int i = 0; i < *historyCnt; i ++){
				strcpy(history[i],history[i+1]);
			}
			strcpy(history[*historyCnt],commandLine);
		}
		else { 
			*historyCnt += 1;
			strcpy(history[*historyCnt],commandLine);
		}
	}// 입력받은 명령어를 Queue 형태의 history 에 저장한다.
}

void showHistory(int *historyCnt){
	printf("< History >\n");
	for (int i = 0; i <= *historyCnt; i ++){
		printf("[%d] : %s\n",i + 1,history[i]);
	}
}

void processCommand(char *commandSemi){
	pid_t pid;
	int pid_status;
	int argCnt = 0; // argument count.
 	int fd,pipe_cnt = 0;

 	deleteSpaceOrTab(commandSemi,'"');
 	bzero(tmpBuffer,INPUTSIZE);
 	strcpy(tmpBuffer,commandSemi);

	parseCommandArg(commandSemi,&argCnt); // 각각 argument 로 파싱함.

	char **argument = NULL;
	if (argCnt > 1){	
		argument = (char **)malloc(sizeof(char *)*(argCnt + 1));
		for (int i = 0; i < argCnt; i ++){
			*(argument + i) = (char *)malloc(sizeof(char)*ARGSIZE);
			strcpy(*(argument + i),commandLineArg[i]);
		}
		*(argument + argCnt) = NULL;
	} else {
		argument = (char **)malloc(sizeof(char *)*2);
		*(argument) = (char *)malloc(sizeof(char)*ARGSIZE);
		*(argument + 1) = (char *)malloc(sizeof(char)*ARGSIZE);
		strcpy(*argument,commandLineArg[0]);
		*(argument + 1) = NULL;
	} // 입력받은 명령어를 fork / execvp 에 사용하기 위해 복사한다.

	for (int i = 0; i < argCnt; i ++){

		if (!strcmp(commandLineArg[0],"cd")){
			return changeDirectory(argCnt);
		} else if (strstr(commandLineArg[i],">>")){
			return inoutFileRedirection(2);
		} else if (strstr(commandLineArg[i],">!")){
			return inoutFileRedirection(3);
		} else if (strstr(commandLineArg[i],">")){
			return inoutFileRedirection(0);
		} else if (strstr(commandLineArg[i],"<")){
			return inoutFileRedirection(1);
		} else if (strstr(commandLineArg[i],"|")){
			pipe_cnt++;
		}
	} // 해당 명령어에 어떠한 type 의 부호가 있는지 판별 후 해당 명령어 처리.

	if (pipe_cnt == 1){
		return pipeFunction();
	} else if (pipe_cnt >= 2){
		return multipipeFunction(pipe_cnt);
	} // pipe / multiple pipes 인 경우.

	pid = fork();
	if (pid < 0) error("Fork Failed");
	else if (pid == 0) {
		if (execvp(commandLineArg[0],argument) == -1){
			printf("-bash: %s: command not found\n",commandLineArg[0]);
		}
	} else {
		if (background) {
			backgroundPid[backgroundCnt++] = (int)pid;
			printf("[%d] %d\n",backgroundCnt,pid);
			// 백그라운드 실행 알림.
			return ;
		} else {
			wait(&pid_status);
		}
	}
}

void inoutFileRedirection(int what){
	pid_t pid;
	int pip[2];
	int i = 0,fd,pid_status,n;
	char *buffer[ARGSIZE],*filename[ARGSIZE];
	char *token1,*token2,*token3;
	char *redirection;
	if (what == 0) redirection = ">";
	else if (what == 1) redirection = "<";
	else if (what == 2) redirection = ">>";
	else if (what == 3) redirection = ">!";
	// redirection 종류에 따라 token 구분자를 나눈다.
	token1 = (char *)strtok(tmpBuffer,redirection);
	token2 = (char *)strtok(NULL,redirection);

	token3 = (char *)strtok(token1," ");
	while (token3 != NULL){
		buffer[i++] = token3;
		token3 = (char *)strtok(NULL," ");
	}
	buffer[i] = (char *)'\0';
	// 명령어를 사용하기 위해 tokenize 된 구문을 다시 스페이스로 파싱함.
	i = 0;
	token3 = (char *)strtok(token2," ");
	while (token3 != NULL){
		filename[i++] = token3;
		token3 = (char *)strtok(NULL," ");
	}
	filename[i] = (char *)'\0';
	// 명령어를 사용하기 위해 tokenize 된 구문을 다시 스페이스로 파싱함.
	
	if (what == 1){
		if (pipe(pip) == -1) error("Pipe Error");
	} // "<" 인 경우 파이프 생성하여 파이프로 해당 data 전송.

	pid = fork();
	if (pid < 0) error("Fork Failed");
	else if (pid == 0) {
		if (what == 0 || what == 2 || what == 3){
			if (what == 0){
				if ( (fd = open(filename[0], O_RDWR | O_CREAT | O_TRUNC, 0644)) == -1 ) error("File Open/Create Failed");
			} else if (what == 2){
				if ( (fd = open(filename[0], O_WRONLY | O_APPEND, 0644)) == -1 ) error("File Open/Create Failed");
			} else if (what == 3){
				if ( ( fd = open(filename[0],O_RDWR | O_CREAT | O_TRUNC, 0644)) == -1 ) error("File Create Failed");
			}
			dup2(fd,OUTPUT);
			close(fd);
			if (execvp(buffer[0],buffer) == -1) printf("-bash: %s: command not found\n",buffer[0]);	
		} else if (what == 1){
			char tmp[2];
			close(pip[0]);
			dup2(pip[1],INPUT);
			close(pip[1]);
			if ( (fd = open(filename[0], O_RDONLY, 0644)) == -1 ) error("File Open Failed");
			while ( (n = read(fd,tmp,1)) > 0 ) putchar(tmp[0]);
			if (execvp(buffer[0],buffer) == -1) printf("-bash: %s: command not found\n",buffer[0]);
			close(fd);
		}
	} // 각각 redirection 에 따른 명령어 수행을 처리해준다. 
	else {
		if (background) {
			backgroundPid[backgroundCnt++] = (int)pid;
			printf("[%d] %d\n",backgroundCnt,pid);
			// 백그라운드 실행 알림.
			return ;
		} else {
			wait(&pid_status);
		}
	}
}

void pipeFunction(void){
	pid_t pid;
	int pip[2];
	int i = 0,fd,pid_status;
	char *buffer1[ARGSIZE],*buffer2[ARGSIZE];
	char *token1,*token2,*token3;

	token1 = (char *)strtok(tmpBuffer,"|");
	token2 = (char *)strtok(NULL,"|");

	token3 = (char *)strtok(token1," ");
	while (token3 != NULL){
		buffer1[i++] = token3;
		token3 = (char *)strtok(NULL," ");
	}
	buffer1[i] = (char *)'\0';
	// 명령어를 사용하기 위해 tokenize 된 구문을 다시 스페이스로 파싱함.
	i = 0;
	token3 = (char *)strtok(token2," ");
	while (token3 != NULL){
		buffer2[i++] = token3;
		token3 = (char *)strtok(NULL," ");
	}
	buffer2[i] = (char *)'\0';
	// 명령어를 사용하기 위해 tokenize 된 구문을 다시 스페이스로 파싱함.
	if (pipe(pip) == -1) error("Pipe Error");
	
	pid = fork();
	if (pid < 0) error("Fork Failed");
	else if (pid == 0){
		close(OUTPUT);
		dup(pip[1]);
		close(pip[0]);
		close(pip[1]);
		if (execvp(buffer1[0], buffer1) == -1 ) printf("-bash: %s: command not found\n",buffer1[0]);
	} // 첫번째 (앞) 구문 실행.
	pid = fork();
	if (pid < 0) error("Fork Failed");
	else if (pid == 0){
		close(INPUT);
		dup(pip[0]);
		close(pip[0]);
		close(pip[1]);
		if (execvp(buffer2[0], buffer2) == -1 ) printf("-bash: %s: command not found\n",buffer2[0]);
	} // 두번째 (뒤) 구문 실행.
	close(pip[0]);
	close(pip[1]);
	if (background) {
		backgroundPid[backgroundCnt++] = (int)pid;
		printf("[%d] %d\n",backgroundCnt,pid);
		// 백그라운드 실행 알림.
		return ;
	} else {
		wait(&pid_status);
	}
}

void multipipeFunction(int pipe_cnt){
	pid_t pid;
	int pip[2],pid_status;
	int fd = 0,t_cnt = 0,b_cnt = 0;
	char *tokenTmp;
	char *token[pipe_cnt];
	char *buffer[ARGSIZE];

	tokenTmp = (char *)strtok(tmpBuffer,"|");
	while (tokenTmp != NULL){
		token[t_cnt++] = tokenTmp;
		tokenTmp = (char *)strtok(NULL,"|");
	}

	for (int i = 0; i < t_cnt; i ++){
		bzero(buffer,ARGSIZE);
		b_cnt = 0;
		tokenTmp = (char *)strtok(token[i]," ");
		while (tokenTmp != NULL){
			buffer[b_cnt++] = tokenTmp;
			tokenTmp = (char *)strtok(NULL," ");
		}
		buffer[b_cnt] = (char *)'\0';

		if (pipe(pip) == -1) error("Pipe Error");

		pid = fork();
		if (pid < 0) error("Fork Failed");
		else if (pid == 0){
			dup2(fd,INPUT);
			if (i != t_cnt - 1) dup2(pip[1],OUTPUT);
			close(pip[0]);
			if (execvp(buffer[0], buffer) == -1 ) printf("-bash: %s: command not found\n",buffer[0]);
		} else{
			wait(&pid_status);
			close(pip[1]);
			fd = pip[0];
		}
	}// pipe 갯수 만큼 돌리면서 서로 서로 pipe 의 입출력을 연결해줌.
}

void changeDirectory(int argCnt){
	if (argCnt == 1 || !strcmp(commandLineArg[1],"~")) chdir(getenv("HOME"));
	else {
		if (chdir(commandLineArg[1]) == -1) {
			printf("-bash: cd: %s: No such file or directory\n",commandLineArg[1]);
		}
	} 
}

void backgroundDone(void){
	usleep(100000*backgroundCnt);
	for (int i = 0; i < backgroundCnt; i ++){
		printf("[%d] Done %d\n",i + 1,backgroundPid[i]);
	}
	backgroundCnt = 0;
}

void handler(int sig){

	while (waitpid(-1,NULL,WNOHANG) > 0);
}

void error(char *s){
	fprintf(stderr,"ERROR : %s\n",s);
	exit(1);
}

char *trim(char *s) {
	char *tmp = lightTrim(s);
	return rightTrim(tmp);
}

char *rightTrim(char *s) {
	char tmp[INPUTSIZE2];
	char *end;
	strcpy(tmp,s);
	end = tmp + strlen(tmp) - 1;
	while (end != tmp && isspace(*end)) end--;
	*(end + 1) = '\0';
	s = tmp;
	return s;
}

char *lightTrim(char *s) {
	char *begin;
	begin = s;
	while (*begin != '\0') {
		if (isspace(*begin)) begin++;
		else {
			s = begin;
			break;
		}
	}
	return s;
}

void deleteSpaceOrTab(char *s,char word){
    char *pstr = s;
    char *pstrOld = strdup(s);
    char *pstrNew;
    char *pstrOldFree = pstrOld;
    
    pstrNew = strchr(pstrOld,word);
    
    while(pstrNew){
           strncpy( pstr, pstrOld, pstrNew-pstrOld);
           *(pstr+(pstrNew-pstrOld)) = 0;
   	       
           pstr += (pstrNew-pstrOld);
           
           pstrOld = pstrNew + 1;
           pstrNew = strchr( pstrOld,word);
           
           if( pstrNew == NULL )
           strcat(pstr, pstrOld );
           
    }
    free(pstrOldFree);
}