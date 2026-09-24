#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#define _XOPEN_SOURCE 600

pthread_barrier_t barr;
// sem_t sem;
long globalCounter = 0;
// Note the argument and return types: void*
void *fn(void *args)
{
    long localCounter = 0;
    for (int i = 0; i < 1000000; i++)
    {
        localCounter++;
        // sem_wait(&sem);
        globalCounter++;
        // sem_post(&sem);
    }
    printf("Local counter done: %ld, enter wait state \n", localCounter);
    int res = pthread_barrier_wait(&barr);
    return NULL;
}

int main()
{
    pthread_barrier_init(&barr, NULL, 2);
    // sem_init(&sem, 0, 1);

    pthread_t threadIncrement;
    pthread_t threadHandle;
    pthread_create(&threadHandle, NULL, fn, NULL);
    pthread_create(&threadIncrement, NULL, fn, NULL);
    printf("Threads created \n");
    pthread_join(threadHandle, NULL);
    pthread_join(threadIncrement, NULL);
    // sem_destroy(&sem);
    printf("Threads joined, global counter finished at value %ld\n", globalCounter);
}