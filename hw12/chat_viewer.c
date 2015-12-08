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

int main(int argc, char** argv)
{	
	int server_sockfd = establish_connection(argc, argv);

	char line[1024];	
	int n;
	while ((n = read(server_sockfd, line, 999)) > 0) {
		fprintf(stderr, "%s", line);
	}
}
