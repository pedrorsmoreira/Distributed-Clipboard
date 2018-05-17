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
//#include "regions.h"
#include "utils.h"

down_list *head = NULL;

/**
 * @brief      Initializes a socket stream server
 *
 * @param[in]  family	Stream type (UNIX or INET)
 *
 * @return     socket file descriptor 
 */
void *server_init(void * family){
	//assures there was no previous socket with the same name
	unlink(SOCK_ADDRESS);

	struct sockaddr *local_addr;
	socklen_t addrlen;

	client_socket *CS = (client_socket *) malloc(sizeof(client_socket));
	CS->family = family;

	//set the communication type parameters 
	if (family == (void *) UNIX){
		struct sockaddr_un local_addr_un;
		addrlen = sizeof(local_addr_un);
		local_addr_un.sun_family = AF_UNIX;
		strcpy(local_addr_un.sun_path, SOCK_ADDRESS);
		local_addr = (struct sockaddr *) &local_addr_un;
		CS->sock_fd = socket(AF_UNIX, SOCK_STREAM, 0);
	}
	else{
		struct sockaddr_in local_addr_in;
		addrlen = sizeof(local_addr_in);
		local_addr_in.sin_family = AF_INET;
		int port = rand_port_gen();
		local_addr_in.sin_port = htons(port);
		local_addr_in.sin_addr.s_addr = INADDR_ANY;
		local_addr = (struct sockaddr *) &local_addr_in;
		CS->sock_fd = socket(AF_INET, SOCK_STREAM, 0);
		CS->port = port;
		if (family == (void *) INET_RECV)
			printf("port number: %d\n", port);
		/*if (family == (void *) INET_SEND)
			printf("port number2: %d\n", port);*/
	}

	//check endpoint creation faliure
	if (CS->sock_fd < 0){
		perror("socket: ");
		exit (-1);
	}

	//address the socket (own it)
	if ( bind(CS->sock_fd, local_addr, addrlen) < 0){
		perror("bind: ");
		exit (-1);
	}

	//get ready to act as a server
	if (listen (CS->sock_fd, 10) == -1){
		perror("listen: ");
		exit (-1);
	}

 return (void *) CS;
}

/**
 * @brief      Recursive function - accepts a socket stream 
 *             connection every function call and launches 
 *             the function to process the new connection
 *
 * @param[in]  CS_   pointer to stream connection parameters
 * 
 * @return     useless
 */
void *accept_clients(void * CS_){
	int client_fd;

	client_socket *CS = (client_socket *) CS_;

	struct sockaddr *client_addr;
	
	if (CS->family == UNIX){
		struct sockaddr_un client_addr_un;
		client_addr = (struct sockaddr *) &client_addr_un;
	}
	else{
		struct sockaddr_in client_addr_in;
		client_addr = (struct sockaddr *) &client_addr_in;
	}
	
	socklen_t size = sizeof(struct sockaddr);
	
	//stablish connections with the client
	client_fd = accept( CS->sock_fd, client_addr, &size);
	if (client_fd == -1){
		perror("accept: ");
		exit (-1);
	}

	if (CS->family == INET_RECV){
		if ( write(client_fd, &(CS->port), sizeof(int)) < 0){
		perror("write: ");
		exit(-1);
		}
	}
	else if (CS->family == INET_SEND)
		head = add_down_list(head, client_fd);

	//create new thread for next client connection
	pthread_t thread_id;
	if (pthread_create(&thread_id, NULL, accept_clients, CS) != 0){
		perror("pthread_create: ");
		exit(-1);
	}

	if (CS->family == UNIX || CS->family == INET_RECV)
		connection_handle(client_fd, DOWN);
	
 return NULL;
}

/**
 * @brief      handles client requests (copy or paste)
 *             until it closes the connection	
 *
 * @param[in]  client_fd_recv  client file descriptor
 */
void connection_handle(int fd, int reference){
	Smessage data;
	int data_size = sizeof(Smessage);

	//listens until the connection is closed
	while ( read(fd, &data, data_size) > 0){
		//check for valid region
		if ( (data.region < 0) || (data.region > REGIONS_NR)){
			printf("received wrong region in connection_handle\n");
			exit(-2);
		}
		//copy or paste
		if (data.order == COPY){
			if (reference == UP)
				update_region(&head, fd, data, data_size);
			else if (reference == DOWN)
				send_up_region(fd, data, data_size);
		}
		else if (data.order == PASTE)
			send_region(fd, data, data_size);
	}
}