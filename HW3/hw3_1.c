/*
  * threadHello.c -- Simple multi-threaded program.
  *
  * Compile with
  *    > gcc -g -Wall -Werror -D_POSIX_THREAD_SEMANTICS threadHello.c -c -o threadHello.o
  *    > gcc -g -Wall -Werror -D_POSIX_THREAD_SEMANTICS thread.c -c -o thread.o
  *    > gcc -lpthread threadHello.o thread.o -o threadHello
  * Run with
  *    > ./threadHello
  */
#include <stdio.h>
#include "thread.h"

static void go(int n);

#define NTHREADS 10
static thread_t threads[NTHREADS];

int main(int argc, char **argv) {
    int i;
    long exitValue;

    for (i = 0; i < NTHREADS; i++){
        thread_create(&(threads[i]), &go, i);
    }
    for (i = 0; i < NTHREADS; i++){
        exitValue = thread_join(threads[i]);
        printf("Thread %d returned with %ld\n", 
                        i, exitValue);
    }
    printf("Main thread done.\n");
    return 0;
}

void go(int n) {
    printf("Hello from thread %d\n", n);
    thread_exit(100 + n);
    // Not reached
}

