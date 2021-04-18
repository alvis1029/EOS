/*
* waitpid.c -- shows how to get child ’s exit status
*/

#include <errno.h>          /*  Errors  */
#include <stdio.h>          /*  Input/Output  */
#include <stdlib.h>         /*  General Utilities  */
#include <sys/types.h>      /*  Primitive System Data Types  */
#include <sys/wait.h>       /*  Wait for Process Termination  */
#include <unistd.h>         /*  Symbolic Constants  */
#include <time.h>           /*  Time functions  */

pid_t childpid;     /* variable to store the child ’s pid */

void childfunc(void)
{
    int randtime;   /* random sleep time */
    int exitstatus;  /* random exit status */

    printf("Child: I am the child process!\n");
    printf("Child: My PID is: %d\n", getpid());

    /* sleep */
    srand(time(NULL));
    randtime = rand() % 5;
    printf("Child: Sleeping for %d second ...\n", randtime);
    sleep(randtime);

    /* rand exit status */
    exitstatus = rand() % 2;
    printf("Child: Exit status is %d\n", exitstatus);

    printf("Child: Goodbye!\n");
    exit(exitstatus)    /* child exits with user-provided return code */
}

void parentfunc(void)
{
    int status;   /* child's exit status */
    pid_t pid;

    printf("Parent: I am the parent process!\n");
    printf("Parent: My PID is: %d\n", getpid());
    printf("Parent: I will now wait for my child to exit.\n");
    
    wait(&status);
    printf("Parent: Child's exit code is: %d\n", WEXITSTATUS(status));
    printf("Parent: Goodbye!\n");

    exit(0)    /* parent exits */
}

int main(int argc, char *argv[])
{
    /* now create new process */
    childpid = fork();

    if(childpid >= 0)
    {
        /* fork succeeded */
        if(childpid == 0)
        {
            /* fork() returns 0 to the child process */
            childfunc();
        }
        else
        {
            /* fork() returns new pid to the parent process */
            parentfunc();
        }
    }
    else
    {
        /* fork returns -1 on failure */
        perror("fork");     /* display error message */
        exit(0);
    }

    return 0;
}