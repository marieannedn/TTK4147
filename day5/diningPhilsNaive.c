#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

pthread_mutex_t *forks;
pthread_t *phils;

int philNum;

void eat(int i)
{
    printf("Philosopher %d is eating \n", i + 1);
}

void *philosopher(void *num)
{
    int i = 0, left, right;

    while (!pthread_equal(*(phils + i), pthread_self()) && i < 5)
        i++;

    while (1)
    {
        printf("Phil %d is thinking \n", i + 1);

        left = i;
        right = (i + 1) % 5;

        pthread_mutex_lock(forks + left);
        pthread_mutex_lock(forks + right);
        eat(i);
        pthread_mutex_unlock(forks + left);
        pthread_mutex_unlock(forks + right);
    }

    return NULL;
}

int main(void)
{
    phils = (pthread_t *)malloc(5 * sizeof(pthread_t));
    forks = (pthread_mutex_t *)malloc(5 * sizeof(pthread_mutex_t));

    for (int i = 0; i < 5; i++)
    {
        if (pthread_mutex_init(forks + i, NULL) != 0)
        {
            printf("Failed fork init: %d\n", i + 1);
        }
    }

    for (int i = 0; i < 5; i++)
    {
        int err = pthread_create(phils + i, NULL, &philosopher, NULL);
        if (err != 0)
        {
            printf("Failed phil init: %d\n", i + 1);
        }
        else
        {
            printf("Phil init succeeded: %d\n", i + 1);
        }
    }
    for (int i = 0; i < 5; i++)
    {
        pthread_join(*(phils + i), NULL);
    }
    free(phils);
    free(forks);

    return 0;
}