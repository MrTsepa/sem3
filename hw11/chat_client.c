#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>

#define LINE_SIZE 1000

struct sockaddr_in servaddr;

int establish_connection(int argc, char** argv) 
{
	int sockfd;
	if (argc != 2) {
		printf("Usage: a.out <IP adress>\n");
		exit(1);
	}
	if ((sockfd = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
		perror(NULL);
		exit(1);
	}
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(51000);
	inet_aton(argv[1], &servaddr.sin_addr);
	return sockfd;
}

int main(int argc, char **argv)
{
	int sockfd = establish_connection(argc, argv);
	printf("Connection established\n");
	char sendline[LINE_SIZE];
	char login[256];
	bzero(sendline, LINE_SIZE);
	bzero(login, 256);
	printf("Insert your login > ");
	fgets(login, LINE_SIZE, stdin);
	login[strlen(login) - 1] = '\0';
	while(1) {
		printf("Message > ");
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
	close(sockfd);
}
