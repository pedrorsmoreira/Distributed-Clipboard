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

	pthread_t thread_id_un;
	if (pthread_create(&thread_id_un, NULL, server_init, (void *) UNIX) != 0){
		perror("pthread_create: ");
		exit(-1);
	}

	pthread_t thread_id_in;
	if (pthread_create(&thread_id_in, NULL, server_init, (void *) INET) != 0){
		perror("pthread_create: ");
		exit(-1);
	}
//////////////////////////UNIX
	int *sock_fd_un = NULL;
	pthread_join(thread_id_un, (void **) &sock_fd_un);

	//struct with the client adress info to send to the thread
	client_socket CS;
	CS.sock_fd = *sock_fd_un;
	free(sock_fd_un);
	CS.family = UNIX;

	//handle apps
	if (pthread_create(&thread_id_un, NULL, accept_clients, &CS) != 0){
		perror("pthread_create: ");
		exit(-1);
	}
/////////////////////////////INET
	int *sock_fd_in = NULL;
	pthread_join(thread_id_in, (void **) &sock_fd_in);

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