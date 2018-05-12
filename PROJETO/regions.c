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

/**
 * @brief      initializes the local clipboard regions
 *
 * @param[in]  fd    file descriptor for the remote 
 * 					 clipboard to pull the regions from 
 * 					 (fd < 0  indicates single mode)
 */
void regions_init(int fd){
	int i;
	if (fd < 0){
		for (i = 0; i <REGIONS_NR; i++)
			regions[i].message = NULL;
	}
	else{
		for (i = 0; i <REGIONS_NR; i++)
			regions[i].size = clipboard_paste(fd, i, regions[i].message, -1);
	}
}

/**
 * @brief      updates a clipboard region
 *
 * @param[in]  fd         file descriptor to recieve the message
 * @param[in]  data       struct with message info
 * @param[in]  data_size  message size in bytes
 */
void update_region( int fd, Smessage data, int data_size){
	if ( regions[data.region].message != NULL)
		free(regions[data.region].message);

	regions[data.region].size = data.message_size;
	regions[data.region].message = (void *) malloc (data.message_size);
	if ( regions[data.region].message == NULL){
		printf ("malloc failure\n");
		exit (-1);
	}

	//read the message and copy it
	if ( read(fd, regions[data.region].message, data.message_size) < 0){
		perror("read: ");
		exit(-1);
	}
	
	//TEMPORARY PRINT FOR TESTING - TO BE DELETED
	printf("copied %s to region %d\n", (char *) regions[data.region].message, data.region);
}

/**
 * @brief      Sends a cilpboard region message
 *
 * @param[in]  fd         file descriptor to send the message
 * @param[in]  data       struct with the message info
 * @param[in]  data_size  message size in bytes
 */
void send_region(int fd, Smessage data, int data_size){
	//check if there's anything to paste
	if (regions[data.region].message == NULL){
		printf("nothing to paste in region %d \n", data.region);
		data.region = -1;
	}
	else
		data.message_size = regions[data.region].size;
	
	//send the message info
	if ( write(fd, &data, data_size) < 0){
		perror("write: ");
		exit(-1);
	}

	if (data.region == -1)	
		return;
	
	//send the message requested
	if ( write(fd, regions[data.region].message, data.message_size) < 0){
		perror("write: ");
		exit(-1);
	}
	//TEMPORARY PRINT FOR TESTING - TO BE DELETED
	printf("pasted %s to region %d\n", (char *) regions[data.region].message, data.region);
}
