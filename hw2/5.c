#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

int main()
{
	int n;
	scanf("%d", &n);
	int i;
	int cur_time = 0;
	for(i = 0; i < n; i++){
		int j;
		int time, x;
		scanf ("%d %d", &time, &x);		//программы вводятся уже отсортированные по времени, x - количество аргументов в строке
		sleep(time-cur_time);
		cur_time = time;
		char *arr[100];
		for (j = 0; j < x; j++){
			arr[j] = (char*)malloc(30*sizeof(char));
			scanf("%s", arr[j]);
		}	
		pid_t p = fork();
		if (p == 0){
			execvp(arr[0], arr);
		}
		if (p != 0){
			wait(NULL);
		}
	}
}
