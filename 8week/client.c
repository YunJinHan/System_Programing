#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "prio_queue.h"

int msg_put(int msgq_id,char *request,int prio){
	int len,a;
	struct msg_buf req_msg;
	if ((len = strlen(request)) > MAXLENGTH){
		perror("request name too long");
		exit(1);
	}
	if (prio > MAXPRIO || prio < 0){
		perror("wrong priority\n");
		return -1;
	}
	req_msg.mtype = (long)prio;
	strncpy(req_msg.mtext,request,MAXLENGTH);

	if (msgsnd(msgq_id,&req_msg,len,0) == -1){
		perror("message send failed");
		return -1;
	}
	else return 0;
}

int main(int argc,char **argv){
	int msg_prio,msgq_id;
	int running = 1;
	char request[MAXLENGTH + 1];
	msgq_id = msgget(MSGQKEY,IPC_CREAT|PERMISSION);

	printf("%d\n",msgq_id);

	if (msgq_id == -1){
		perror("msg queue creation failed");
		exit(1);
	}

	while (running){
		strncpy(request,argv[1],MAXLENGTH);
		printf("argv[1]: %s\n",request);
		msg_prio = atoi(argv[2]);
		printf("argv[2]: %d\n",msg_prio);

		if (msg_put(msgq_id,request,msg_prio) < 0){
			perror("msg send failed");
			exit(1);
		}
		if (!strcmp(request,"end")){
			printf(">>> NULL <<<\n");
			strcpy(request,"end");
			msg_prio = MAXPRIO;

			if (msg_put(msgq_id,request,msg_prio) < 0){
				perror("msg send failed");
				exit(1);
			}
		}
		running = 0;
	}
	exit(0);
}