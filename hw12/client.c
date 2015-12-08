#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>

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
	servaddr.sin_port = htons(51000);
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

int main(int argc, char **argv)
{
	int sockfd = establish_connection(argc, argv);
	printf("Connection established\n");
	int n;
	int i;
	char sendline[1000], recvline[1000];
	bzero(sendline, 1000);
	bzero(recvline, 1000);
	while(1) {
		printf("String > ");
		fflush(stdin);
		fgets(sendline, 1000, stdin);
		if ( (n = write(sockfd, sendline, 
				strlen(sendline)+1)) < 0) {
			perror("Can\'t write\n");
			close(sockfd);
			exit(1);
		}
		if ( (n = read(sockfd, recvline, 999)) < 0) {
			perror("Can\'t read\n");
			close(sockfd);
			exit(1);
		}
		printf("%s", recvline);
	}
	close(sockfd);
}
