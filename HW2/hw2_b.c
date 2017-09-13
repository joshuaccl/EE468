#include <stdio.h>


main() {

int pid;

pid = fork();

printf("First print %d\n",getpid());
if (pid == 0) { /* child process */
     printf("In child process %d\n", getpid());
     fork();
     printf("Thread created by process %d\n",getpid());
}
fork();
printf("Second print %d\n", getpid());

}
