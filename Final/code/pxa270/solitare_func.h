/*
    solitare_func.h
*/ 
#ifndef _SOLITARE_FUNC_H_
#define _SOLITARE_FUNC_H_
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <netdb.h>
#include <time.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/ioctl.h> 
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <pthread.h>
#include <semaphore.h>
#include "asm-arm/arch-pxa/lib/creator_pxa270_lcd.h"

#define errexit(format, arg ... ) exit(printf(format,##arg))

void Clear_IO(int fd, lcd_write_info_t lcd, _7seg_info_t _7seg, unsigned short key, unsigned short led)
{
    led = LED_ALL_OFF;
    ioctl(fd, LED_IOCTL_SET, &led);
    ioctl(fd, LCD_IOCTL_CLEAR, NULL);
    ioctl(fd, KEY_IOCTL_CLEAR, key);
    ioctl(fd, _7SEG_IOCTL_ON, NULL);
    _7seg.Mode = _7SEG_MODE_HEX_VALUE;
    _7seg.Which = _7SEG_ALL;
    _7seg.Value = 0;
    ioctl(fd, _7SEG_IOCTL_SET, &_7seg);
    ioctl(fd, LCD_IOCTL_CLEAR, NULL);
}

void display_7seg(int fd, _7seg_info_t _7seg, int ans)
{
    _7seg.Mode = _7SEG_MODE_HEX_VALUE;
    _7seg.Which = _7SEG_ALL;
    _7seg.Value = (ans/100)*256 + ((ans%100)/10)*16 + ans%10;
    ioctl(fd, _7SEG_IOCTL_SET, &_7seg);
}

void display_led(int fd, unsigned short led)
{
    ioctl(fd, LED_IOCTL_SET, &led);
}

void display_lcd(int fd, lcd_write_info_t lcd)
{
    ioctl(fd, LCD_IOCTL_WRITE, &lcd);
}

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
        printf("Can’t create socket: %s\n", strerror(errno));

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

int connectsock(const char *host, const char *service, const char *transport)
{
    struct hostent *phe;            /* pointer to host information entry */
    struct servent *pse;            /* pointer to service information entry */
    struct sockaddr_in sin;         /* an Internet endpoint address */
    int s , type ;                  /* socket descriptor and socket type */

    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;

    /* Map service name to port number */
    if((pse = getservbyname(service, transport)))
    {
        sin.sin_port = pse->s_port;
    }
    else if((sin.sin_port = htons((unsigned short)atoi(service))) == 0)
    {
        errexit( "Can't get \"%s\" serviceentry\n" , service);
    }

    /* Map host name to IP address , allowing for dotted decimal */
    if((phe = gethostbyname(host)))
    {
        memcpy(&sin.sin_addr, phe -> h_addr, phe -> h_length);
    }
    else if((sin.sin_addr.s_addr = inet_addr(host)) == INADDR_NONE)
    {
        errexit( "Can't get \"%s\"  host entry\n" , host);
    }

    /* Use protocol to choose a socket type */
    if(strcmp(transport , "udp") == 0)
    {
        type = SOCK_DGRAM;
    }
    else{
        type = SOCK_STREAM;
    }

    /* Allocate a socket */
    s = socket(PF_INET, type, 0);
    if(s < 0)
    {
        errexit( "Can't create socket : %s\n" , strerror(errno)) ;
    }

    /* Connect the socket */
    if(connect(s , (struct sockaddr *)&sin, sizeof(sin)) < 0)
    {
        errexit( "Can't connect to %s.%s: %s\n", host, service, strerror(errno));
    }

    return s ;
}

pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex2 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex3 = PTHREAD_MUTEX_INITIALIZER;
int count = 0, modify = 0, modifier = 0, change = 1, finish_change = 1, change_times = 0, winner = 0, i1 = 0, i2 = 0, send_msg = 0;
int pocker[54], current_state[10], deck1[22], deck2[22];

void remove_semaphore()
{ 
    printf ("Main thread clean up mutex\n");
    pthread_mutex_destroy(&mutex1);
    pthread_mutex_destroy(&mutex2);
    pthread_mutex_destroy(&mutex3);
}

void ctrlc_handler(int signum)
{
    printf("\nEnd!!!\n\n");
    exit(1);
}

void process_handler(int signum) 
{
    remove_semaphore();
    while (waitpid(-1, NULL, WNOHANG) > 0);
}

void timer_handler(int signum)
{
    count++;
    // printf("Timer expired %d times\n", count);
}

void send_msg_handler(int signum)
{
    send_msg = 1;
}

void write_to(int connfd, char *buf, size_t n)
{
    if((write(connfd, buf, n+1)) == -1)
        printf("Error : write()\n");

    // printf("%s\n", buf);
}

void read_from(int connfd, char *buf)
{   
    memset(buf, 0, 1024);
    if((read(connfd, buf, 1024)) == -1)
        printf("Error : read()\n");
    
    printf("%s\n", buf);
}

void *fast_solitare(void *t)
{
    int connfd = t;
    int connfd0, connfd1, connfd2;
    int n, i, j, card_place, cover_place, ID;
    char snd[1024], rcv[1024];

    for(i=0; i<10; i++)
        current_state[i] = pocker[i];


    read_from(connfd, rcv);
    if(strstr(rcv, "display"))
    {
        ID = 0;
        connfd0 = t;
        n = sprintf(snd, "%.2d%.2d%.2d%.2d%.2d%.2d%.2d%.2d%.2d%.2d%.2d%.2d%.2d%.2d", current_state[0], current_state[1], current_state[2], current_state[3], current_state[4], current_state[5], current_state[6], current_state[7], current_state[8], current_state[9], modifier, modify, change_times, winner); 
        write_to(connfd0, snd, n);
    }
    else if(strstr(rcv, "player1"))
    {
        ID = 1;
        connfd1 = t;
        n = sprintf(snd, "%d", 22-i1);
        write_to(connfd1, snd, n);

        for(i=0; i<22; i++)
            deck1[i] = pocker[i+10];
    }
    else
    {
        ID = 2;
        connfd2 = t;
        n = sprintf(snd, "%d", 22-i1);
        write_to(connfd2, snd, n);

        for(i=0; i<22; i++)
           deck2[i] = pocker[i+32];
    }

    while(1)
    {
        j = 0;
        change = 1;
        finish_change = 0;

        if(ID == 0)
        {
            if(send_msg != 0)
            {
                pthread_mutex_lock(&mutex1);

                n = sprintf(snd, "%.2d%.2d%.2d%.2d%.2d%.2d%.2d%.2d%.2d%.2d%.2d%.2d%.2d%.2d", current_state[0], current_state[1], current_state[2], current_state[3], current_state[4], current_state[5], current_state[6], current_state[7], current_state[8], current_state[9], modifier, modify, change_times, winner);
                write_to(connfd0, snd, n);
                printf("Change cover pocker %d times\n", change_times);
                send_msg = 0;
                change_times = 0;

                pthread_mutex_unlock(&mutex1);
            }
        }
        else
        {
            while(finish_change == 0)
            {
                j = 0;

                while(j<10)
                {
                    if(current_state[j] != 0 && (abs(current_state[j] - current_state[4]) % 13 == 12 || abs(current_state[j] - current_state[4]) % 13 == 1 || abs(current_state[j] - current_state[5]) % 13 == 12 || abs(current_state[j] - current_state[5]) % 13 == 1 || current_state[j] == 53 || current_state[j] == 54 || current_state[4] == 53 || current_state[4] == 54 || current_state[5] == 53 || current_state[5] == 54))
                    {
                        change = 0;
                        finish_change = 1;
                        break;
                    }
                    else
                        change = 1;                

                    if(j == 3)
                        j += 3;
                    else
                        j++;
                }
                
                if(change)
                {
                    if(i1 > 21 && i2 > 21)
                    {
                        int cnt1 = 0, cnt2 = 0;
                        for(i=0; i<4; i++)
                        {
                            if(current_state[i] == 0)
                                cnt2++;
                            if(current_state[i+6] == 0)
                                cnt1++;
                        }

                        if(cnt1 > cnt2)
                            winner = 1;
                        if(cnt1 < cnt2)
                            winner = 2;
                        if(cnt1 == cnt2)
                            winner = 3;
                    }

                    if(winner != 0)
                    {
                        kill(getpid(), SIGUSR1);
                        break;
                    }

                    printf("Cover card change\n");

                    if(i2 < 22)
                        current_state[4] = deck2[i2++];
                    if(i1 < 22)
                        current_state[5] = deck1[i1++];

                    modify = 1;
                    if(ID == 1)
                    {
                        modifier = 3;
                        change_times++;
                        kill(getpid(), SIGUSR1);
                    }
                    else
                    {
                        modifier = 4;
                        change_times++;
                        kill(getpid(), SIGUSR1);
                    }
                }
            }

            read_from(connfd, rcv);
            cover_place = atoi(rcv)%10;
            card_place = atoi(rcv)/10;

            pthread_mutex_lock(&mutex2);

            if(current_state[card_place] != 0)
            {
                if(abs(current_state[card_place] - current_state[cover_place]) % 13 == 12 || abs(current_state[card_place] - current_state[cover_place]) % 13 == 1 || current_state[card_place] == 53 || current_state[card_place] == 54 || current_state[cover_place] == 53 || current_state[cover_place] == 54)
                {
                    modify = 1;  
                    modifier = ID;
                    kill(getpid(), SIGUSR1);
                }
                else
                {
                    modify = 0;
                    modifier = ID;
                    kill(getpid(), SIGUSR1);
                }
            }
            else
            {
                modify = 0;
                modifier = ID;
                kill(getpid(), SIGUSR1);
            }

            if(modify)
            {
                current_state[cover_place] = current_state[card_place];
 
                if(ID == 1)
                {
                    if(i1>21)
                        current_state[card_place] = 0;
                    else
                        current_state[card_place] = deck1[i1++];

                }
                else
                {
                    if(i2>21)
                        current_state[card_place] = 0;
                    else
                        current_state[card_place] = deck2[i2++];
                }
            }

            pthread_mutex_unlock(&mutex2);

            if(ID == 1)
            {
                n = sprintf(snd, "%d", 22-i1);
                write_to(connfd1, snd, n);
            }

            if(ID == 2)
            {
                n = sprintf(snd, "%d", 22-i2);
                write_to(connfd2, snd, n);
            }

            int w1 = 0, w2 = 0;
            for(i=0; i<4; i++)
            {
                if(current_state[i] == 0)
                    w2++;
                
                if(current_state[i+6] == 0)
                    w1++;
            }
            
            if(w1 != 0 || w2 != 0)
            {
                if(w1 == 4)
                {
                    winner = 1;
                    kill(getpid(), SIGUSR1);
                    break;
                }

                if(w2 == 4)
                {
                    winner = 2;
                    kill(getpid(), SIGUSR1);
                    break;
                }
            }
        }
    }

    pthread_exit(NULL);
    close(connfd);
}

#endif /* _SOLITARE_FUNC_H_ */
