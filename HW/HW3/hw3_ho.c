#include <stdio.h>      // fprintf(), perror()
#include <stdlib.h>     // exit()
#include <signal.h>
#include <string.h>     // memset() 
#include <stddef.h>      //null
#include <fcntl.h>     // open()
#include <unistd.h>    // read(), write(), close()
#include <stdbool.h>
#include <sys/time.h> //time


#include <sys/socket.h> // socket(), connect()
#include <netinet/in.h> // struct sockaddr_in
#include <arpa/inet.h>  // htons()

#include <signal.h> //signal()
#include <sys/wait.h>  //waitpid()
#include <pthread.h> //thread()
#include <semaphore.h>


static int timer_count = 0;
int order=0,order_now=0,total_income=0;
int total_pancake=0,total_blacktea=0;
int sufu=3,machine_pancake=2,machine_blacktea=1;
pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex2 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex3 = PTHREAD_MUTEX_INITIALIZER;
// pthread_mutex_t mutex4 = PTHREAD_MUTEX_INITIALIZER;
// pthread_mutex_t mutex5 = PTHREAD_MUTEX_INITIALIZER;
// pthread_mutex_t mutex6 = PTHREAD_MUTEX_INITIALIZER;
// pthread_mutex_t mutex7 = PTHREAD_MUTEX_INITIALIZER;

sem_t semaphore;


void remove_semaphore(){ 
    int rc; 
    
    printf ( "Main thread clean up mutex\n" ) ;
    rc = pthread_mutex_destroy(&mutex1 ) ;
    rc = pthread_mutex_destroy(&mutex2 ) ;

}

void handler(int signum) {
    remove_semaphore();
    while (waitpid(-1, NULL, WNOHANG) > 0);
}

void my_handler(int s){
    printf("exit \n");

    FILE *fPtr = fopen("result.txt", "w");
    /*
    fputs("%d\n",total_income, fPtr);
    */
    fprintf(fPtr,"customer: %d\n",order);
    fprintf(fPtr,"pancake: %d\n",total_pancake);
    fprintf(fPtr,"blacktea: %d\n",total_blacktea);
    fprintf(fPtr,"income: %d$\n",total_income);


    fclose(fPtr);
    exit(1); 

}


void timer_handler(int signo)
{
    timer_count++;
}


void *socketThread(void *t)
{
    
    int menu=0;
    int n=0,num_pancake=0,num_blacktea=0,num_order=0;
    int custom=0;
    //int acc_sock = *((int*)t);
    int acc_sock = t;
    //printf("%d\n",acc_sock);
    char buff[1024],rcv[1024],snd[30];
    int time_pancake=0,time_blacktea=0;
    bool make_pancake=false,make_blacktea=false;
    char command[20];
    while(true)
    {
        
        memset(rcv, 0, 1024);
        read(acc_sock, rcv, 1024);
        printf("%s",rcv);
        if(strstr(rcv, "menu"))
        {
            n=sprintf(snd,"pancake:40$\nblacktea:30$");
            write(acc_sock, snd, n+1);
        }
        else if(strstr(rcv, "pancake"))
        {
            
            num_order=0;
            
            sscanf(rcv, "%s %d", buff,  &num_order);
            memset(buff, '\0', 1024);
            num_pancake += num_order;
            n=sprintf(snd, "p%db%d", num_pancake, num_blacktea);
            write(acc_sock, snd,n+1);
            
        }
        else if(strstr(rcv, "blacktea"))
        {
            
            num_order=0;
            sscanf(rcv, "%s%d", buff,  &num_order);
            memset(buff, '\0', 1024);
            num_blacktea += num_order;
            
            n=sprintf(snd, "p%db%d", num_pancake, num_blacktea);
            write(acc_sock, snd,n+1);
            
        }
        else if(strstr(rcv, "cancel"))
        {
            num_pancake=0;
            num_blacktea=0;
            
        }
        else if(strstr(rcv, "checkout"))
        {

            pthread_mutex_lock( &mutex1 );
            order++;
            order_now++;
            custom=order;
            
            pthread_mutex_unlock( &mutex1 );
            if(order_now<=5)
            {
                int time_now=timer_count;

                
                //printf("\nclient checkout:%d\n",custom);
                
                n=sprintf(snd, "%.04d|%d$", custom,40*num_pancake+30*num_blacktea);
                //printf("%s",snd);
                write(acc_sock, snd, n+1);
                
                time_pancake=num_pancake*3;
                time_blacktea=num_blacktea*2;
                
                while(time_pancake>0||time_blacktea>0)
                {   
                    
                    //pthread_mutex_lock(&mutex2);
                    
                    pthread_mutex_lock(&mutex2);
                    if(time_pancake>0&&make_pancake==false&&sufu>0&&machine_pancake>0)
                    {
                        sufu--;
                        machine_pancake--;
                        make_pancake=true;
                    }

                    if(time_blacktea>0&&make_blacktea==false&&sufu>0&&machine_blacktea>0)
                    {
                        
                        sufu--;
                        machine_blacktea--;

                        make_blacktea=true;
                        
                    }
                    // pthread_mutex_unlock(&mutex2);
                    if(make_pancake&&(timer_count>time_now))
                    {
                        time_pancake--;
                        if(time_pancake%3==0&&make_pancake==true)
                        {
                            // pthread_mutex_lock(&mutex2);
                            total_pancake++;
                            n=sprintf(snd, "%.04d|pancake", custom);
                            printf("%s\n",snd);
                            write(acc_sock, snd, n+1);
                            
                            if(time_pancake==0)
                            {
                                sufu++;
                                machine_pancake++;
                                make_pancake=false;
                            }
                            // pthread_mutex_unlock(&mutex2);
                        }
                    }
                        
                    if(make_blacktea&&(timer_count>time_now))
                    {
                        time_blacktea--;
                        if(time_blacktea%2==0&&make_blacktea==true)
                        {
                            // pthread_mutex_lock(&mutex2);
                            total_blacktea++;
                            n=sprintf(snd, "%.04d|blacktea", custom);
                            printf("%s\n",snd);
                            write(acc_sock, snd, n+1);
                            
                            if(time_blacktea==0)
                            {
                                sufu++;
                                machine_blacktea++;
                                make_blacktea=false;
                            }
                            // pthread_mutex_unlock(&mutex2);
                        }
                    }
                    if(make_blacktea||make_pancake)
                        time_now=timer_count;
                    // if(make_blacktea||make_pancake)
                    // {
                            
                    //     printf("\nmaking%d",custom);
                    // }
                    
                    
                    
                    
                    pthread_mutex_unlock(&mutex2);
                    sleep(1);
                }
                
                pthread_mutex_lock( &mutex1 );
                total_income+=40*num_pancake+30*num_blacktea;
                order_now--;
                pthread_mutex_unlock( &mutex1 );
                num_pancake=0;
                num_blacktea=0;
                //printf("%s\n",snd);
                n=sprintf(snd, "%.04d|done", custom);
                //printf("%s\n",snd);
                if((n = write(acc_sock,snd, n+1))==-1){
                    
                }
                fgets(command, 1023, stdin);
                if(strstr(command, "exit"))
                    printf("%d\n",total_income);
                //write(acc_sock, snd, n+1);
                //printf("order_now_______%d\n",custom); 
                
            }
            else 
            {
                
                //memset(buff, 0, 1024);
                n=sprintf(snd,"wait or not?");
                //printf("\nclient choose:%d\n",order);
                write(acc_sock, snd, n+1);
                //memset(rcv, 0, 1024);
                read(acc_sock, rcv, 24);
                //printf("%s",rcv);
                
                if(strstr(rcv, "wait"))
                {
                    int time_now=timer_count;
                    //printf("\nclient wait:\n");
                    // order++;
                    // custom=order;
                    // order_now++;
                    //printf("\nclient checkout:%d\n",custom);
                    //memset(rcv, '\0', 1024);
                    //memset(snd, '\0', 1024);
                    n=sprintf(snd, "%.04d|%d$", custom,40*num_pancake+30*num_blacktea);
                    write(acc_sock, snd, n+1);
                    // printf("\n%d\n",num_pancake);
                    // printf("%d\n",num_blacktea);
                    time_pancake=num_pancake*3;
                    time_blacktea=num_blacktea*2;
                    while(time_pancake>0||time_blacktea>0)
                    {   
                        
                        //printf("1\n");
                        pthread_mutex_lock(&mutex2);
                        if(time_pancake>0&&make_pancake==false&&sufu>0&&machine_pancake>0)
                        {
                           
                            sufu--;
                            machine_pancake--;
                            make_pancake=true;
                            

                        }
                        if(time_blacktea>0&&make_blacktea==false&&sufu>0&&machine_blacktea>0)
                        {
                            
                            sufu--;
                            machine_blacktea--;
                            make_blacktea=true;
                            
                        }
                        //pthread_mutex_unlock(&mutex2);
                        if(make_pancake&&(timer_count>time_now))
                        {
                            time_pancake--;
                            if(time_pancake%3==0&&make_pancake==true)
                            {
                                // pthread_mutex_lock(&mutex2);
                                total_pancake++;
                                n=sprintf(snd, "%.04d|pancake", custom);
                                printf("%s\n",snd);
                                write(acc_sock, snd, n+1);
                                
                                if(time_pancake==0)
                                {
                                    sufu++;
                                    machine_pancake++;
                                    make_pancake=false;
                                }
                                // pthread_mutex_unlock(&mutex2);
                            }
                        }
                            
                        if(make_blacktea&&(timer_count>time_now))
                        {
                            time_blacktea--;
                            if(time_blacktea%2==0&&make_blacktea==true)
                            {
                                // pthread_mutex_lock(&mutex2);
                                total_blacktea++;
                                n=sprintf(snd, "%.04d|blacktea", custom);
                                printf("%s\n",snd);
                                write(acc_sock, snd, n+1);
                                
                                if(time_blacktea==0)
                                {
                                    sufu++;
                                    machine_blacktea++;
                                    make_blacktea=false;
                                }
                                // pthread_mutex_unlock(&mutex2);
                            }
                        }
                        if(make_blacktea||make_pancake)
                            time_now=timer_count;
                        pthread_mutex_unlock(&mutex2);
                    
                        printf("waiting_done\n");
                    }
                    pthread_mutex_lock( &mutex1 );
                    total_income+=40*num_pancake+30*num_blacktea;
                    order_now--;
                    num_pancake=0;
                    num_blacktea=0;
                    //memset(snd, '\0', 1024);
                    //printf("%s\n",snd);
                    int n=sprintf(snd, "%.04d|done", custom);

                    write(acc_sock, snd, n+1);
                    //printf("%d\n",n);
                    pthread_mutex_unlock( &mutex1 );
                    fgets(command, 1023, stdin);
                    if(strstr(command, "exit"))
                        printf("%d\n",total_income);

                    
                }
                else if(strstr(rcv, "cancel"))
                {
                    pthread_mutex_lock( &mutex1 );
                    //printf("cancel_order%d",order);
                    
                    order--;
                    //custom=order;
                    order_now--;
                    
                    num_pancake=0;
                    num_blacktea=0;
                    pthread_mutex_unlock( &mutex1 );
                    
                    // memset(buff, 0, 1024);
                    // sprintf(buff,"Order Canceled");
                    // write(acc_sock, buff, 1024);


                }
                
                
            }
            
            
            //printf("sufu: %d",sufu);
            
        }

        
        //printf("%d\n",total_income);
        
        
    }

    
    pthread_exit(NULL);
    close(acc_sock);
}

int main(int argc, char *argv[]) {
    
    
    // exit signal
    struct sigaction sigIntHandler;
    sigIntHandler.sa_handler = my_handler;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;
    sigaction(SIGINT, &sigIntHandler, NULL);

    //signal(SIGVTALRM, timer_handler);
    //timer signal
    struct sigaction sa;
    struct itimerval timer;

    memset (&sa, 0, sizeof(sa));
    sa.sa_handler = timer_handler;
    sa.sa_flags = 0;
    sigaction(SIGALRM,&sa, NULL);

    timer.it_value.tv_sec = 1;
    timer.it_value.tv_usec = 0;

    timer.it_interval.tv_sec = 1;
    timer.it_interval.tv_usec = 0;
    setitimer(ITIMER_REAL, &timer, NULL);

    int connfd,acc_sock;
    struct sockaddr_storage serverStorage;
    socklen_t addr_size;
    pthread_t tid[80];
    
    int i = 0,t = 0,rc;
    pthread_mutex_init(&(mutex1),NULL);  
    pthread_mutex_init(&(mutex2),NULL);  
    
    //signal(SIGCHLD, handler);
    // create socket 
    if ((connfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket()");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in serverAddr;

    struct sockaddr_in cli_addr = {0};
    socklen_t cli_addrlen = sizeof(cli_addr);
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons((u_short)atoi(argv[1]));
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);  
    
    int yes = 1;
    setsockopt(connfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));

    bind(connfd, (struct sockaddr *) &serverAddr, sizeof(serverAddr));
    addr_size = sizeof serverStorage; 

    
    //listen for connection
    listen(connfd,80);
    
    
    
    while(1)
    {
        
        
        acc_sock = accept(connfd, (struct sockaddr *)&cli_addr, &cli_addrlen);
        
        rc = pthread_create(&tid[t], NULL, socketThread, (void *)acc_sock);
        t++;
        
        //printf("\nclients:%d\n",t);
        
        if(rc)
        {
            printf("ERROR: return code from pthread_create() is %d\n", rc);
            exit(-1);
        }

        /*
        fgets(command, 1023, stdin);
        if(strstr(command, "exit"))
            printf("%d\n",total_income);
        */
        //signal(SIGCHLD, handler);
    }
    
    pthread_exit(NULL);
    close(acc_sock);
    close(connfd);
    
    return 0;
    
}