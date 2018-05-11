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

 
int main(int argc, char **argv){		
	//initialize the regions
	regions_init();

	pthread_t thread_id;
	if (pthread_create(&thread_id, NULL, app_init, NULL) != 0){
		perror("pthread_create: ");
		exit(-1);
	}
	int *sock_fd;
	pthread_join(thread_id, (void*)sock_fd);

	//struct with the client adress info to send to the thread
	client_socket CS;
	CS.sock_fd = *sock_fd;
	CS.size = sizeof(struct sockaddr);
	free(sock_fd);

	//handle apps
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