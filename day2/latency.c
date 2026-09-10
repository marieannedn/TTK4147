#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <sys/times.h>
#include <stdint.h>
#include <string.h>
#include <sched.h>

struct timespec timespec_normalized(time_t sec, long nsec){
    while(nsec >= 1000000000){
        nsec -= 1000000000;
        ++sec;
    }
    while(nsec < 0){
        nsec += 1000000000;
        --sec;
    }
    return (struct timespec){sec, nsec};
}

struct timespec timespec_sub(struct timespec lhs, struct timespec rhs){
    return timespec_normalized(lhs.tv_sec - rhs.tv_sec, lhs.tv_nsec - rhs.tv_nsec);
}

struct timespec timespec_add(struct timespec lhs, struct timespec rhs){
    return timespec_normalized(lhs.tv_sec + rhs.tv_sec, lhs.tv_nsec + rhs.tv_nsec);
}

int timespec_cmp(struct timespec lhs, struct timespec rhs){
    if (lhs.tv_sec < rhs.tv_sec)
        return -1;
    if (lhs.tv_sec > rhs.tv_sec)
        return 1;
    return lhs.tv_nsec - rhs.tv_nsec;
}

uint64_t rdtsc(void){
    uint64_t val;
    asm volatile("isb; mrs %0, cntvct_el0; isb; " : "=r"(val) :: "memory"); 
    // You can check the current CPU frequency with $sudo dmesg | grep MHz
    return val;
}

int main(void){
    struct timespec now1, now2;
    int ns_max = 5000;
    int histogram[ns_max];
    memset(histogram, 0, sizeof(int)*ns_max);


    for(int i = 0; i < 10*1000*1000; i++){
        clock_gettime(CLOCK_MONOTONIC, &now1);
        sched_yield();
        clock_gettime(CLOCK_MONOTONIC, &now2);

        uint64_t t1 = now1.tv_nsec;
        uint64_t t2 = now2.tv_nsec;
        
        int ns = (t2 - t1); //1 tick tar 18ns
        
        if(ns >= 0 && ns < ns_max){
            histogram[ns]++;
        }
    }

    for(int i = 0; i < ns_max; i++){
        printf("%d\n", histogram[i]);
    }

    return 0;
}

/*
int main(void) {
    struct tms now;
    for(int i = 0; i < 10*1000*1000; i++){
        times(&now);
    }
    return 0;
}*/
