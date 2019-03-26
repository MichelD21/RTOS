#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#define NUM_THREADS 10

int var_global = 0;

void *PrintHello(void *threadid)
{
	long tid;
	tid = (long)threadid;
	while(1)
	{
		if(var_global < 1000)
		{
			var_global++;
			printf("Thread %c, iterating global to %d\n", (char)tid, var_global);
		}
		else pthread_exit(NULL);
	}
} 
int main (int argc, char *argv[])
{ 
	pthread_t threads[NUM_THREADS]; 
	int rc; 
	long t; 
	for(t=65; t<NUM_THREADS+65; t++)
	{ 
		printf("In main: creating thread %c\n", (char)t); 
		rc = pthread_create(&threads[t], NULL, PrintHello, (void *)t); 
		if (rc)
		{ 
			printf("ERROR; return code from pthread_create() is %d\n", rc); 
			exit(-1); 
		} 
	}
	for(t=65; t<NUM_THREADS+65; t++)
	{
		pthread_join(threads[t], NULL);
	}
}
