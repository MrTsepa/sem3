#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>

#define STRING "Hello, World!"
#define STRING_LEN 13

pid_t ppid, chpid;
int t = 0;

void handler(int nsig) {
	if (nsig == 30) {
		t = 0;
	}
	if (nsig == 31) {
		t = 1;
	}
}

int main()
{
	int a = 0;
	printf("%d", a << 1);
}
