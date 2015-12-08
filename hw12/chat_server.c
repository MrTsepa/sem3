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

int sockfd, viewer_sockfd;
int establish_connection(int argc, char** argv);

void *thread_func()
{
	int n;
	char line[1000];
	int client_sockfd;
	struct sockaddr_in cliaddr;
	socklen_t clilen = sizeof(cliaddr);
	if ((client_sockfd = accept(sockfd, 
			(struct sockaddr*) &cliaddr, 
			&clilen)) < 0) {
		perror(NULL);
		close(sockfd);
		exit(1);
	}

	char login[1000];
	bzero(login, 1000);
	write(client_sockfd, "Please enter your login > ", 30);
	read (client_sockfd, login, 128);
	login[strlen(login)-1] = '\0';
	printf ("Client [%s] connected\n", login);
	while ((n = read(client_sockfd, line, 999)) > 0) {;
		char *str = (char*)malloc(strlen(line)+strlen(login)+4);
		str[0] = '[';
		strcat(str, login);
		strcat(str, "]: ");
		strcat(str, line);
		write(viewer_sockfd, str, strlen(str) + 1);
	}
	if (n < 0) {
		perror(NULL);
		close(sockfd);
		close(client_sockfd);
		exit(1);
	}
	close(client_sockfd);

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

	struct sockaddr_in viewaddr;
	int view_sockfd;

	if((view_sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror(NULL);
		exit(1);
	}
	
	bzero(&viewaddr, sizeof(viewaddr));
	viewaddr.sin_family = AF_INET;
	viewaddr.sin_port = htons(51001);
	viewaddr.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(view_sockfd, (struct sockaddr *) &viewaddr, 
				sizeof(viewaddr)) < 0) {
		perror(NULL);
		close(sockfd);
		exit(1);
	}

	if (listen(view_sockfd, 5) < 0) {
		perror(NULL);
		close(sockfd);
		exit(1);
	}

	socklen_t viewlen = sizeof(viewaddr);
	if ((viewer_sockfd = accept(view_sockfd, 
			(struct sockaddr*) &viewaddr, 
			&viewlen)) < 0) {
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

int establish_connection(int argc, char** argv) 
{
	int sockfd;
	struct sockaddr_in servaddr;
	if (argc != 2) {
		printf("Usage: a.out <IP adress>\n");
		exit(1);
	}

	if ((sockfd = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
		perror(NULL);
		exit(1);
	}

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(51001);
	if (inet_aton(argv[1], &servaddr.sin_addr) == 0) {
		printf("Invalid IP address\n");
		close(sockfd);
		exit(1);
	}

	if (connect(sockfd, (struct sockaddr *) &servaddr, 
				sizeof(servaddr)) < 0) {
		perror(NULL);
		close(sockfd);
		exit(1);
	}
	return sockfd;
}
