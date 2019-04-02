#define _GNU_SOURCE
#include <sched.h>
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>

#define NUM_THREADS 5

// -lrt

void *threadFunction(void *tid) {
	int t;
	long i;
	t = (int)tid;

	clockid_t cpuid;
	struct timespec t;

	pthread_getcpuclockid(pthread_self(), &cpuid);
	clock_gettime(cpuid, &t);
	while
		clock_gettme(cpuid, &t);


	pthread_exit(NULL);
}

int main() {

pthread_t t;

struct sched_param param;

for(i=0; i<NUM_THREADS; i++)
	pthread_create(&t, NULL, teste, (void *)&tid);

// Obtem parametros da thread
if (pthread_getschedparam(t, &policy, &param) != 0) {
printf("Erro pthread_getschedparam!\n");
return 0;
}
printf("Escalonador = %d, Prioridade = %d\n", policy, param.sched_priority);
// Prioridade minimas e maximas para escalonador FIFO
printf("Prioridade minima FIFO: %d\n", prio_min = sched_get_priority_min(SCHED_FIFO));
printf("Prioridade maxima FIFO: %d\n", prio_max = sched_get_priority_max(SCHED_FIFO));
// Seta prioridade da thread
param.sched_priority = (int)prio_max/2;
//if (pthread_setschedparam(t, SCHED_FIFO, &param) != 0) {
//  printf("Erro pthread_setschedparam!\n");
//  return 0;
//}
// Obtem parametros da thread
if (pthread_getschedparam(t, &policy, &param) != 0) {
printf("Erro pthread_getschedparam!\n");
return 0;
}
printf("Escalonador = %d, Prioridade = %d\n", policy, param.sched_priority);
pthread_join(t, NULL);
return 0;
}
