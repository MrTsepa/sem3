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

int sockaddr_incmp(struct sockaddr_in addr1, struct sockaddr_in addr2)
{
	if ((addr1.sin_family == addr2.sin_family) &&
		(addr1.sin_port == addr2.sin_port) &&
		(addr1.sin_addr.s_addr == addr2.sin_addr.s_addr)) 
		return 0;
	else return 1;
}

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
	
	struct sockaddr_in clientaddr_arr[1000];
	int client_number = 0;

	while(1) {
		struct sockaddr_in temp_addr;
		socklen_t temp_addr_len = sizeof(temp_addr);
		recvfrom(sockfd, recvline, LINE_SIZE, 0, (struct sockaddr*)&temp_addr, &temp_addr_len);
		int i;
		int flag = 0;
		for (i = 0; i < client_number; i++) {
			if(sockaddr_incmp(temp_addr, clientaddr_arr[i]) == 0) {
				flag = 1;
			}
		}
		if (flag == 0) {
			clientaddr_arr[client_number] = temp_addr;
			client_number++;
			printf("%d\n", client_number);
		}
		for (i = 0; i < client_number; i++) {
			sendto(sockfd, recvline, strlen(recvline)+1, 0,
					(struct sockaddr*)&clientaddr_arr[i], sizeof(clientaddr_arr[i]));
		}
		printf("%s", recvline);
	}
}
