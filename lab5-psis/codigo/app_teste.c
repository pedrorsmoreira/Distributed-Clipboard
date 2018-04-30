#include "clipboard.h"
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>



int main(){
		//variables to hold the info from stdin
		char aux[sizeof(int)], message[MESSAGE_SIZE];
		//get the FIFOs file descriptor
		int region, fd = clipboard_connect("./");
		if(fd== -1){
			printf("clipboard is off\n");
			exit(-1);
		}
		
		//read the region of the action from stdin
		while(1){
			printf("introduce the region where to act(from 0 to %d):\n", REGIONS_NR -1);
			fgets(aux, sizeof(int), stdin);
			sscanf(aux, "%d", &region);
			if ((region >= 0) && (region < REGIONS_NR))	break;
			printf("invalid region\n");
		}
		//read the action from stdin and perform it
		while (1){
		char order;
			printf("introduce the action, copy[C] or paste [P]:");
			scanf("%c", &order);
			//perform
			if ((order == 'c') || (order == 'C')){
				printf("write the message to be copied:");
				fgets(message, MESSAGE_SIZE, stdin);	//cobrir falha?!?!?!?!?
				fgets(message, MESSAGE_SIZE, stdin);	//WWWWWWWWWWWWWWWWWWWHHHHHHHHHHHHHHHHHHHHHHHHHHYYYYYYYYYYYYYYYYYYYYYYYYY?!?!?!??!?!?!?!?!?!?!?!?!?!?!?!?!?!??!
				clipboard_copy(fd, region, message, MESSAGE_SIZE);
				break;
			}
			if ((order == 'P') || (order == 'p')){
				clipboard_paste(fd, region, message, MESSAGE_SIZE);
				break;
			}
			printf("Option not available\n");
		}
		
		printf("Received %s\n", message);
	
	close(fd);
	close(fd+1);	
	exit(0);
	}
