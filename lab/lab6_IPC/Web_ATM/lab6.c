#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pthread.h>
#include <semaphore.h>
#include "sockop.h"

int total_property = 0; 
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void remove_semaphore()
{ 
    int rc; 
    
    printf ("Main thread clean up mutex\n");
    rc = pthread_mutex_destroy(&mutex);
}

void handler(int signum) 
{
    remove_semaphore();
    while (waitpid(-1, NULL, WNOHANG) > 0);
}

void write_to_client(int connfd, char *buf, size_t n)
{
    if((write(connfd, buf, n+1)) == -1)
        printf("Error : write()\n");

    // printf("%s\n", buf);
}

void read_from_client(int connfd, char *buf)
{   
    memset(buf, 0, 1024);
    if((read(connfd, buf, 1024)) == -1)
        printf("Error : read()\n");
    
    // printf("%s\n", buf);
}

void *web_atm(void *t)
{
    char snd[1024], rcv[1024], action[1024];
    int connfd = t;
    int n, amount, i, rc;

    while(1)
    {
        read_from_client(connfd, rcv);
        
        if(strstr(rcv, "Finish"))
            break;
        else
            sscanf(rcv, "%s%d", action, &amount);
        
        if(strstr(action, "deposit"))
        {
            // printf("deposit $%d\n", amount);
            rc = pthread_mutex_lock(&mutex);
            total_property += amount;
            printf("After deposit: %d\n", total_property);
            rc = pthread_mutex_unlock(&mutex);
            
            n = sprintf(snd, "Done");
            write_to_client(connfd, snd, n);
            memset(action, 0, 1024);
        }

        if(strstr(action, "withdraw"))
        {
            // printf("withdraw $%d\n", amount);  
            rc = pthread_mutex_lock(&mutex);
            total_property -= amount;
            printf("After withdraw: %d\n", total_property);
            rc = pthread_mutex_unlock(&mutex);

            n = sprintf(snd, "Done");
            write_to_client(connfd, snd, n);
            memset(action, 0, 1024);
        } 
    }

    pthread_exit(NULL);
    close(connfd);
}

int main(int argc, char *argv[])
{
    int sockfd , connfd ; /* socket descriptor */
    struct sockaddr_in addr_cln;
    socklen_t sLen = sizeof(addr_cln) ;
    pthread_t threads[100];
    int rc, t = 0;
    pthread_mutex_init(&(mutex),NULL);

    if(argc != 2)
        printf("Usage: ./hw6 <port>\n");

    /* create socket and bind socket to port */
    sockfd = passivesock(argv[1], "tcp", 100);
    
    while(1)
    {   
        /* waiting for connection */
        connfd = accept(sockfd, (struct sockaddr *) &addr_cln, &sLen);
        
        if(connfd == -1)
            printf("Error: accept()\n");

        
        rc = pthread_create(&threads[t], NULL, web_atm, (void *)connfd);
        if(rc)
        {
            printf("ERROR: return code from pthread_create() is %d\n", rc);
            exit(-1);
        }
        t++;

        signal(SIGCHLD, handler);
    }
    
    pthread_exit(NULL);

    /* close client connection */
    close(connfd);

    /* close server socket */
    close(sockfd);

    return 0;
}
