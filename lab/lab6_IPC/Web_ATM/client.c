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
#include "sockop.h"

void write_to_server(int connfd, char *buf, size_t n)
{
    if((write(connfd, buf, n+1)) == -1)
        printf("Error : write()\n");

    // printf("%s\n", buf);
}

void read_from_server(int connfd, char *buf)
{  
    memset(buf, 0, 1024);
    if((read(connfd, buf, 1024)) == -1)
        printf("Error : read()\n");
    
    printf("%s\n", buf);
}

int main(int argc, char *argv[])
{
    int connfd; /* socket descriptor */
    char snd[1024], rcv[1024];
    int n, i, times;

    if(argc != 6)
        printf("Usage: ./client <ip> <port> <deposit/withdraw> <amount> <times>\n");

    times = atoi(argv[5]);
    printf("%d\n", times);
    n = sprintf(snd, "%s %s", argv[3], argv[4]);
    
    connfd = connectsock(argv[1], argv[2], "tcp");
    
    for(i=0; i<times; i++)
    {           
        write_to_server(connfd, snd, n);
        printf("%d\n", i);
        while(1)
        {
            read_from_server(connfd, rcv);
            if(strstr(rcv, "Done"))
                break;
        }
    }

    n = sprintf(snd, "Finish");
    write_to_server(connfd, snd, n);
    
    /* close client socket */
    close(connfd);

    return 0;
}
