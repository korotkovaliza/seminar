#define _GNU_SOURCE
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <poll.h>
#define S 1024

int main(int argc, char ** argv){
	
	if( argc < 2 ){
		printf("ERROR\n");
		return 0;
	}
	
	int n = argc - 1;
	int * f = (int*) calloc(n , sizeof(int));
	
	for (int i = 0; i < n; ++i)
	{
		f[i] = mkfifo(argv[i+1], O_CREAT | 0666);
		if (f[i] == -1)
		{
			printf("error mkfifo %s\n", argv[i+1]);
		}
	}
	int count = 0;
	for(int i = 1; i < argc; i++){
		f [i - 1]= open(argv[i],O_NONBLOCK | O_RDONLY);
		if(f [i - 1] <= 0){
			printf("error opening %s\n", argv[i]);
			count++;
		}
	}
	if (count == n){
		printf("All files were not opened \n");
		exit(-1);
		}
		
	
	struct pollfd *fdpoll = calloc(n, sizeof(struct pollfd));

	
	for (int i = 0; i < n; i++) {
		fdpoll[i].fd = f[i];
		fdpoll[i].events = POLLIN;
	}
	
	char* k =  calloc(S , sizeof(char));
	
	 while ( poll(fdpoll, n, -1) != -1) {

		for (int i = 0; i < n; i++) {
			if (fdpoll[i].revents & POLLIN){
				int R = read(f[i], k, S);
				if (R == -1) {
					printf("error reading %s\n", argv[i+1]);
					R = 0;
				}
				else if (R != 0) { 
					int W = write(STDOUT_FILENO, k, R);
					if (W == -1) 
						printf("error writing\n");
				}
			}
		}
	}
	
	free(f);
	free(fdpoll);
	free(k);
	return 0;
}
