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
#include "threads.h"

 
int main(){
	struct sockaddr_un local_addr;
	
	//assure there was no previous socket with the same name
	unlink(SOCK_ADDRESS);

	//create a socket stream for app_server_addr comunitacion
	int sock_fd = socket(AF_UNIX, SOCK_STREAM, 0);
	if (sock_fd == -1){
		perror("socket: ");
		exit (-1);
	}

	//set the local communication parameters
	local_addr.sun_family = AF_UNIX;
	strcpy(local_addr.sun_path, SOCK_ADDRESS);

	//adress the socket (own it)
	if ( bind(sock_fd, (struct sockaddr *) &local_addr, sizeof(local_addr)) < 0){
		perror("bind: ");
		exit (-1);
	}

	//get ready to act as a server
	if (listen (sock_fd, 2) == -1){
		perror("listen: ");
		exit (-1);
	}
	
	//initialize the regions
	regions_init();

	//struct with the client adress info to send to the thread
	client_socket CS;
	CS.sock_fd = sock_fd;
	CS.size = sizeof(struct sockaddr);

	//handle apps
	pthread_t thread_id;
	if (pthread_create(&thread_id, NULL, app_thread, &CS) != 0){
		perror("pthread_create: ");
		exit(-1);
	}

	//temporary, just to keep main alive
	//think about it do it nice
	while(1);


	//final clean
	/*for (int i = 0; i <REGIONS_NR; i++) free(regions[i]);
	unlink(file_name_out);
	close(fifo_out);
	unlink(file_name_in);
	close(fifo_in);
	exit(0);*/	
}