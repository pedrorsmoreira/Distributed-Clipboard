#include "clipboard.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>


int clipboard_connect(char * clipboard_dir){
	char fifo_name[100];
	
	sprintf(fifo_name, "%s%s", clipboard_dir, INBOUND_FIFO);
	int fifo_send = open(fifo_name, O_WRONLY);
	sprintf(fifo_name, "%s%s", clipboard_dir, OUTBOUND_FIFO);
	int fifo_recv = open(fifo_name, O_RDONLY);
	if(fifo_recv < 0)
		return fifo_recv;
	else
		return fifo_send;
}

void clipboard_copy(int clipboard_id, int region, void *buf, size_t count){
	int data_size = DATA_SIZE;
	Smessage data;
	data.region = region;
	data.order = COPY;
	char *bytestream = (char *) malloc(size);
	if ( bytestream == NULL){
		printf ("malloc failure\n");
		exit (-1);
	}
	strcpy (data.message, (char *) buf);		//nao podia ser com = ??
	memcpy((void *) bytestream, (void *) &data, data_size);												// verificar isto??!!??!?!?!?!?!?
	if ( write(clipboard_id, bytestream, DATA_SIZE) < 0){
		printf("write failure\n");
		exit(-1);
	}
}
void clipboard_paste(int clipboard_id, int region, void *buf, size_t count){
	int data_size = DATA_SIZE;
	Smessage data;
	data.region = region;
	data.order = PASTE;
	char *bytestream = (char *) malloc(size);
	if ( bytestream == NULL){
		printf ("malloc failure\n");
		exit (-1);
	}
	memcpy((void *) bytestream, (void *) &data, data_size);
	if (write(clipboard_id, bytestream, data_size) < 0){
		printf("write failure\n");
		exit(-1);
	}
	if (read(clipboard_id + 1, bytestream, data_size) < 0){
		printf("read failure\n");
		exit(-1);
	}
	memcpy((void *) &data, (void *) bytestream, data_size);
	if (data.region == -1)	exit(-1);
	strcpy((char *) buf, data.message);
}
