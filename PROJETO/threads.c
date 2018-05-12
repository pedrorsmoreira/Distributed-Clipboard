#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <pthread.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <arpa/inet.h>

#include "clipboard.h"
#include "threads.h"
#include "regions.h"

int rand_port_gen(){
	srand(time(NULL));
	int ret = 1024 + rand()%63715;
	return ret;
}

void *server_init(void * family){
	//assure there was no previous socket with the same name
	unlink(SOCK_ADDRESS);

	struct sockaddr *local_addr;
	socklen_t addrlen;

	int *sock_fd = (int *) malloc(sizeof(int));

	//set the communication type parameters
	if (family == (void *) UNIX){
		struct sockaddr_un local_addr_un;
		addrlen = sizeof(local_addr_un);
		local_addr_un.sun_family = AF_UNIX;
		strcpy(local_addr_un.sun_path, SOCK_ADDRESS);
		local_addr = (struct sockaddr *) &local_addr_un;
		*sock_fd = socket(AF_UNIX, SOCK_STREAM, 0);
	}
	else if (family == (void *) INET){
		struct sockaddr_in local_addr_in;
		addrlen = sizeof(local_addr_in);
		local_addr_in.sin_family = AF_INET;
		int port = rand_port_gen();
		printf("port number: %d\n", port);
		local_addr_in.sin_port = htons(port);
		local_addr_in.sin_addr.s_addr = INADDR_ANY;
		local_addr = (struct sockaddr *) &local_addr_in;
		*sock_fd = socket(AF_INET, SOCK_STREAM, 0);
	}
	
	if (*sock_fd < 0){
		perror("socket: ");
		exit (-1);
	}

	//adress the socket (own it)
	if ( bind(*sock_fd, local_addr, addrlen) < 0){
		perror("bind: ");
		exit (-1);
	}

	//get ready to act as a server
	if (listen (*sock_fd, 2) == -1){
		perror("listen: ");
		exit (-1);
	}
 return (void *) sock_fd;
}

void *accept_clients(void * CS_){
	client_socket *CS = (client_socket *) CS_;

	struct sockaddr *client_addr;
	
	if (CS->family == UNIX){
		struct sockaddr_un client_addr_un;
		client_addr = (struct sockaddr *) &client_addr_un;
	}
	else if (CS->family == INET){
		struct sockaddr_in client_addr_in;
		client_addr = (struct sockaddr *) &client_addr_in;
	}
	
	socklen_t size = sizeof(struct sockaddr);
	
	//stablish connection with the client
	int client_fd = accept( CS->sock_fd, client_addr, &size);
	if (client_fd == -1){
		perror("accept: ");
		exit (-1);
	}
	
	//create new thread for next client connection
	pthread_t thread_id;
	if (pthread_create(&thread_id, NULL, accept_clients, CS) != 0){
		perror("pthread_create: ");
		exit(-1);
	}

	if (CS->family == UNIX)
		app_handle(client_fd);
	//else if (CS->family == INET)
		//ISN'T IT THE SAME AS APP_HANDLE (data.order==COPY) CODE??

return NULL;
}

void app_handle(int client_fd){
	Smessage data;
	int data_size = sizeof(Smessage);

	while ( read(client_fd, &data, data_size) > 0){
		if ( (data.region < 0) || (data.region > REGIONS_NR))	
			exit(-2);
		
		if (data.order == COPY)	
			update_region(client_fd, data, data_size);
		else if (data.order == PASTE)
			send_region(client_fd, data, data_size);
		else exit(-2);
	}
}