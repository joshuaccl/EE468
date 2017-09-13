#include <stdio.h>

void forkthem(int n);

main(int argc,char** argv){
	forkthem(5);
}

void forkthem(int n){
	if(n>0){
		printf("process number: %d\n",getpid());
		fork();
		forkthem(n-1);
	}
}
