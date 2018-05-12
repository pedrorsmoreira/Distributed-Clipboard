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

REG regions[REGIONS_NR];

//COMMENT THIS CODE WHEN BOTH CASES ARE WRITTEN
void regions_init(){
	// if (working alone)
		for (int i = 0; i <REGIONS_NR; i++)
			regions[i].message = NULL;
	// else .....
}

void *server_init(void * family){
	//assure there was no previous socket with the same name
	unlink(SOCK_ADDRESS);

	struct sockaddr *local_addr;

	int *sock_fd = (int *) malloc(sizeof(int));

	//set the communication type parameters
	if (family == UNIX){
		struct sockaddr_un local_addr_un;
		local_addr_un.sun_family = AF_UNIX;
		local_addr = (struct sockaddr *) &local_addr_un;
		*sock_fd = socket(AF_UNIX, SOCK_STREAM, 0);
	}
	else if (family == INET){
		struct sockaddr_in local_addr_in;
		local_addr_in.sin_family = AF_INET;
		local_addr_in.sin_port = htons(3010);
		local_addr_in.sin_addr.s_addr = INADDR_ANY;
		local_addr = (struct sockaddr *) &local_addr_in;
		*sock_fd = socket(AF_INET, SOCK_STREAM, 0);
	}
	
	if (*sock_fd < 0){
		perror("socket: ");
		exit (-1);
	}

	//adress the socket (own it)
	if ( bind(*sock_fd, local_addr, sizeof(local_addr)) < 0){
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
		
		if (data.order == COPY){	
			// if something is already copied in this region, replace it
			if ( regions[data.region].message != NULL)
				free(regions[data.region].message);

			regions[data.region].size = data.message_size;
			regions[data.region].message = (void *) malloc (data.message_size);
			if ( regions[data.region].message == NULL){
				printf ("malloc failure\n");
				exit (-1);
			}

			//read the message and copy it
			if ( read(client_fd, regions[data.region].message, data.message_size) < 0){
				perror("read: ");
				exit(-1);
			}
			
			//TEMPORARY PRINT FOR TESTING - TO BE DELETED
			printf("copied %s to region %d\n", (char *) regions[data.region].message, data.region);	
		}else if (data.order == PASTE){
			//check if there's anything to paste
			if (regions[data.region].message == NULL){
				printf("nothing to paste in region %d \n", data.region);
				data.region = -1;
			}
			else
				data.message_size = regions[data.region].size;
			
			//send back the message info
			if ( write(client_fd, &data, data_size) < 0){
				perror("write: ");
				exit(-1);
			}

			if (data.region == -1)	
				continue;
			
			//send the message requested
			if ( write(client_fd, regions[data.region].message, data.message_size) < 0){
				perror("write: ");
				exit(-1);
			}
	
		}
		else exit(-2);
	}
}
/*
void *distributed_clipboard_client(){
	
	struct sockaddr_in server_addr;
	int sock_fd= socket(AF_INET, SOCK_STREAM, 0);
	if (sock_fd == -1){
		perror("socket: ");
		exit(-1);
	}

	server_addr.sin_family = AF_INET;
	server_addr.sin_port= htons(3000);
	inet_aton("193.136.128.104", &server_addr.sin_addr);
	 
	
	if( -1 == connect(sock_fd, (const struct sockaddr *) &server_addr, sizeof(server_addr))){
		printf("Error connecting\n");
		exit(-1);
	}



}*/