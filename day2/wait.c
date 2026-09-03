#define _POSIX_C_SOURCE 199309L
//#define _DEFAULT_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <sys/times.h>

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
 
void busy_wait(struct timespec t){
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    struct timespec then = timespec_add(now, t);
    
    while(timespec_cmp(now, then) < 0){
        for(int i = 0; i < 10000; i++){}
        clock_gettime(CLOCK_MONOTONIC, &now);
    }
}

void busy_wait_times(double seconds){
    struct tms start, now;
    long ticks_per_second = sysconf(_SC_CLK_TCK);
    
    times(&start);
    
    while(1){
        times(&now);

        clock_t elapsed_ticks = (now.tms_utime + now.tms_stime) - (start.tms_utime + start.tms_stime);

        if ((double)elapsed_ticks /ticks_per_second  >= seconds) break;
    }
}

int main(void){
    /*struct timespec ts;
    ts.tv_sec = 1;
    ts.tv_nsec = 0;*/

    busy_wait_times(1);

    return 0;
}