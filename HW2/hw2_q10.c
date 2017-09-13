#include <stdio.h>

// Program 1

/*
main() {
	int val = 5;
	int pid;

	if(pid=fork()) wait(pid);
	val++;
	printf("%d and %d\n", val, getpid());
	return val;
}
*/

// Program 2
main() {
	int val = 5;
	int pid;

	if(pid=fork()) wait(pid);
	else exit(val);
	val++;
	printf("%d and %d\n", val, getpid());
	return val;
}
