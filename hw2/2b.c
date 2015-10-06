#include <stdio.h>
#include <unistd.h>

#define N 5

int main()
{
	int i;
	printf("First process, process id = %d\n", getpid());
	for(i = 0; i < N-1; i++){
		pid_t p = fork();
		if (p == 0){
			printf("Process id = %d, Parent id = %d\n", getpid(), getppid());
		}
		if (p != 0){
			wait(NULL);
			break;
		}
	}
}
