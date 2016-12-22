#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "prio_queue.h"

int msg_servie(int msgq_id){
	int len;
	struct msg_buf req_msg;

	if ((len = msgrcv(msgq_id,&req_msg,MAXLENGTH,((-1)*MAXPRIO),MSG_NOERROR)) == -1){
		perror("message rcv failed");
		return -1;
	} else {
		req_msg.mtext[len] = '\0';
		printf("-----> %s",req_msg.mtext);

		if (strcmp(req_msg.mtext,"end") == 0){
			printf("\n!!!!!!\n");
			return 1;
		} else {
			printf("\npriority:%ld name:%s\n",req_msg.mtype,req_msg.mtext);
			return 0;
		}
	}
}

int main(void){
	int msg_prio, msgq_id, res, running = 1;
	msgq_id = msgget(MSGQKEY,PERMISSION | IPC_CREAT);
	if (msgq_id == -1){
		perror("msg queue creation failed");
		exit(1);
	}
	while (running){
		res = msg_servie(msgq_id);
		if (res < 0){
			perror("msg service failed");
			exit(1);
		} else if (res == 1) running = 0;
	}

	if (msgctl(msgq_id,IPC_RMID,0) == -1){
		perror("msgq remove failed");
		exit(1);
	}
}