/******************************************************************************
 *
 * File Name: buggy_app.c
 *	      (c) 2018 PSis
 * Authors: Pedro Moreira e Inês Gaspar
 *
 * DESCRIPTION
 *		app that lets the user define all parameters 
 *****************************************************************************/

#include "clipboard.h"
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include <signal.h>

#define MESSAGE_SIZE 100000


int read_region(){
	int region;
	char aux[50];
		printf("Introduce the region where to act(from 0 to %d): ", REGIONS_NR -1);
		if (fgets(aux, 50, stdin) == NULL){
			perror("fgets: ");
			exit(-1);
		}
		sscanf(aux, "%d", &region);

	return region;
}


int main(){
//variables to hold the info from stdin
	char aux[50], message[MESSAGE_SIZE];
	
	int region, nbytes, alloc, count;

	int fd = clipboard_connect("./");
	if(fd== -1){
		printf("Clipboard is off\n");
		exit(-1);
	}

while (1){
		char order;
			printf("(Enter [Q] to quit) \nIntroduce the action, copy [C], paste [P] or wait [W]:");
			if (fgets(aux, sizeof(aux), stdin) == NULL){
				perror("fgets: ");
				exit(-1);
			}
			sscanf(aux, "%c", &order);
			


			printf("enter the size of bytes to allocate:");
			if (fgets(aux, sizeof(aux), stdin) == NULL){
				perror("fgets: ");
				exit(-1);
			}
			sscanf(aux, "%d", &alloc);
			char *buf = (char *) malloc(alloc);
			if(buf == NULL)
				printf("falhou a alocação.\n");

			printf("enter the count  to be sent:");
			if (fgets(aux, sizeof(aux), stdin) == NULL){
				perror("fgets: ");
				exit(-1);
			}
			sscanf(aux, "%d", &count);


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

				strcpy(buf, message);

				nbytes = clipboard_copy(fd, region, buf, count);
				if (nbytes == 0)	printf("copy failed\n---\n");
				else	printf("%d bytes copied\n---\n", nbytes);
				break;
			}
			else if ((order == 'P') || (order == 'p')){
				//read the region of the action from stdin
				region = read_region();
				
				nbytes = clipboard_paste(fd, region, buf, count);
				if (nbytes == 0)	printf("paste failed\n---\n");
				else	printf("Received %s\n (%d bytes pasted)\n---\n", buf, nbytes);
				
				break;
			}
			else if ((order == 'W') || (order == 'w')){
				//read the region of the action from stdin
				region = read_region();
				
				nbytes = clipboard_wait(fd, region, buf, count);
				if (nbytes == 0)	printf("paste (wait) failed\n---\n");
				else	printf("Received %s\n (%d bytes pasted (wait))\n---\n", buf, nbytes);
				
				break;
			}
			printf("Option not available\n---\n");
		
			free(buf);
		}
	}