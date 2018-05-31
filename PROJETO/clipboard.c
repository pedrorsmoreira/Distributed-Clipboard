#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>

#include "clipboard.h"
#include "threads.h"
#include "regions.h"

extern pthread_mutex_t mutex_writeUP;

//Ctrl^C signal handler
void terminate_ok(int a){
	unlink(SOCK_ADDRESS);
	exit(0);
}

int main(int argc, char **argv){		
	int server_fd;
	client_socket *CS_un;
	client_socket *CS_in;

	if (signal(SIGINT, terminate_ok) == SIG_ERR || signal(SIGPIPE, SIG_IGN) == SIG_ERR)
		system_error();

	//initialize the lock variables
	init_locks();

//INITIALIZE LOCAL CLIPBOARD
	if (argc == 4 && strcmp(argv[1], "-c") == 0){
			server_fd = connected_clipboard_init(argv[2], argv[3]);
			if (server_fd > 0)
				regions_init_local(server_fd);
			else {
				server_fd = redundant_server();
				regions_init_local(-1);
			}

	}
	else if (argc == 1){
		server_fd = redundant_server();
		regions_init_local(-1);
	}
	else{
		printf("invalid arguments\n");
		exit(-2);
	}

//INITIALIZE THE SERVERS
	//initialize the server to handle local connections (apps)
	pthread_t thread_id_un;
	if (pthread_create(&thread_id_un, NULL, server_init, (void *) UNIX) != 0)
		system_error();
	
	//initialize the servers to handle remote connections (clipboards)
	pthread_t thread_id_in;
	if (pthread_create(&thread_id_in, NULL, server_init, (void *) INET) != 0)
		system_error();
	

//LAUNCH THE CYCLES TO PROCESS THE CONNECTIONS
	//launch the cycle to handle local connections (apps)
	if (pthread_join(thread_id_un, (void **) &CS_un) != 0)
		system_error();
	if (pthread_create(&thread_id_un, NULL, accept_clients, CS_un) != 0)
		system_error();
	
	//launch the cycle to handle remote connections (clipboards)
	if (pthread_join(thread_id_in, (void **) &CS_in) != 0 )
		system_error();
	if (pthread_create(&thread_id_in, NULL, accept_clients, CS_in) != 0)
		system_error();
	

	//receive updates from the clipboard "server"
	connection_handle(server_fd, UP);

	//lock the writes to clipboard "server"
	if (pthread_mutex_lock(&mutex_writeUP) != 0)
		system_error();

		//if connection with clipboard server ended, work in single mode
		close (server_fd);
		server_fd = redundant_server();

	if (pthread_mutex_unlock(&mutex_writeUP) != 0)
		system_error();

	connection_handle(server_fd, UP);
	
 exit (-1);
}