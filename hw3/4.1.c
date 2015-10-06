#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define S 1000
const int N = 2;

int arr[S][S];
int arr_res[S][S];

void* func(void* x)
{
	int i;
	for(  i = *((int*)x); i < S; i += N)
	{
		int j;
		for(  j = *((int*)x); j < S; j += N)
		{
			int k;
			int sum;
			for(  k = 0; k < S; k += 1)
			{
				sum += arr[i][k]*arr[k][j];
			}
			arr_res[i][j] = sum;
		}
	}
	return NULL;
}

int main ()
{
	time_t t = time(NULL);
	pthread_t id[N];
	int i;
	for(  i = 0; i < N; i += 1)
	{
		int j = i;
		pthread_create(&id[i], 
				(pthread_attr_t *)NULL, 
				func, &j);
	}
	for(  i = 0; i < N; i += 1)
	{
		pthread_join(id[i], (void **) NULL);
	}
	printf("%f\n", difftime(time(NULL), t));
	return 0;
}
