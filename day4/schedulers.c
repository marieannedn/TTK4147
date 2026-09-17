#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>
#include <string.h>
#include "scheduling.h"
#include "schedulers.h"

void set_task_state(struct Task *task, enum taskState taskNewState)
{
    pthread_mutex_lock(&taskStateMutex);
    task->state = taskNewState;
    pthread_mutex_unlock(&taskStateMutex);
}

void wait_for_rescheduling(int quantum, struct Task *task)
{
    int startTime;
    int waitTime;

    pthread_mutex_lock(&timeMutex);
    startTime = globalTime;
    pthread_mutex_unlock(&timeMutex);

    do
    {
        pthread_mutex_lock(&timeMutex);
        pthread_cond_wait(&timeCond, &timeMutex);
        waitTime = globalTime - startTime;
        pthread_mutex_unlock(&timeMutex);
    } while (task->state != finished && waitTime < quantum);

    usleep(timeUnitUs / 100);
}

void round_robin(struct Task **tasks, int taskCount, int timeout, int quantum)
{
    int taskIndex = 0;

    do
    {
        // Skip finished tasks or those that have not arrived yet
        if (tasks[taskIndex]->state == finished || tasks[taskIndex]->arrivalTime > globalTime)
        {
            taskIndex = (taskIndex + 1) % taskCount;
            continue;
        }

        // Set the task state to running
        if (tasks[taskIndex]->startTime == -1)
            tasks[taskIndex]->startTime = globalTime;
        set_task_state(tasks[taskIndex], running);

        // Wait for the quantum interval
        wait_for_rescheduling(quantum, tasks[taskIndex]);

        //  Check if the task is finished
        if (tasks[taskIndex]->state == finished)
        {
        }
        else
        {
            set_task_state(tasks[taskIndex], preempted);
        }

        // Find the next task to run
        taskIndex = (taskIndex + 1) % taskCount;

    } while (globalTime < timeout);
}

// Implement your schedulers here!
void first_come_first_served(struct Task **tasks, int taskCount, int timeout)
{
    // Implement your solution here
    int taskIndex = 0;

    do
    {
        // Skip finished tasks or those that have not arrived yet
        if (tasks[taskIndex]->state == finished || tasks[taskIndex]->arrivalTime > globalTime)
        {
            taskIndex = (taskIndex + 1) % taskCount;
            continue;
        }

        // Set the task state to running
        if (tasks[taskIndex]->startTime == -1)
            tasks[taskIndex]->startTime = globalTime;
        set_task_state(tasks[taskIndex], running);

        // Wait for the quantum interval
        wait_for_rescheduling(QUANTUM, tasks[taskIndex]);

        //  Check if the task is finished
        if (tasks[taskIndex]->state == finished)
        {
        }
        else
        {
            set_task_state(tasks[taskIndex], preempted);
        }
        int minIndex = -1;
        int minArrival = 100000000000000;
        // Find the next task to run
        for (int t = 0; t < taskCount; t++) {
            if (tasks[t]->arrivalTime < minArrival){
                minArrival = tasks[t]->arrivalTime;
                minIndex = tasks[t]->ID;
            }
        }
        taskIndex = minIndex;
    } while (globalTime < timeout);
}

void shortest_process_next(struct Task **tasks, int taskCount, int timeout)
{
    // Implement your solution here
    int taskIndex = 0;

    do
    {

        // Set the task state to running
        int minIndex = -1;
        int minRunTime = 100000000000000;
        // Find the next task to run
        for (int t = 0; t < taskCount; t++) {
            if (tasks[t]->totalRuntime < minRunTime && tasks[t]->state != finished && tasks[t]->arrivalTime < globalTime){
                minRunTime = tasks[t]->totalRuntime;
                minIndex = t;
                printf("id: %d \n",minIndex);
                printf("time: %d \n",minRunTime);
            }
        }

        taskIndex = minIndex;
        printf("next id: %d \n",taskIndex);
        if (taskIndex != -1) {
            if (tasks[taskIndex]->startTime == -1)   tasks[taskIndex]->startTime = globalTime;
            set_task_state(tasks[taskIndex], running);

            // Wait for the quantum interval
            wait_for_rescheduling(1000, tasks[taskIndex]);
        }
    } while (globalTime < timeout);
}
void highest_response_ratio_next(struct Task **tasks, int taskCount, int timeout)
{
    // Implement your solution here
    int taskIndex = 0;

    do
    {

        // Set the task state to running
        int maxIndex = -1;
        int maxRatio = 0;
        // Find the next task to run
        for (int t = 0; t < taskCount; t++) {
            int queueTime = globalTime - tasks[t]->arrivalTime;
            int runTime = tasks[t]->totalRuntime;
            float ratio = (queueTime + runTime)/runTime;
            if (ratio > maxRatio && tasks[t]->state != finished && tasks[t]->arrivalTime < globalTime){
                maxRatio = ratio;
                maxIndex = t;
            }
        }

        taskIndex = maxIndex;
        printf("next id: %d \n",taskIndex);
        if (taskIndex != -1) {
            if (tasks[taskIndex]->startTime == -1)   tasks[taskIndex]->startTime = globalTime;
            set_task_state(tasks[taskIndex], running);

            // Wait for the quantum interval
            wait_for_rescheduling(1000, tasks[taskIndex]);
        }
    } while (globalTime < timeout);
}
void shortest_remaining_time(struct Task **tasks, int taskCount, int timeout, int quantum)
{
    int taskIndex = 0;

    do
    {   int minIndex = -1;
        int minRunTime = 100000000000000;
        // Find the next task to run
        for (int t = 0; t < taskCount; t++) {
            if (tasks[t]->totalRuntime < minRunTime && tasks[t]->state != finished && tasks[t]->arrivalTime < globalTime){
                minRunTime = tasks[t]->totalRuntime;
                minIndex = t;
                printf("id: %d \n",minIndex);
                printf("time: %d \n",minRunTime);
            }
        }

        taskIndex = minIndex;
        if (taskIndex != -1) {
            // Set the task state to running
            if (tasks[taskIndex]->startTime == -1)
                tasks[taskIndex]->startTime = globalTime;
            set_task_state(tasks[taskIndex], running);

            // Wait for the quantum interval
            wait_for_rescheduling(quantum, tasks[taskIndex]);

            //  Check if the task is finished
            if (tasks[taskIndex]->state == finished)
            {
            }
            else
            {
                set_task_state(tasks[taskIndex], preempted);
            }
        }
    } while (globalTime < timeout);
}
void feedback(struct Task **tasks, int taskCount, int timeout, int quantum)
{
    int taskIndex = 0;
    int preemptedCounter[] = {0,0,0,0,0};

    do
    {   int minIndex = -1;
        int minRunTime = 100000000000000;
        int minPreemption = 100000;
        // Find the next task to run
        for (int t = 0; t < taskCount; t++) {
            if (preemptedCounter[t] <= minPreemption && tasks[t]->state != finished && tasks[t]->arrivalTime < globalTime){
                minRunTime = tasks[t]->totalRuntime;
                minIndex = t;
                minPreemption = preemptedCounter[t];
            }
        }

        taskIndex = minIndex;
        if (taskIndex != -1) {
            // Set the task state to running
            if (tasks[taskIndex]->startTime == -1)
                tasks[taskIndex]->startTime = globalTime;
            set_task_state(tasks[taskIndex], running);

            // Wait for the quantum interval
            wait_for_rescheduling(quantum, tasks[taskIndex]);

            //  Check if the task is finished
            if (tasks[taskIndex]->state == finished)
            {
            }
            else
            {
                set_task_state(tasks[taskIndex], preempted);
                preemptedCounter[taskIndex]++;
            }
        }
    } while (globalTime < timeout);
}
