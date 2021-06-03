#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/time.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <string.h>

#define SHMSZ 32

typedef struct
{
    int guess;
    char result[8];
}data;

data *shmdata;
int count = 0, leave = 0;
int upper_bound, lower_bound, guess_ans, serverpid;

void guess_handler(int signum)
{
    if(strstr(shmdata->result, "bigger"))
        guess_ans *= 2;
    else if(strstr(shmdata->result, "smaller"))
        guess_ans /= 2;
    else if(strstr(shmdata->result, "bingo"))
        leave = 1;
}

void timer_handler(int signum)
{
    count++;
    // printf("Timer expired %d times\n", count);
    shmdata->guess = guess_ans;
    printf("[game] Guess: %d\n", shmdata->guess);
    kill(serverpid, SIGUSR1);
}

int main(int argc, char *argv[])
{
    struct sigaction sa;
    struct itimerval timer;
    timer.it_value.tv_sec = 1;
    timer.it_value.tv_usec = 0;
    timer.it_interval.tv_sec = 1;
    timer.it_interval.tv_usec = 0;
    setitimer(ITIMER_VIRTUAL, &timer, NULL);
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = timer_handler;
    sigaction(SIGVTALRM, &sa, NULL);

    struct timespec req;
    int shmid, retval;
    key_t key;
    void *shmaddr;

    /* Locate the segment */
    key = atoi(argv[1]);
    if((shmid = shmget(key, SHMSZ, 0666)) < 0)
    {
        perror("shmget");
        exit(1);
    }

    /* Attach the segment to our data space */
    if((shmaddr = shmat(shmid, NULL, 0)) == (char *) -1) 
    {
        perror("shmat");
        exit(1);
    }
    printf("Server create and attach the share memory.\n");
    shmdata = (data *)shmaddr;

    upper_bound = atoi(argv[2]);
    serverpid = atoi(argv[3]);
    guess_ans = upper_bound/2;
    signal(SIGUSR1, guess_handler);

    req.tv_sec = 0;
    req.tv_nsec = 10;

    do{
        // retval = nanosleep(&req, &req);
    }while(leave != 1);

    /* Detach the share memory segment */
    shmdt(shmaddr);

    return 0;
}
