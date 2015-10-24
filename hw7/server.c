#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

#define SLEEP_TIME 20
#define MAX_THREAD_NUMBER 0

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
  /*
   * FIXIT:
   * как мы уже убеждались, операция инкрементации неатомарная, поэтому не исключено состояние гонки.
   * надо обеспеспечить её атомарность.
   */
	thread_number++;

	printf("Executing task of pid %d\n", ((struct RcvMsg*)RcvBuf)->info.pid);

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
	
	/*
   * FIXIT: та же беда, что и с инкрементацией выше.
   */
	thread_number--;

	printf("	Waiting to recieve...\n");
	
	return NULL;
}

int main()
{
	char pathname[] = "temp.txt";
	key_t key;

	int i;

  /*
   * FIXIT:
   * у вас все переменные названы со строчной буквы, а следующие две почему-то с заглавной. поправьте.
   */
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

/*
 * Что с форматироваем стало?
 */
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
	
  /*
   * FIXIT: вы передаёте в ф-ю потока указатель на одну и ту же переменную RcvBuf,
   * которая на следующей итерации цикла уже изменится, что в общем случае приведёт к неверной работе программы.
   * Нужно обеспечить ситуацию, когда внутри каждой ф-и будет своя RcvBuf, которая не изменится в течение всей работы нити.
   */
	if (pthread_create(&thread, NULL, thread_func, (void*)&RcvBuf) != 0) {
		return EXIT_FAILURE;
	}
}
}
