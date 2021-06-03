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

int connectsock(const char *host, const char *service, const char *transport)
{
    struct hostent *phe; /* pointer to host information entry */
    struct servent *pse; /* pointer to service information entry */
    struct sockaddr_in sin; /* an Internet endpoint address */

    int s, type; /* socket descriptor and socket type */
    
    memset(sin.sin_zero, '\0', sizeof(sin.sin_zero));

    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = INADDR_ANY;

    /* Map service name to port number */
    if((pse = getservbyname(service, transport)))
        sin.sin_port = pse->s_port;
    else if((sin.sin_port = htons((unsigned short)atoi(service))) == 0)
        printf("Can’t get %s service entry\n", service);
    
    /* Map host name to IP address , allowing for dotted decimal */
    if((phe = gethostbyname(host)))
        memcpy(&sin.sin_addr, phe->h_addr, phe->h_length);
    else if((sin.sin_addr.s_addr = inet_addr(host)) == INADDR_NONE)
        printf("Can’t get %s host entry\n", host);
    
    /* Use protocol to choose a socket type */
    if(strcmp(transport, "udp") == 0)
        type = SOCK_DGRAM;
    else
        type = SOCK_STREAM;

    /* Allocate a socket */
    s = socket(PF_INET, type, 0);
    
    if(s < 0)
        printf("Can’t create socket: %s\n", strerror(errno));
    
    /* Connect the socket */
    if(connect(s, (struct sockaddr *) &sin, sizeof(sin)) < 0)
        printf("Can’t connect to %s.%s: %s\n", host, service, strerror(errno));
    
    return s ;
}

void write_to_server(int connfd, char *buf)
{
    if((write(connfd, buf, strlen(buf))) == -1)
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
    int connfd ; /* socket descriptor */
    char buf[1024], snd[1024], rcv[1024];

    connfd = connectsock(argv[1], argv[2], "tcp");
    
    while(true)
    {   
        memset(rcv, 0, 1024);
        memset(buf, 0, 1024);
        memset(snd, 0, 1024);
        fgets(buf, 1023, stdin);

        if(strstr(buf, "checkout"))
        { 
            write_to_server(connfd, buf);
            while(true)
            {
                read_from_server(connfd, rcv);

                if(strstr(rcv, "done"))
                    break;
            }
        }
        else
        {   
            write_to_server(connfd, buf);
            read_from_server(connfd, rcv);
        }
    }
    
    /* close client socket */
    close(connfd);

    return 0;
}
