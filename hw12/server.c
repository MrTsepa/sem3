#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/sem.h>
#include <pthread.h>

#define START_SUM 100
#define MAX_THREAD_NUMBER 3

int semid;

struct sembuf semaphore;

int sockfd;

void *thread_func()
{
	int n;
	char line[1000];
	int newsockfd;
	struct sockaddr_in cliaddr;
	socklen_t clilen = sizeof(cliaddr);
	if ((newsockfd = accept(sockfd, 
			(struct sockaddr*) &cliaddr, 
			&clilen)) < 0) {
		perror(NULL);
		close(sockfd);
		exit(1);
	}
	while ((n = read(newsockfd, line, 999)) > 0) {
		line[0] = line[0] + 1;
		if ((n = write(newsockfd, line,
				strlen(line) + 1)) < 0) {
			perror(NULL);
			close(sockfd);
			close(newsockfd);
			exit(1);
		}
	}
	if (n < 0) {
		perror(NULL);
		close(sockfd);
		close(newsockfd);
		exit(1);
	}
	close(newsockfd);

	semaphore.sem_flg = 0;
	semaphore.sem_num = 0;
	semaphore.sem_op = 1;
	semop(semid, &semaphore, 1);

	return NULL;
}

int main(int argc, char **argv)
{
	int n;
	struct sockaddr_in servaddr;

	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror(NULL);
		exit(1);
	}
	
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(51000);
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(sockfd, (struct sockaddr *) &servaddr, 
				sizeof(servaddr)) < 0) {
		perror(NULL);
		close(sockfd);
		exit(1);
	}

	if (listen(sockfd, 5) < 0) {
		perror(NULL);
		close(sockfd);
		exit(1);
	}

	char sem_pathname[] = "sem";
	key_t key_sem;
	if ((key_sem = ftok(sem_pathname, 0)) < 0) {
		printf("Can\'t generate semaphore key\n");
		exit(-1);
	}

	if((semid = semget(key_sem, 1, 0666 | IPC_CREAT)) < 0) {
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

	while(1) {
		semaphore.sem_flg = 0;
		semaphore.sem_num = 0;
		semaphore.sem_op = -1;
		semop(semid, &semaphore, 1);

		pthread_t thread;

		if (pthread_create(&thread, NULL, thread_func, NULL) != 0)
		{
			return EXIT_FAILURE;
		}
	}
}
