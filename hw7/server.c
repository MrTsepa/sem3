#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/sem.h>

#define SLEEP_TIME 10
#define MAX_THREAD_NUMBER 2

int msqid;
int semid;

struct sembuf semaphore;

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

void *thread_func(void *ptrrcvBuf)
{
	struct RcvMsg rcvBuf = *((struct RcvMsg*)ptrrcvBuf);
	free(ptrrcvBuf);					// Теперь данные точно успевают скопироваться
	printf("Executing task of pid %d.\n", rcvBuf.info.pid);

	struct SndMsg sndBuf;
	int snd_len = sizeof(sndBuf.info);

	sndBuf.mtype = rcvBuf.info.pid;
	sndBuf.info.res = rcvBuf.info.a * rcvBuf.info.b;
	sleep(SLEEP_TIME);

	if (msgsnd(msqid, (struct SndMsg *) &sndBuf, snd_len, 0) < 0) {
		printf("Can\'t send message to queue\n");
		msgctl(msqid, IPC_RMID, (struct msqid_ds*)NULL);
		exit(-1);
	}
	else {
		printf("Message sent to pid %lu. Res = %d\n",
				sndBuf.mtype, sndBuf.info.res);
	}

	semaphore.sem_flg = 0;
	semaphore.sem_num = 0;
	semaphore.sem_op = 1;
	semop(semid, &semaphore, 1);

	return NULL;
}

int main()
{
	char pathname[] = "temp.txt", pathname_s[] = "sem.txt";
	key_t key, key_s;

	int i;
	struct RcvMsg rcvBuf;
	int rcv_len = sizeof(rcvBuf.info);

	if ((key = ftok(pathname, 0)) < 0) {
		printf("Can\'t generate key\n");
		exit(-1);
	}
	if ((key_s = ftok(pathname_s, 0)) < 0) {
		printf("Can\'t generate semaphore key\n");
		exit(-1);
	}

	if((msqid = msgget(key, 0666 | IPC_CREAT)) < 0) {
		printf("Can\'t get msqid\n");
		exit(-1);
	}
	if((semid = semget(key_s, 1, 0666 | IPC_CREAT)) < 0) {
		printf("Can\'t get semid\n");
		exit(-1);
	}

	int semval = semctl(semid, 0, GETVAL);	//
	semaphore.sem_flg = 0;			// обнуление семафора
	semaphore.sem_num = 0;			// перед началом работы
	semaphore.sem_op  = -1*semval;		//
	semop(semid, &semaphore, 1);		//

	semaphore.sem_flg = 0;
	semaphore.sem_num = 0;
	semaphore.sem_op  = MAX_THREAD_NUMBER;
	semop(semid, &semaphore, 1);

	while(1)
	{
		semaphore.sem_flg = 0;
		semaphore.sem_num = 0;
		semaphore.sem_op = -1;
		semop(semid, &semaphore, 1);

		struct RcvMsg rcvBuf;
		int rcv_len = sizeof(rcvBuf.info);

		if (msgrcv(msqid, (struct RcvMsg*)&rcvBuf, rcv_len, 1, 0) < 0) {
			printf("Can\'t recieve message from queue\n");
			msgctl(msqid, IPC_RMID, (struct msqid_ds*)NULL);
			exit(-1);
		}
		else {
			printf("Message recieved from pid %d. info = %d, %d\n",
				       rcvBuf.info.pid, rcvBuf.info.a,
				       rcvBuf.info.b);
		}

		pthread_t thread;

		/*
		 * Зачем вам дополнительная переменная? Могли бы сразу выделить rcvBuf в куче.
		 * В целом, нормально. Засчитано. 
		 */
		struct RcvMsg *ptrrcvBuf = (struct RcvMsg*)malloc(sizeof(rcvBuf));
		*ptrrcvBuf = rcvBuf;

		if (pthread_create(&thread, NULL, thread_func, (void*)ptrrcvBuf) != 0) {
			return EXIT_FAILURE;
		}
	}
}
