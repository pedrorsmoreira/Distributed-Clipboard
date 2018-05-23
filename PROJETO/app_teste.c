#include "clipboard.h"
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#define MESSAGE_SIZE 10000

int read_region(){
	int region;
	char aux[50];

	while(1){
		printf("Introduce the region where to act(from 0 to %d): ", REGIONS_NR -1);
		if (fgets(aux, 50, stdin) == NULL){
			perror("fgets: ");
			exit(-1);
		}
		sscanf(aux, "%d", &region);
		if ((region >= 0) && (region < REGIONS_NR))	break;
		printf("Invalid region\n---\n");
	}
	return region;
}

int main(){

	int region = 2;
	char palavra[3] = "ol";
int fd = clipboard_connect("./");
int ret1 = clipboard_copy(fd, region, NULL, 10);
int ret2 = clipboard_copy(fd, region + 1, palavra, 10);

printf("retornou %d e %d\n", ret1, ret2);

return 0;

}
	/*
		//variables to hold the info from stdin
		char aux[50], message[MESSAGE_SIZE];
		//get the FIFOs file descriptor
		int region, nbytes;

		int fd = clipboard_connect("./");
		if(fd== -1){
			printf("Clipboard is off\n");
			exit(-1);
		}
		while(1){
			//read the action from stdin and perform it
			while (1){
			char order;
				printf("(Enter [Q] to quit) \nIntroduce the action, copy [C], paste [P] or wait [W]:");
				if (fgets(aux, sizeof(aux), stdin) == NULL){
					perror("fgets: ");
					exit(-1);
				}
				sscanf(aux, "%c", &order);
				//perform
				if ((order == 'q') || (order == 'Q')){
					clipboard_close(fd);
					return 0;
				}
				else if ((order == 'c') || (order == 'C')){
					//read the region of the action from stdin
					region = read_region();
					
					printf("Write the message to be copied:");	
					if (fgets(message, MESSAGE_SIZE, stdin) == NULL){
						perror("fgets: ");
						exit(-1);
					}
					nbytes = clipboard_copy(fd, region, message, strlen(message)+1);
					if (nbytes == 0)	printf("copy failed\n---\n");
					else	printf("%d bytes copied\n---\n", nbytes);
					break;
				}
				else if ((order == 'P') || (order == 'p')){
					//read the region of the action from stdin
					region = read_region();
					
					nbytes = clipboard_paste(fd, region, message, MESSAGE_SIZE);
					if (nbytes == 0)	printf("paste failed\n---\n");
					else	printf("Received %s\n (%d bytes pasted)\n---\n", message, nbytes);
					
					break;
				}
				else if ((order == 'W') || (order == 'w')){
					//read the region of the action from stdin
					region = read_region();
					
					nbytes = clipboard_wait(fd, region, message, MESSAGE_SIZE);
					if (nbytes == 0)	printf("paste (wait) failed\n---\n");
					else	printf("Received %s\n (%d bytes pasted (wait))\n---\n", message, nbytes);
					
					break;
				}
				printf("Option not available\n---\n");
			}
		}
	exit(-5);
	}

	*/