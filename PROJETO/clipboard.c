#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>

#include "clipboard.h"
#include "threads.h"
#include "regions.h"

int main(int argc, char **argv){		
	int server_fd;
	client_socket *CS_un;
	client_socket *CS_in;

	//initialize the lock variables
	init_locks(REGIONS_NR);

//INITIALIZE LOCAL CLIPBOARD
	if (argc == 4 && strcmp(argv[1], "-c") == 0){
			server_fd = connected_clipboard_init(argv[2], argv[3]);
			regions_init(server_fd);
	}
	else if (argc == 1){
		server_fd = redundant_server();
		regions_init(-1);
	}
	else
		invalid_args_exit();

//INITIALIZE THE SERVERS
	//initialize the server to handle local connections (apps)
	pthread_t thread_id_un;
	if (pthread_create(&thread_id_un, NULL, server_init, (void *) UNIX) != 0){
		perror("pthread_create: ");
		exit(-1);
	}
	//initialize the servers to handle remote connections (clipboards)
	pthread_t thread_id_in;
	if (pthread_create(&thread_id_in, NULL, server_init, (void *) INET) != 0){
		perror("pthread_create: ");
		exit(-1);
	}

//LAUNCH THE CICLES TO PROCESS THE CONNECTIONS
	//launch the cicle to handle local connections (apps)
	pthread_join(thread_id_un, (void **) &CS_un);
	if (pthread_create(&thread_id_un, NULL, accept_clients, CS_un) != 0){
		perror("pthread_create: ");
		exit(-1);
	}
	//launch the cicle to handle remote connections (clipboards)
	pthread_join(thread_id_in, (void **) &CS_in);
	if (pthread_create(&thread_id_in, NULL, accept_clients, CS_in) != 0){
		perror("pthread_create: ");
		exit(-1);
	}

	//recieve updates from the clipboard "server"
	connection_handle(server_fd, UP);
	server_fd = redundant_server();
	connection_handle(server_fd, UP);
	
 exit (-3);
}