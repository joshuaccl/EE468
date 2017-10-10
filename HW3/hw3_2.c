#include <stdio.h>
#include "thread.h"

static void period(int n);
static void ask(int n);

static thread_t threads[2];

int main(int argc, char **argv) {
	thread_create(&(threads[0]), &period, 1);
	thread_create(&(threads[1]), &ask, 1);
	thread_join(threads[0]);
	thread_join(threads[1]);
}

void period(int n) {
	if(n==1) {
		int counter = 0;
		while(1) {
			if(counter % 10000000 == 0) printf(".\n");
			counter++;
		}
	}
}

void ask(int n) {
	if(n==1) {
		char a[100];
		while(1) {
			printf("Please enter some text less than 100 characters: \n");
			scanf("%s", a);
			printf("Thank you for your input: %s\n", a);
		}
	}
}
