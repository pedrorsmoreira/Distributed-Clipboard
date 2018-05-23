#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/un.h>
#include <arpa/inet.h>

#include "clipboard.h"
#include "threads.h"
#include "regions.h"

down_list *head = NULL;
extern pthread_mutex_t mutex_init;

/**
 * @brief      Initializes a socket-stream server
 *
 * @param[in]  family	Stream type (UNIX or INET)
 *
 * @return     struct with info to accpet the clients
 */
void *server_init(void * family){
	struct sockaddr *local_addr;
	socklen_t addrlen;

	client_socket *CS = (client_socket *) malloc(sizeof(client_socket));
	CS->family = family;

	//set the communication type parameters 
	if (family == (void *) UNIX){
		unlink(SOCK_ADDRESS);
		struct sockaddr_un local_addr_un;
		addrlen = sizeof(local_addr_un);
		local_addr_un.sun_family = AF_UNIX;
		strcpy(local_addr_un.sun_path, SOCK_ADDRESS);
		local_addr = (struct sockaddr *) &local_addr_un;
		CS->sock_fd = socket(AF_UNIX, SOCK_STREAM, 0);
	}
	else if (family == (void *) INET){
		struct sockaddr_in local_addr_in;
		addrlen = sizeof(local_addr_in);
		local_addr_in.sin_family = AF_INET;
		int port = rand_port_gen();
		local_addr_in.sin_port = htons(port);
		local_addr_in.sin_addr.s_addr = INADDR_ANY;
		local_addr = (struct sockaddr *) &local_addr_in;
		CS->sock_fd = socket(AF_INET, SOCK_STREAM, 0);
		printf("port number: %d\n", port);
	}

	//check endpoint creation failure
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
 * @param[in]  CS_   pointer to a struct with info 
 * 					 to accpet the clients
 * 
 * @return     useless
 */
void *accept_clients(void * CS_){
	client_socket *CS = (client_socket *) CS_;
	int client_fd;

	//set the connection parameters
	struct sockaddr *client_addr;
	socklen_t size = sizeof(struct sockaddr);
	if (CS->family == UNIX){
		struct sockaddr_un client_addr_un;
		client_addr = (struct sockaddr *) &client_addr_un;
	}
	else if (CS->family == INET){
		struct sockaddr_in client_addr_in;
		client_addr = (struct sockaddr *) &client_addr_in;
	}
	
	//stablish connections with the client
	client_fd = accept( CS->sock_fd, client_addr, &size);
	if (client_fd == -1){
		perror("accept: ");
		exit (-1);
	}

	//if clipboard "client" add it the list
	if (CS->family == INET){
		//lock the run of the list of clipboard "clients" - por no relatorio q se assim nao fosse, se hoivesse uma modificacao a meio da atualizacao esse novo clipboard ja nao seria atualizado
		if (pthread_mutex_lock(&mutex_init) != 0){
			printf("mutex writeUP lock failure\n");
			exit(-1);
		}
		regions_init_client(client_fd);//
		head = add_down_list(head, client_fd);
		//unlock
		if (pthread_mutex_unlock(&mutex_init) != 0){
			printf("mutex writeUP lock failure\n");
			exit(-1);
		}
	}

	//create new thread for next client connection
	pthread_t thread_id;
	if (pthread_create(&thread_id, NULL, accept_clients, CS) != 0){
		perror("pthread_create: ");
		exit(-1);
	}

	//handle the client requests
	connection_handle(client_fd, DOWN);

	//TEMPORARY PRINT FOR TESTING
	printf("acabou connection type (inet/unix) %d\n", CS->family);
 return NULL;
}

/**
 * @brief      handles client requests until it closes the connection
 *
 * @param[in]  fd         connection file descriptor
 * @param[in]  reference  server (UP) or client(DOWN)
 */
void connection_handle(int fd, int reference){
	Smessage data;
	int data_size = sizeof(Smessage);

	//listens until the connection is closed
	while ( read(fd, &data, data_size) > 0){
		//check for valid region JJJUUSSSTT FFOOORRR DDEEEBBBUUUGGG
		if ( (data.region < 0) || (data.region > REGIONS_NR)){
			printf("received wrong region in connection_handle\n");
			exit(-2);
		}
		//process the order
		if (data.order == COPY){
			if (reference == UP)
				update_region(&head, fd, data, data_size);
			else if (reference == DOWN)
				send_up_region(fd, data, data_size);
		}
		else
			send_region(fd, data, data_size, data.order);
	}printf("acabou connection type (up/down) %d\n", reference);
}