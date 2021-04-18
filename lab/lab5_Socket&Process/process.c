/*
* process .c
*/

#include <errno.h>          /*  Errors  */
#include <stdio.h>          /*  Input/Output  */
#include <stdlib.h>         /*  General Utilities  */
#include <sys/types.h>      /*  Primitive System Data Types  */
#include <sys/wait.h>       /*  Wait for Process Termination  */
#include <unistd.h>         /*  Symbolic Constants  */

pid_t childpid;     /* variable to store the child ’s pid */

void childfunc(void)
{
    int retval;   /* user-provided return code */

    printf("Child: I am the child process!\n");
    printf("Child: My PID is: %d\n", getpid());
    printf("Child: My parent's PID is: %d\n", getppid());
    printf("Child: Sleeping for one second ...\n");
    sleep(1);

    printf("Child: Enter an exit value (0 to 255): ");
    scanf(" %d", &retval);
    printf("Child: Goodbye!\n");

    exit(retval)    /* child exits with user-provided return code */
}

void parentfunc(void)
{
    int status;   /* child's exit status */

    printf("Parent: I am the parent process!\n");
    printf("Parent: My PID is: %d\n", getpid());
    printf("Parent: My child's PID is: %d\n", childpid);
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