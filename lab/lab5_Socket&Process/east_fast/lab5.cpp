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

#define BUFSIZE 1024

int passivesock(const char *service, const char *transport, int qlen)
{
    // struct servent *pse; /* pointer to service information entry */
    struct sockaddr_in sin; /* an Internet endpoint address */

    int s, type; /* socket descriptor and socket type */

    memset(sin.sin_zero, '\0', sizeof(sin.sin_zero));

    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = INADDR_ANY;
    sin.sin_port = htons((u_short)atoi(service));

    /* Use protocol to choose a socket type */
    if(strcmp(transport, "udp") == 0)
        type = SOCK_DGRAM;
    else
        type = SOCK_STREAM;

    /* Allocate a socket */
    s = socket(AF_INET, type, 0);

    if(s < 0)
        printf("Can’t create socket: %s \n", strerror(errno));

    int yes = 1;
    setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));

    /* Bind the socket */
    if(bind(s, (struct sockaddr *) &sin, sizeof(sin)) < 0)
        printf("Can’t bind to port %s : %s\n", service , strerror(errno));
    
    /* Set the maximum number of waiting connection */
    if(type == SOCK_STREAM && listen(s, qlen) < 0)
        printf("Can’t listen on port %s : %s\n", service, strerror(errno));

    return s ;
}

void handler(int signum) 
{
    while (waitpid(-1, NULL, WNOHANG) > 0);
}

int main(int argc, char *argv[])
{
    int sockfd , connfd ; /* socket descriptor */
    struct sockaddr_in addr_cln;
    socklen_t sLen = sizeof(addr_cln) ;
    int n;
    char snd[BUFSIZE], rcv[BUFSIZE];
    
    if(argc != 2)
        printf("Usage: %s port\n", argv[0]);

    /* create socket and bind socket to port */
    sockfd = passivesock(argv[1], "tcp", 3);
    
    while(true)
    {
        /* waiting for connection */
        connfd = accept(sockfd, (struct sockaddr *) &addr_cln, &sLen);

        if(connfd == -1)
            printf("Error: accept()\n");

        sprintf(snd, "Client connetion success\n");
        send(connfd, snd, strlen(snd), 0);
        
        pid_t childpid = fork();
        if(childpid >= 0)
        {
            if(childpid == 0)
            {
                printf("Train ID is: %d\n", getpid());
                dup2(connfd, STDOUT_FILENO);
                execlp("sl", "ls", "-l", NULL);
                signal(SIGCHLD, handler);
            }
        }
        else
        {
            perror("fork");
            exit(-1);
        }
        
        /* close client connection */
        close(connfd);
    }

    /* close server socket */
    close(sockfd);

    return 0;
}
