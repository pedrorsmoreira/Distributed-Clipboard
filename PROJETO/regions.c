#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <pthread.h>

#include "clipboard.h"
#include "regions.h"

REG regions[REGIONS_NR];

//COMMENT THIS CODE WHEN BOTH CASES ARE WRITTEN
void regions_init(){
	// if (working alone)
		for (int i = 0; i <REGIONS_NR; i++)
			regions[i].message = NULL;
	// else .....
}


void update_region( int client_fd, Smessage data, int data_size){
	if ( regions[data.region].message != NULL)
		free(regions[data.region].message);

	regions[data.region].size = data.message_size;
	regions[data.region].message = (void *) malloc (data.message_size);
	if ( regions[data.region].message == NULL){
		printf ("malloc failure\n");
		exit (-1);
	}

	//read the message and copy it
	if ( read(client_fd, regions[data.region].message, data.message_size) < 0){
		perror("read: ");
		exit(-1);
	}
	
	//TEMPORARY PRINT FOR TESTING - TO BE DELETED
	printf("copied %s to region %d\n", (char *) regions[data.region].message, data.region);
}

void send_region(int client_fd, Smessage data, int data_size){
	//check if there's anything to paste
	if (regions[data.region].message == NULL){
		printf("nothing to paste in region %d \n", data.region);
		data.region = -1;
	}
	else
		data.message_size = regions[data.region].size;
	
	//send back the message info
	if ( write(client_fd, &data, data_size) < 0){
		perror("write: ");
		exit(-1);
	}

	if (data.region == -1)	
		return;
	
	//send the message requested
	if ( write(client_fd, regions[data.region].message, data.message_size) < 0){
		perror("write: ");
		exit(-1);
	}
	
}
