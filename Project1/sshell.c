/*
 *  This is a simple shell program from
 *  rik0.altervista.org/snippetss/csimpleshell.html
 *  It's been modified a bit and comments were added.
 *
 *  It doesn't allow misdirection, e.g., <, >, >>, or |
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#define BUFFER_SIZE 80
#define ARR_SIZE 80

 #define DEBUG 1  /* In case you want debug messages */

void parse_args(char *buffer, char** args, 
		size_t args_size, size_t *nargs)
{
	/* 
	 * size_t data type is defined in the 1999 ISO C standard (C99).
	 * It is used to represent the sizes of objects. size_t is the
	 * preferred way to declare arguments or variables that hold the
	 * size of an object.
	 */
	char *buf_args[args_size]; /* You need C99.  Note that args_size
				      is normally a constant. */
	char **cp;  /* This is used as a pointer into the string array */
	char *wbuf;  /* String variable that has the command line */
	size_t i, j; 

	wbuf=buffer;
	buf_args[0]=buffer; 
	args[0] =buffer;
	/*
	 * Now 'wbuf' is parsed into the string array 'buf_args'
	 *
	 * The for-loop uses a string.h function
	 *   char *strsep(char **stringp, const char *delim);
	 *
	 *   Description:  
	 *   If *stringp = NULL then it returns NULL and does
	 *   nothing else.  Otherwise the function finds the first token in
	 *   the string *stringp, where tokens are delimited by symbols
	 *   in the string 'delim'.  
	 *
	 *   In the example below, **stringp is &wbu, and 
	 *   the delim = ' ', '\n', and '\t'.  So there are three possible 
	 *   delimiters. 
	 *
	 *   So in the string " Aloha World\n", the spaces and "\n" are
	 *   delimiters.  Thus, there are three delimiters.  The tokens
	 *   are what's between the delimiters.  So the first token is
	 *   "", which is nothing because a space is the first delimiter.
	 *   The second token is "Aloha", and the third token is "World".
	 *   
	 *   The function will scan a character string starting from
	 *   *stringp, search for the first delimiter.  It replaces
	 *   the delimiter with '\0', and *stringp is updated to point
	 *   past the token.  In case no delimiter was found, the
	 *   token is taken to be the entire string *stringp, and *stringp
	 *   is made NULL.   Strsep returns a pointer to the token. 
	 *
	 *   Example:  Suppose *stringp -> " Aloha World\n"
	 *
	 *   The first time strsep is called, the string is "\0Aloha World\n",
	 *   and the pointer value returned = 0.  Note the token is nothing.
	 *
	 *   The second time it is called, the string is "\0Aloha\0World\n",
	 *   and the pointer value returned = 1  Note that 'Aloha' is a token.
	 *
	 *   The third time it is called, the string is '\0Aloha\0World\0', 
	 *   and the pointer value returned is 7.  Note that 'World' is a token.
	 *
	 *   The fourth time it is called, it returns NULL.
	 *
	 *   The for-loop, goes through buffer starting at the beginning.
	 *   wbuf is updated to point to the next token, and cp is
	 *   updated to point to the current token, which terminated by '\0'.
	 *   Note that pointers to tokens are stored in array buf_args through cp.
	 *   The loop stops if there are no more tokens or exceeded the
	 *   array buf_args.
	 */   
	/* cp is a pointer to buff_args */ 
	for(cp=buf_args; (*cp=strsep(&wbuf, " \n\t")) != NULL ;){
		if ((*cp != '\0') && (++cp >= &buf_args[args_size]))
			break; 
	}

	/* 
	 * Copy 'buf_args' into 'args'
	 */    
	for (j=i=0; buf_args[i]!=NULL; i++){ 
		if(strlen(buf_args[i])>0)  /* Store only non-empty tokens */
			args[j++]=buf_args[i];
	}

	*nargs=j;
	args[j]=NULL;
}


int main(int argc, char *argv[], char *envp[]){
	char buffer[BUFFER_SIZE];
	char *args[ARR_SIZE];
	char *executable[ARR_SIZE];

	int *ret_status;
	size_t nargs;
	pid_t pid;
	int pipe_seen, i, j, k;
	int pipe_num;
	int pipe_location[8];

	struct single_pipe {
		int my_pipe[2];
		};

	struct single_pipe array_of_pipes[8];

	while(1){
		printf("ee468>> "); /* Prompt */
		fgets(buffer, BUFFER_SIZE, stdin); /* Read in command line */
		/* Parse the command line into args */
		parse_args(buffer, args, ARR_SIZE, &nargs); 

		if (nargs==0) continue; /* Nothing entered so prompt again */
		if (!strcmp(args[0], "exit" )) exit(0);       

		pipe_num = 0;
		pipe_seen = 0;
		#ifdef DEBUG		
		for(i = 0; args[i]!=NULL;i++){
			printf("The command is: %s\n",args[i]);
		}
		#endif 	
	
		for (  i = 0 ; args[i]!=NULL ; i++ ) {
			
			if(!strcmp(args[i] , "|")) {

				pipe_seen = 1;			
				pipe_location[pipe_num] = i;
				pipe_num++;	
			}
		}

		if(pipe_seen) {
			pipe(array_of_pipes[0].my_pipe);
			if(pipe_num==1) {
				// first command
				if( fork() == 0) {
					close(1);
					dup2( array_of_pipes[0].my_pipe[1], 1);
					close(array_of_pipes[0].my_pipe[0]);
					for(i=0; i<pipe_location[0]; i++) {
						executable[i] = args[i]; 
					}
					executable[i] = NULL;
				
					#ifdef DEBUG
						for(i = 0; executable[i]!=NULL; i++) {
							printf("The original executable is: %s\n",executable[i]);
						}
					#endif

					if(execvp(args[0], executable)) {
						puts(strerror(errno));
						exit(127);
					}
				}

				// second command
				if( fork() == 0) {
					close(0);
					dup2(array_of_pipes[0].my_pipe[0], 0);
					close(array_of_pipes[0].my_pipe[1]);
					j = 0;
					for(i=(pipe_location[0]+1); args[i] != NULL; i++) {
						executable[j] = args[i];
						j++;
					}
					executable[j] = NULL;

					#ifdef DEBUG
						for(i = 0; executable[i]!=NULL; i++) {
							printf("The second executable is: %s\n",executable[i]);
						}
					#endif


					if(execvp(executable[0], executable)) {
						puts(strerror(errno));
						exit(127);
					}
				}
				close(array_of_pipes[0].my_pipe[0]);
				close(array_of_pipes[0].my_pipe[1]);
				wait(ret_status);
				wait(ret_status);
			} else if(pipe_num>1){//more than one pipe
				pipe(array_of_pipes[0].my_pipe);
				//handle the first pipe
				if( fork() == 0) {
					close(1);	
					dup2( array_of_pipes[0].my_pipe[1], 1);
					close(array_of_pipes[0].my_pipe[0]);
					
					for(i=0; i<pipe_location[0]; i++) {
						executable[i] = args[i];
					}
					executable[i] = NULL;
					#ifdef DEBUG
						for(i = 0; executable[i]!=NULL; i++) {
							printf("The first executable is: %s\n",executable[i]);
						}
					#endif

					if(execvp(executable[0], executable)) {
						puts(strerror(errno));
						exit(127);
					}
				}
					
				// do the middle pipes
				for( i = 1 ; i<pipe_num ; i++) {
					pipe(array_of_pipes[i].my_pipe);
					
					//code for piping the data
					if( fork() == 0) { // this is the child
						close(0);
						close(1);
						dup2( array_of_pipes[i-1].my_pipe[0], 0);
						dup2( array_of_pipes[i].my_pipe[1], 1);
						close(array_of_pipes[i-1].my_pipe[1]);
						close(array_of_pipes[i].my_pipe[0]);

						k=0;
						// get the arguments
						for(j=(pipe_location[i]+1) ; j<pipe_location[i+1] ; j++) {
							executable[k] = args[j];
							k++;
						}
						executable[k]=NULL;
						#ifdef DEBUG
							for(j = 0; executable[j]!=NULL; j++) {
								printf("The %d executable is: %s\n",j,executable[j]);
							}
						#endif
				
						if(execvp(executable[0], executable)) {
							puts(strerror(errno));
							exit(127);
						}
					}
				/*	
					close(array_of_pipes[i-1].my_pipe[0]);
					close(array_of_pipes[i-1].my_pipe[1]);
				*/
				}

				// do the last command
				if( fork() == 0) { // this is the child
					close(0);
					dup2( array_of_pipes[pipe_num-1].my_pipe[0], 0);
					close(array_of_pipes[pipe_num-1].my_pipe[1]);
					k = 0;
					// get the last command
					for(j=pipe_location[pipe_num-1]+1 ; args[j] != NULL ; j++) {
						executable[k] = args[j];
						k++;
					}
					executable[k] = NULL;
					#ifdef DEBUG
						for(i = 0; executable[i]!=NULL; i++) {
							printf("The last executable is: %s\n",executable[i]);
						}
					#endif

					if(execvp(executable[0], executable)) {
						puts(strerror(errno));
						exit(127);
					}
				}
	
				// close all the pipes used
				for( k = 0; i<pipe_num ; i++){	
					close(array_of_pipes[i].my_pipe[0]);
					close(array_of_pipes[i].my_pipe[1]);
				}


/*
				close(array_of_pipes[0].my_pipe[0]);
				close(array_of_pipes[0].my_pipe[1]);
				close(array_of_pipes[pipe_num-1].my_pipe[1]);
				close(array_of_pipes[pipe_num-1].my_pipe[0]);
*/
				
				waitpid(-1, NULL, 0);
/*
				// wait for all children to die
				for( i = 0 ; i<pipe_num+1 ; i++) {
					wait(ret_status);
				}
*/
			} 
				
		}	
		if(!pipe_seen) {
			if( fork() == 0) { // this is the child
			
				if(execvp(args[0], args)) {
					puts(strerror(errno));
					exit(127);
				}
				
			}
			wait(ret_status);
		}
		
	}    
	return 0;
}


