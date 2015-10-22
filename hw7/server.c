#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define SLEEP_TIME 10
#define MAX_THREAD_NUMBER 2

int msqid;
int thread_number = 0;

struct RcvMsg {
	long mtype;
	struct {
		pid_t pid;
		int a;
		int b;
	} info;
};

struct SndMsg {
	long mtype;
	struct {
		int res;
	} info;
};

void *thread_func(void *RcvBuf)
{
	struct SndMsg SndBuf;
	int snd_len = sizeof(SndBuf.info);

	SndBuf.mtype = ((struct RcvMsg*)RcvBuf)->info.pid;
	SndBuf.info.res = ((struct RcvMsg*)RcvBuf)->info.a * ((struct RcvMsg*)RcvBuf)->info.b;
	sleep(SLEEP_TIME);

	if (msgsnd(msqid, (struct SndMsg *) &SndBuf, snd_len, 0) < 0) {
		printf("Can\'t send message to queue\n");
		msgctl(msqid, IPC_RMID, (struct msqid_ds*)NULL);
		exit(-1);
	}
	else {
		printf("Message sent to pid %lu. Res = %d\n", 
				SndBuf.mtype, SndBuf.info.res);
	}
	
	printf("	Waiting to recieve...\n");
	
	return NULL;
}

int main()
{
	char pathname[] = "temp.txt";
	key_t key;

	int i;

	struct SndMsg SndBuf;
	struct RcvMsg RcvBuf;
	int rcv_len = sizeof(RcvBuf.info);
	int snd_len = sizeof(SndBuf.info);

	if ((key = ftok(pathname, 0)) < 0) {
		printf("Can\'t generate key\n");
		exit(-1);
	}

	if((msqid = msgget(key, 0666 | IPC_CREAT)) < 0) {
		printf("Can\'t get msqid\n");
		exit(-1);
	}

while(1)
{
	printf("	Waiting to recieve...\n");

	if (msgrcv(msqid, (struct RcvMsg*)&RcvBuf, rcv_len, 1, 0) < 0) {
		printf("Can\'t recieve message from queue\n");
		msgctl(msqid, IPC_RMID, (struct msqid_ds*)NULL);
		exit(-1);
	}
	else {
		printf("Message recieved from pid %d. info = %d, %d\n",
			       RcvBuf.info.pid, RcvBuf.info.a, 
			       RcvBuf.info.b);
	}

	pthread_t thread;

	if (pthread_create(&thread, NULL, thread_func, (void*)&RcvBuf) != 0)
	{
		return EXIT_FAILURE;
	}
}
}
