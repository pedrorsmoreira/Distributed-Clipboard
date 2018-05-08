#include "clipboard.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/un.h>
#include <sys/socket.h>

int clipboard_connect(char * clipboard_dir){
	struct sockaddr_un server_addr;
	struct sockaddr_un client_addr;	

	int sock_fd= socket(AF_UNIX, SOCK_STREAM, 0);	
	if (sock_fd == -1){
		perror("socket: ");
		exit(-1);
	}

	client_addr.sun_family = AF_UNIX;
	sprintf(client_addr.sun_path, "%ssocket_%d", clipboard_dir, getpid());
	server_addr.sun_family = AF_UNIX;
	strcpy(server_addr.sun_path, SOCK_ADDRESS);

	if(connect(sock_fd, (const struct sockaddr *) &server_addr, sizeof(server_addr)) < 0)	return -1;

	return sock_fd;
}

int clipboard_copy(int clipboard_id, int region, void *buf, size_t count){
	int data_size = DATA_SIZE;
	Smessage data;
	data.region = region;
	data.order = COPY;
	data.message_size = count;

	// check for valid region
	if ((region < 0) || (region > REGIONS_NR))	return 0;
	
	
	if ( write(clipboard_id, &data, DATA_SIZE) < 0){
		perror("write: ");
		return 0;
	}

	int err_w;
	
		if ( ( err_w = write(clipboard_id, buf, data.message_size) ) == -1){
			perror("write: ");
			return 0;
		}
return data.message_size;
}


int clipboard_paste(int clipboard_id, int region, void *buf, size_t count){
	int data_size = DATA_SIZE;
	Smessage data;
	data.region = region;
	data.order = PASTE;

	// check for valid region
	if ((region < 0) || (region > REGIONS_NR))	return 0;

	if (write(clipboard_id, &data, data_size) < 0){
		perror("write: ");
		return 0;
	}
	if (read(clipboard_id, &data, data_size) < 0){
		perror("read: ");
		return 0;
	}

	//if the region is empty
	if (data.region == -1)	return 0;

	char *aux = (char *) malloc (data.message_size);
	if (aux == NULL){
		printf ("malloc failure\n");
		exit (-1);
	}

	int err_read;
	
	if ( ( err_read = read(clipboard_id, aux, data.message_size) ) == -1){
		perror("read: ");
		exit(-1);
	}

	if (memcpy(buf, aux, data_size) == NULL){
		perror("memcpy: ");
		exit(-1);
	}
	free(aux);
return data.message_size;
}

int clipboard_wait(int clipboard_id, int region, void *buf, size_t count){
return 0;
}

void clipboard_close(int clipboard_id){
	close(clipboard_id);
}