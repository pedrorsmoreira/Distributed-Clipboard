/******************************************************************************
 *
 * File Name: library.c
 *	      (c) 2018 PSis
 * Authors: Pedro Moreira e Inês Gaspar
 *
 * DESCRIPTION
 *		API functions (connect, copy, paste, wait and close)
 *****************************************************************************/


#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <signal.h>

#include "clipboard.h"

/**
 * @brief      creates the connection with the local clipboard
 *
 * @param[in]  clipboard_dir  directory of the local clipboard
 *
 * @return     returns the clipboard identifier to interact
 *             with the clipboard, or -1 in case of error
 */
int clipboard_connect(char * clipboard_dir){
	//to prevent exits due to SIGPIPE came from writes
	if (signal(SIGPIPE, SIG_IGN) == SIG_ERR)
		return -1;

	//set the local communication parameters
	struct sockaddr_un server_addr;
	server_addr.sun_family = AF_UNIX;
	strcpy(server_addr.sun_path, SOCK_ADDRESS);

	int sock_fd= socket(AF_UNIX, SOCK_STREAM, 0);	
	if (sock_fd == -1)
		return -1; 

	//connect the end-points to create the stream
	if(connect(sock_fd, (const struct sockaddr *) &server_addr, sizeof(server_addr)) == 0)
		return sock_fd;
	else
		return -1;
}

/**
 * @brief      stores data in a certain region of the clipboard
 *
 * @param[in]  clipboard_id  Clipboard identifier - file descriptor
 * @param[in]  region        Clipboard region to store the data
 * @param[in]  buf	         Buffer which the data is pointed by
 * @param[in]  count         Number of bytes of data to be copied
 *
 * @return     returns the number of bytes copied
 *             or returns 0 in case of error
 */
int clipboard_copy(int clipboard_id, int region, void *buf, size_t count){
	int data_size = sizeof(Smessage);
	Smessage data;
	data.region = region;
	data.order = COPY;
	data.message_size = count;
	
	// check for valid input
	if ((region < 0) || (region >= REGIONS_NR) || count == 0 || buf == NULL)	
		return 0;

	//send the message specs
	if (write(clipboard_id, &data, data_size) != data_size)
		return 0;

	//send the message
	if (write(clipboard_id, buf, data.message_size) != data.message_size)
	{
		printf("falhehi copy\n");
		return 0;
	}

 return data.message_size;
}

/**
 * @brief      Gets data from a certain region of the clipboard
 *
 * @param[in]  clipboard_id  clipboard identifier - file descriptor
 * @param[in]  region        Clipboard region to copy the data
 * @param[out] buf           Buffer to store the message
 * @param[in]  count         Max number of bytes to be stored in the buffer
 *
 * @return     returns the number of bytes stored in the buffer
 *             or returns 0 in case of error
 */
int clipboard_paste(int clipboard_id, int region, void *buf, size_t count){
	int data_size = sizeof(Smessage);
	Smessage data;
	data.region = region;
	data.order = PASTE;
	data.message_size = count;

	// check for valid input
	if ((region < 0) || (region >= REGIONS_NR) || count == 0 || buf == NULL)	
		return 0;

	//send the region requested
	if (write(clipboard_id, &data, data_size) !=  data_size)
		return 0;

	//read the message specs
	if (read(clipboard_id, &data, data_size) != data_size)
		return 0;

	//if the region is empty or message too big
	if (data.region == -1)	
		return 0;

	//read the message
	if (read(clipboard_id, buf, data.message_size) != data.message_size)
		return 0;

 return data.message_size;
}

/**
 * @brief      Waits on a certain clipboard region to change
 *             and sends the new data
 *
 * @param[in]  clipboard_id  clipboard identifier - file descriptor
 * @param[in]  region        Clipboard region to copy the data
 * @param[in]  buf           Buffer to store the message
 * @param[in]  count         Max number of bytes to be stored in the buffer
 *
 * @return     returns the number of bytes stored in the buffer
 *             or returns 0 in case of error
 */
int clipboard_wait(int clipboard_id, int region, void *buf, size_t count){
	int data_size = sizeof(Smessage);
	Smessage data;
	data.region = region;
	data.order = WAIT;

	if(buf == NULL || count == 0)
		return 0;

	// check for valid input
	if ((region < 0) || (region >= REGIONS_NR) || count == 0 || buf == NULL)	
		return 0;

	//send the region requested
	if (write(clipboard_id, &data, data_size) !=  data_size)
		return 0;

	//read the message specs
	if (read(clipboard_id, &data, data_size) != data_size)
		return 0;

	//if message too big
	if (data.region == -1)	
		return 0;

	//read the message
	if (read(clipboard_id, buf, data.message_size) != data.message_size)
		return 0;
	
 return data.message_size;
}

/**
 * @brief      closes the connection with the local clipboard
 *
 * @param[in]  clipboard_id  clipboard identifier - file descriptor
 */
void clipboard_close(int clipboard_id){
	close(clipboard_id);
}