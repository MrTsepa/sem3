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

#define LINE_SIZE 1000

// Не смог придумать как реализовать viewer на стороне пользователя,
// так как непонятно как хранить адреса всех подключенных клиентов.

int main(int argc, char **argv)
{
	int sockfd;
	struct sockaddr_in servaddr;
	char recvline[LINE_SIZE];

	if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
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

	while(1) {
		recvfrom(sockfd, recvline, LINE_SIZE, 0, NULL, NULL);
		printf(recvline);
	}
}
