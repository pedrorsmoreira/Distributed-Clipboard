#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "clipboard.h"

 
int main(){
//create FIFOs
	char file_name_out[100];
	char file_name_in[100];
	// FIFO_OUT
	sprintf(file_name_out, "./%s", OUTBOUND_FIFO);
	unlink(file_name_out);
	if(mkfifo(file_name_out, 0666)==-1){
		printf("Error creating out fifo\n");
		exit(-1);
	}
	int fifo_out = open(file_name_out, O_RDWR);
	if(fifo_out == -1){
		printf("Error opening in fifo\n");
		exit(-1);
	}
	// FIFO_IN
	sprintf(file_name_in, "./%s", INBOUND_FIFO);
	unlink(file_name_in);
	if(mkfifo(file_name_in, 0666)==-1){
		printf("Error creating in fifo\n");
		exit(-1);
	}
	int fifo_in = open(file_name_in, O_RDWR);
	if(fifo_in == -1){
		printf("Error opening in fifo\n");
		exit(-1);
	}
//declare and initialize variables for the actions cicle
	Smessage data;
	char *regions[REGIONS_NR];
	for (int i = 0; i <REGIONS_NR; i++) regions[i] = NULL;
	//bytestream holds the struct info to be sent through the fifo
	int data_size = DATA_SIZE;
	char *bytestream = (char *) malloc(size);
	if ( bytestream == NULL){
		printf ("malloc failure\n");
		exit (-1);
	}
//actions cicle
	while(1){
		printf(".\n");
		//read data
		if ( read(fifo_in, bytestream, data_size) < 0){
			printf("read failure\n");
			exit(-1);
		}
		memcpy((void *) &data, (void *) bytestream, data_size);										//verificar isto ?!?!?!?
		
		if ( (data.region < 0) || (data.region > REGIONS_NR))	exit(-2);
		if (data.order == COPY){
			// if something is already copied in this region, replace it
			if ( regions[data.region] != NULL) free(regions[data.region]);
			// copy the message
			regions[data.region] = (char *) malloc (data_size);
			strcpy(regions[data.region], data.message);
			//print
			printf("copied %s to region %d\n", data.message, data.region);	
		}else if (data.order == PASTE){
			//check if there anything to paste
			if (regions[data.region] == NULL){
				printf("nothing to paste in region %d \n", data.region);
				data.region = -1;
			}
			else	strcpy (data.message, regions[data.region]);
			//enviar de volta a estrutura com a mensagem
			memcpy((void *) bytestream,(void *) &data, data_size);									//verificar isto ?!?!?!?
			write(fifo_out, bytestream, data_size);
		}
		else exit(-2);	
	}
//final clean
	for (int i = 0; i <REGIONS_NR; i++) free(regions[i]);
	free(bytestream);
	unlink(file_name_out);
	close(fifo_out);
	unlink(file_name_in);
	close(fifo_in);
	exit(0);	
}