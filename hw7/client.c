#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>

/* Тип сообщения для прекращения работы программы 2 */
#define LAST_MESSAGE 255 

int main()
{
	int msqid;
	char pathname[] = "temp.txt";
	key_t key;

	int i, len;
	pid_t pid = getpid();

/*
 * Чтобы не дублировать один и тот же код дважды (объявление типов), можно это вынести в отдельный файл mytypes.h
 * В клиенте и сервере в начале файла написать #include "mytypes.h"
 * А при компиляции писать gcc client.c mytypes.h -o client,
 * т.е. указывать все "причастные файлы".
 */
  
	struct SndMsg {
		long mtype;
		struct {
			pid_t pid;
			int a;
			int b;
		} info;
	} SndBuf;

	struct RcvMsg {
		long mtype;
		struct {
			int res;
		} info;
	} RcvBuf;

	if ((key = ftok(pathname, 0)) < 0) {
		printf("Can\'t generate key\n");
		exit(-1);
	}

	if((msqid = msgget(key, 0666 | IPC_CREAT)) < 0) {
		printf("Can\'t get msqid\n");
		exit(-1);
	}

	int a = 2, b = 3;
//	scanf("%d %d", &a, &b);

	SndBuf.mtype = 1;
	SndBuf.info.a = a;
	SndBuf.info.b = b;
	SndBuf.info.pid = pid;
	len = sizeof(SndBuf.info);

	if (msgsnd(msqid, (struct SndMsg *) &SndBuf, len, 0) < 0) {
		printf("Can\'t send message to queue\n");
		msgctl(msqid, IPC_RMID, (struct msqid_ds*)NULL);
		exit(-1);
	}
	else {
		printf("Message sent. Type = %lu, info = (%d, %d), pid = %d\n", 
				SndBuf.mtype, SndBuf.info.a, SndBuf.info.b,
				SndBuf.info.pid);
	}

	/*
   * Зачем это действия с семафором?
   */
	sem_t *sem;
	sem = sem_open("/my_sem", 0);
	sem_post(sem);

	printf("	Waiting to recieve...\n");

	len = sizeof(RcvBuf.info);

	if (msgrcv(msqid, (struct RcvMsg*)&RcvBuf, len, pid, 0) < 0) {
		printf("Can\'t recieve message from queue\n");
		msgctl(msqid, IPC_RMID, (struct msqid_ds*)NULL);
		exit(-1);
	}
	else {
		printf("Message recieved. res = %d\n", RcvBuf.info.res);
	}
}
