#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>

#define LINE_SIZE 1000

int sockfd;
struct sockaddr_in servaddr, clientaddr;

int establish_connection(int argc, char** argv) 
{
	int tmp_sockfd;
	if (argc != 2) {
		printf("Usage: a.out <IP adress>\n");
		exit(1);
	}
	if ((tmp_sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		perror(NULL);
		exit(1);
	}
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(51000);
	inet_aton(argv[1], &servaddr.sin_addr);

	bzero(&clientaddr, sizeof(clientaddr));
	clientaddr.sin_family = AF_INET;
	clientaddr.sin_port = htons(0);
	clientaddr.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(tmp_sockfd, (struct sockaddr *) &clientaddr, 
				sizeof(clientaddr)) < 0) {
		perror(NULL);
		close(sockfd);
		exit(1);
	}
	return tmp_sockfd;
}

void* input_thread_func()
{
	char sendline[LINE_SIZE];
	char login[256];
	bzero(sendline, LINE_SIZE);
	bzero(login, 256);
	printf("Insert your login > ");
	fgets(login, LINE_SIZE, stdin);
	login[strlen(login) - 1] = '\0';
	while(1) {
//		printf("Message > ");
		fflush(stdin);
		char inputline[1000];
		fgets(inputline, LINE_SIZE, stdin);
		bzero(sendline, LINE_SIZE);
		strcat(sendline, "[");
		strcat(sendline, login);
		strcat(sendline, "]: ");
		strcat(sendline, inputline);
		if (sendto(sockfd, sendline, strlen(sendline)+1, 0,
				(struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
			perror("Can\'t write\n");
			close(sockfd);
			exit(1);
		}
	}
	return NULL;
}

void* output_thread_func()
{
	while(1)
	{
		char buf[LINE_SIZE];
		recvfrom(sockfd, buf, LINE_SIZE, 0, NULL, NULL); 
		printf("%s", buf);
	}
	return NULL;
}

/*
 * Сделайте без отдельного viewer`а.
 * Пусть клиент создаст две нити: первая для чтения с консоли и отправки сообщений, 
 * а вторая для чтения из сокета и вывода сообщений на экран.
 */

int main(int argc, char **argv)
{
	sockfd = establish_connection(argc, argv);
	printf("Connection established\n");

	pthread_t input_thread, output_thread;
	pthread_create(&input_thread, NULL, input_thread_func, NULL);
	pthread_create(&output_thread, NULL, output_thread_func, NULL);

	pthread_join(input_thread, NULL);
	pthread_join(output_thread, NULL);
	close(sockfd);
}
