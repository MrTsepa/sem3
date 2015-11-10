#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>

pid_t ppid, chpid;

char * str = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";
char ** str_res_ptr;

char cur_char = 0;
int pos_in_char = 0;
int pos_in_str = 0;

void handler(int nsig) {
	char* str_res = *str_res_ptr;
	int t;
	if (nsig == SIGUSR1)
		t = 0;
	if (nsig == SIGUSR2)
		t = 1;
	if (pos_in_char < 8) {
		cur_char += t * (1 << pos_in_char);
		pos_in_char++;
	}
	if (pos_in_char == 8) {
		str_res[pos_in_str] = cur_char;
		cur_char = 0;
		pos_in_char = 0;
		pos_in_str++;
	}
	if (pos_in_str == strlen(str)) {
		str_res[pos_in_str] = '\0';
		printf("%s\n", str_res);
		kill(chpid, SIGKILL);
		exit(0);
	}
	kill(chpid, SIGINT);
}

int a;

void ch_handler(int nsig) 
{}

int main()
{
	str_res_ptr = (char**)malloc(sizeof(char*));
	*str_res_ptr = (char*)malloc(sizeof(char)*(strlen(str)+1));
	signal(SIGUSR1, handler);
	signal(SIGUSR2, handler);
	signal(SIGINT, ch_handler);
	pid_t pid = fork();
  
	if (pid != 0) {
		chpid = pid;
		while(1) {;}
	}
	if (pid == 0) {
		ppid = getppid();
		int i;
		for (i = 0; i < strlen(str); i++) {
			int j;
			char c = str[i];
			for (j = 0; j < sizeof(char) * 8; j++) {
				int flag = c % 2;
				if (flag == 0) {
					kill(ppid, SIGUSR1);
				}
				if (flag == 1) {
					kill(ppid, SIGUSR2);
				}
				c = c >> 1;
				pause();
			}
		}
	}
}
