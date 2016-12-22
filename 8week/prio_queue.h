#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>

#define MSGQKEY (key_t)0111
#define PERMISSION 0777
#define MAXLENGTH 100
#define MAXPRIO 20

struct msg_buf{
	long mtype;
	char mtext[MAXLENGTH + 1];
	// long type 과 char type 순서 바뀌면 안됨.
	// msg_buf 첫 type 은 반드시 4byte long 이여야함. 
};