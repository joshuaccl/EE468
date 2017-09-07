#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int staticVar = 0;

void main()
{
while(staticVar < 10) {
	staticVar++;
	sleep(10);
	printf("static address: %x, value %d\n", &staticVar, staticVar);
}
}
