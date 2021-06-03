#include "solitare_func.h"

int main(int argc, char *argv[])
{
    struct sigaction sigIntHandler;
    sigIntHandler.sa_handler = ctrlc_handler;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;
    sigaction(SIGINT, &sigIntHandler, NULL);

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

    signal(SIGUSR1, send_msg_handler);

    int sockfd , connfd; /* socket descriptor */
    struct sockaddr_in addr_cln;
    socklen_t sLen = sizeof(addr_cln) ;
    pthread_t threads[32]; 
    int rc, t = 0;
    pthread_mutex_init(&(mutex1),NULL);  
    pthread_mutex_init(&(mutex2),NULL);
    pthread_mutex_init(&(mutex3),NULL);

    /* create socket and bind socket to port */
    sockfd = passivesock(argv[1], "tcp", 80);

    srand(time(NULL));

    int i, r, tmp;

    for(i=0; i<54; i++)
        pocker[i] = i+1;
    
    for(i=0; i<54; i++)
    {
        r = rand() % 53 + 1;

        tmp = pocker[i];
        pocker[i] = pocker[r];
        pocker[r] = tmp;
    }
    
    printf("Finish Shuffling\n");

    while(1)
    {   
        /* waiting for connection */
        connfd = accept(sockfd, (struct sockaddr *) &addr_cln, &sLen);

        if(connfd == -1)
            printf("Error: accept()\n");

        rc = pthread_create(&threads[t], NULL, fast_solitare, (void *)connfd);
        if(rc)
        {
            printf("ERROR: return code from pthread_create() is %d\n", rc);
            exit(-1);
        }
        t++;

        signal(SIGCHLD, process_handler);
    }
    
    pthread_exit(NULL);

    /* close client connection */
    close(connfd);

    /* close server socket */
    close(sockfd);

    return 0;
}
