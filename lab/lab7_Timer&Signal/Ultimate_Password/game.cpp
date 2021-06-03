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
int count = 0, leave = 0, ans;

void ctrlc_handler(int signum)
{
    printf("\nexit\n");
    leave = 1;
}

void guess_handler(int signum, siginfo_t *info, void *context)
{
    int n;

    if(shmdata->guess > ans)
    {
        n = sprintf(shmdata->result, "[game] Guess %d, smaller", shmdata->guess);
        printf("%s\n", shmdata->result);
        kill(info->si_pid, SIGUSR1);
    }
    else if (shmdata->guess < ans)
    {
        n = sprintf(shmdata->result, "[game] Guess %d, bigger", shmdata->guess);
        printf("%s\n", shmdata->result);
        kill(info->si_pid, SIGUSR1);
    }
    else
    {
        n = sprintf(shmdata->result, "[game] Guess %d, bingo", shmdata->guess);
        printf("%s\n", shmdata->result);
        kill(info->si_pid, SIGUSR1);
    }

    FILE *fPtr = fopen("result.txt", "a");
    fprintf(fPtr,"%s\n",shmdata->result);
    fclose(fPtr);
}

int main(int argc, char *argv[])
{
    struct sigaction sigIntHandler;
    sigIntHandler.sa_handler = ctrlc_handler;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;
    sigaction(SIGINT, &sigIntHandler, NULL);

    struct sigaction my_action;
    memset(&my_action, 0, sizeof(struct sigaction));
    my_action.sa_flags = SA_SIGINFO;
    my_action.sa_sigaction = guess_handler;
    sigaction(SIGUSR1, &my_action, NULL);

    struct timespec req;
    int shmid, retval;
    key_t key;
    void *shmaddr;

    /* Create the segment */
    key = 1234;
    if((shmid = shmget(key, SHMSZ, IPC_CREAT|0666)) < 0)
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

    printf("[game] Game PID: %d\n", getpid());
    ans = atoi(argv[2]);

    FILE *fPtr = fopen("result.txt", "w");
    fclose(fPtr);

    req.tv_sec = 0;
    req.tv_nsec = 10; 

    do{
        retval = nanosleep(&req, &req);
    }while(leave != 1);

    /* Detach and Destroy the share memory segment */
    shmdt(shmaddr);
    printf("Server destroy the share memory.\n");
    retval = shmctl(shmid, IPC_RMID, NULL);
    if(retval < 0)
    {
        fprintf(stderr, "Server remove share memory failed\n");
        exit(1);
    }

    return 0;
}
