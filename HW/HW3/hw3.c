#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <pthread.h>
#include <semaphore.h>

int total_order = 0, total_earn = 0, working_order = 0, total_p = 0, total_b = 0;
int p_mech = 0, b_mech = 0, employee = 0;
int count = 0;
pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex2 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex3 = PTHREAD_MUTEX_INITIALIZER;

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

void remove_semaphore()
{ 
    printf ("Main thread clean up mutex\n");
    pthread_mutex_destroy(&mutex1);
    pthread_mutex_destroy(&mutex2);
    pthread_mutex_destroy(&mutex3);
}

void tax_handler(int signum)
{
    if(total_earn < 100)
        printf("Taxing %.2f$\n", total_earn*0.0);
    else if(total_earn < 150)
        printf("Taxing %.2f$\n", total_earn*0.16);
    else if(total_earn < 500)
        printf("Taxing %.2f$\n", total_earn*0.18);
    else
        printf("Taxing %.2f$\n", total_earn*0.2);
}

void ctrlc_handler(int signum)
{
    printf("\nexit\n");

    FILE *fPtr = fopen("result.txt", "w");

    fprintf(fPtr,"customer: %d\n",total_order);
    fprintf(fPtr,"pancake: %d\n",total_p);
    fprintf(fPtr,"blacktea: %d\n",total_b);
    fprintf(fPtr,"income: %d$\n",total_earn);

    fclose(fPtr);
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
    printf("Timer expired %d times\n", count);
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
    
    printf("%s\n", buf);
}

void *waffle_house(void *t)
{
    int connfd = t;
    int n, p_mech_mod = 0, b_mech_mod = 0, e_mod = 0, start = 0;
    int is_order = 0, local_count = -1, work = 1, is_same_sec = 0;
    int p = 0, b = 0, num;
    int p_doing = 0, b_doing = 0;
    int local_order_number;
    char snd[1024], rcv[1024], res[1024];

    while(work)
    {
        read_from_client(connfd, rcv);
        
        if(strstr(rcv, "menu"))
        {
            n = sprintf(snd, "pancake:40$\nblacktea:30$");
            write_to_client(connfd, snd, n);
        }
        if(strstr(rcv, "cancel"))
        {
            // printf("Client %.04d return cancel!\n", local_order_number);
            is_order = 0;
            work = 0;
            pthread_mutex_lock(&mutex1);
            working_order--;
            total_order--;
            total_earn -= p*40+b*30;
            total_p -= p;
            total_b -= b;
            pthread_mutex_unlock(&mutex1);
            break;           
        }
        if(strstr(rcv, "checkout"))
        {
            pthread_mutex_lock(&mutex1);
            working_order++;
            total_order++;
            total_earn += p*40+b*30;
            total_p += p;
            total_b += b;
            pthread_mutex_unlock(&mutex1);
            local_order_number = total_order;

            if(working_order > 5)
            {
                n = sprintf(snd, "wait or not?");
                write_to_client(connfd, snd, n);
            }
            else
            {   
                n = sprintf(snd, "%.04d|%d$", local_order_number, p*40+b*30);
                write_to_client(connfd, snd, n);
                is_order = 1;
            }
        }
        if(strstr(rcv, "pancake"))
        {   
            sscanf(rcv, "%s%d", res, &num);
            p += num;
            n = sprintf(snd, "p%db%d", p, b);
            write_to_client(connfd, snd, n);
        }
        if(strstr(rcv, "blacktea"))
        {
            sscanf(rcv, "%s%d", res, &num);
            b += num;
            n = sprintf(snd, "p%db%d", p, b);
            write_to_client(connfd, snd, n);
        }
        if(strstr(rcv, "wait"))
        {
            n = sprintf(snd, "%.04d|%d$", local_order_number, p*40+b*30);
            write_to_client(connfd, snd, n);
            is_order = 1;
            // printf("Client %.04d return wait!\n", local_order_number);
        }
        if(strstr(rcv, "tax"))
        {
            signal(SIGUSR1, tax_handler);
            kill(getpid(), SIGUSR1);
        }

        while(is_order)
        {   
            if(p == 0 && b == 0)
            {
                sleep(0.01);
                printf("Client %.04d is finished!!!\n", local_order_number);
                n = sprintf(snd, "%.04d|done", local_order_number);
                write_to_client(connfd, snd, n);
                is_order = 0;
                work = 0;
                start = 0;
                break;
            }
            
            if(p > 0)
            {   
                if(p_doing == 0 && p_mech < 2 && employee < 3)
                {   
                    start = 1;
                    p_mech_mod = 1;
                    e_mod += 1;
                    p_doing = 1;
                }
                if(p_doing == 1 && count > 0 && count%3 == 0 && is_same_sec == 0)
                {
                    sleep(0.01);
                    n = sprintf(snd, "%.04d|pancake", local_order_number);
                    write_to_client(connfd, snd, n);
                    p -= 1;
                    p_doing = 0;
                    p_mech_mod = -1;
                    e_mod += -1;
                    is_same_sec = 1;
                    printf("Client %.04d Pancake finish one!\n", local_order_number);
                }
            }
            
            if(b > 0)
            {   
                if(b_doing == 0 && b_mech < 2 && employee < 3)
                {   
                    start = 1;
                    b_mech_mod = 1;
                    b_doing = 1;
                    e_mod += 1;
                }
                if(b_doing == 1 && count > 0 && count%2 == 0 && is_same_sec == 0)
                {
                    sleep(0.01);
                    n = sprintf(snd, "%.04d|blacktea", local_order_number);
                    write_to_client(connfd, snd, n);
                    b -= 1;
                    b_doing = 0;
                    b_mech_mod = -1;
                    e_mod += -1;
                    is_same_sec = 1;
                    printf("Client %.04d Blacktea finish one!\n", local_order_number);
                    
                }   
            }
            
            if(start == 1)
            {
                pthread_mutex_lock(&mutex2);
                p_mech += p_mech_mod;
                b_mech += b_mech_mod;
                employee += e_mod;
                pthread_mutex_unlock(&mutex2);
                p_mech_mod = 0;
                b_mech_mod = 0;
                e_mod = 0;
            }

            if(local_count < count)
            {
                local_count = count;
                is_same_sec = 0;
            }

            // printf("p_mech is %d  b_mech is %d  employee is %d\n", p_mech, b_mech, employee);
            // printf("Client %d still has %d pancake %d blacktea\n", local_order_number, p, b);
            // printf("Client %.04d start: %d\n", local_order_number, start);
            // printf("%d\n", counter);
            // printf("Now have %d p_mech | %d b_mech | %d employee is working\n", p_mech, b_mech, employee);
        }

        if(work == 0)
        {
            pthread_mutex_lock(&mutex1);
            working_order--;
            pthread_mutex_unlock(&mutex1);
            // printf("working_order is %d\n", working_order);
        }
    }
    char str[10];
    fgets(str, 10, stdin);
    if(strstr(str, "exit"))
        printf("Total earn $%d\n", total_earn);

    pthread_exit(NULL);
    close(connfd);
}

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
    
    // struct sigaction my_action;
    // my_action.sa_handler = tax_handler;
    // sigemptyset(&my_action.sa_mask);
    // my_action.sa_flags = 0;
    // sigaction(SIGUSR1, &my_action, NULL);


    int sockfd , connfd; /* socket descriptor */
    struct sockaddr_in addr_cln;
    socklen_t sLen = sizeof(addr_cln) ;
    pthread_t threads[32]; 
    int rc, t = 0;
    pthread_mutex_init(&(mutex1),NULL);  
    pthread_mutex_init(&(mutex2),NULL);
    pthread_mutex_init(&(mutex3),NULL);

    if(argc != 2)
        printf("Usage: ./hw2 <port>\n");

    /* create socket and bind socket to port */
    sockfd = passivesock(argv[1], "tcp", 80);

    while(1)
    {   
        /* waiting for connection */
        connfd = accept(sockfd, (struct sockaddr *) &addr_cln, &sLen);

        if(connfd == -1)
            printf("Error: accept()\n");

        rc = pthread_create(&threads[t], NULL, waffle_house, (void *)connfd);
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
