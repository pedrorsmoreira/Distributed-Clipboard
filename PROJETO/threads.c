#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/un.h>
#include <arpa/inet.h>
#include <fcntl.h>

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
	int port;
	struct sockaddr *local_addr = NULL;
	socklen_t addrlen = 0;

	client_socket *CS = (client_socket *) malloc(sizeof(client_socket));
	CS->family = family;

	//set the communication type parameters 
	if (family == (void *) UNIX){
		port = MAX_PORT + 1;
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
		port = rand_port_gen();
		local_addr_in.sin_port = htons(port);
		local_addr_in.sin_addr.s_addr = INADDR_ANY;
		local_addr = (struct sockaddr *) &local_addr_in;
		CS->sock_fd = socket(AF_INET, SOCK_STREAM, 0);
	}

	//check endpoint creation failure
	if (CS->sock_fd < 0)
		system_error("socket() in server_init");

	while (bind(CS->sock_fd, local_addr, addrlen) < 0){
		if (port <= MAX_PORT){
			((struct sockaddr_in*) local_addr) -> sin_port = htons(++ port);
			continue;
		}
		else
			system_error("bind in server_init");
	}

	//get ready to act as a server
	if (listen (CS->sock_fd, 10) == -1)
		system_error("listen ins server_init");

	if (family == (void *) INET)
		printf("port number: %d\n", port);

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
	struct sockaddr *client_addr = NULL;
	socklen_t size = sizeof(struct sockaddr);
	if (CS->family == UNIX){
		struct sockaddr_un client_addr_un;
		client_addr = (struct sockaddr *) &client_addr_un;
	}
	else if (CS->family == INET){
		struct sockaddr_in client_addr_in;
		client_addr = (struct sockaddr *) &client_addr_in;
	}
	
	//stablish connections with the client (wait for the client to connect)
	if ( (client_fd = accept( CS->sock_fd, client_addr, &size)) == -1)
		system_error("accept() in accept_clients");

	//if it is a clipboard -> add it to the list
	if (CS->family == INET){
		//lock the run of the list of clipboard "clients"
		if (pthread_mutex_lock(&mutex_init) != 0)
			system_error("mutex_init lock in accept_clients");

			regions_init_client(client_fd);
			head = add_down_list(head, client_fd);
		
		if (pthread_mutex_unlock(&mutex_init) != 0)
			system_error("mutex_init unlock in accept_clients");
	}

	//create new thread for next client connection
	pthread_t thread_id;
	if (pthread_create(&thread_id, NULL, accept_clients, CS) != 0)
		system_error("pthread_create in accept_clients");

	//handle the client requests
	connection_handle(client_fd, DOWN);

	//close the socket of the finnished connection
	if (fcntl(client_fd, F_GETFD) != -1)
		close(client_fd);

	//TEMPORARY PRINT FOR TESTING
	//printf("acabou connection type (inet/unix) %d\n", CS->family);
	pthread_t self = pthread_self();
if (pthread_detach(self) != 0)
		system_error("pthread_detach in accept_clients");


 return NULL;
}

/**
 * @brief      handles client requests until it closes the connection
 *
 * @param[in]  fd         connection file descriptor
 * @param[in]  reference  server (UP) or client(DOWN)
 */
void connection_handle(int fd, int reference){int temporary;
	Smessage data;
	int data_size = sizeof(Smessage);

	//listens until the connection is closed
	while ( (temporary = read(fd, &data, data_size)) == data_size ){//printf("leu dados da msg com o tamanho certo (%d bytes)\n", temporary);
		if ( (data.region < 0) || (data.region > REGIONS_NR - 1)){
			printf("received wrong region from type (up/down) %d\n", reference);
			exit(-3);
		}

		if (data.order == COPY){
			if (reference == UP)
				update_region(&head, fd, data, data_size);
			else if (reference == DOWN)
				send_up_region(fd, data, data_size);
		}
		else
			send_region(fd, data, data_size, data.order);
	}//printf("acabou connection type (up/down) %d\n", reference);
}