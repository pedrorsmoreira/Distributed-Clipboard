#include "utils.h"

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <fcntl.h>

#include "clipboard.h"

extern int server_fd;
extern pthread_mutex_t mutex_init;
extern pthread_mutex_t mutex_writeUP;
extern pthread_rwlock_t regions_lock_rw[];
extern pthread_mutex_t wait_mutexes[];
extern pthread_cond_t wait_conditions[];


void system_error(char *function){
	printf("%s:\n", function);
	perror("ERROR: ");
	unlink(SOCK_ADDRESS);
	exit(-1);
}

/**
 * @brief      initializes the variables for 
 * 			   multi-threading locks
 */
void init_locks(){
	if(pthread_mutex_init(&mutex_writeUP, NULL) != 0)
		system_error("mutex_writeUP init"); 
	
	if(pthread_mutex_init(&mutex_init, NULL) != 0)
		system_error("mutex_init init");

	for (int i = 0; i < REGIONS_NR; i ++){
		if(pthread_rwlock_init(&regions_lock_rw[i], NULL) != 0)
			system_error("regions_lock_rw init"); 

		if(pthread_mutex_init(&wait_mutexes[i], NULL) != 0)
			system_error("wait_mutexes init"); 

		if(pthread_cond_init(&wait_conditions[i], NULL) != 0)
			system_error("wait_conditions init"); 
	}
}

/**
 * @brief      initializes a clipboard in connected mode
 * 			   (connects with the clipboard "server")
 *
 * @param      IP     clipboard "server" IP
 * @param      port_  clipboard "server" openned 
 * 					  port to receive connections
 *
 * @return     endpoint to clipboard "server" connection
 */
int connected_clipboard_init(char *IP, char *port_){
	//set the connection parameters
	struct sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	int port = atoi(port_);
	server_addr.sin_port = htons(port);
	inet_aton(IP, &server_addr.sin_addr);
	
	//create the endpoint to connect
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		system_error("socket() in connected_clipboard_init"); 

	//connect with clipboard "server"
	if(connect(server_fd, (const struct sockaddr *) &server_addr, sizeof(server_addr))<0)
		return -1;

 return server_fd;
}

/**
 * @brief      Generates a "random" number in a valid
 *             range for a computer port connection
 *
 * @return     The "random" number
 */
int rand_port_gen(){
	srand(pthread_self());
	int ret = MIN_PORT + rand()%(MAX_PORT - MIN_PORT + 1);
	return ret;
}


/**
 * @brief      creates the pipe for the redundant single mode 
 * 			   connection (local clipboard is its own server)
 *
 * @return     pipte reading endpoint
 */
int redundant_server(){
	int fd[2];
	if (pipe(fd) < 0)
		system_error("pipe creation"); 
	
	server_fd = fd[1];
 
 return fd[0];
}

/**
 * @brief      Adds a block to the linked list of connected
 * 			   clipboard "clients" (starts the list if it was empty)
 *
 * @param      head            pointer to the head of the list
 * @param[in]  client_fd	   file descriptor for the new client
 *
 * @return     pointer to the head of the list
 */
down_list *add_down_list(down_list *head, int client_fd){
	down_list *new_head = (down_list*) malloc (sizeof(down_list));
	new_head->fd = client_fd;
	new_head->next = head;
	return new_head;
}

/**
 * @brief      Removes from the down_list the block of a 
 * 			   clipboard "client" which disconnected
 *
 * @param      head           pointer to the head of the list 
 * @param[in]  client_fd	  file descriptor of the disconnected client
 *
 * @return     pointer to the head of the list
 */
down_list *remove_down_list(down_list *head, int client_fd){
	//close the socket of the finnished connection
	if (fcntl(client_fd, F_GETFD) != -1)
		close(client_fd);

	down_list *head_ret = head;

	if (head->fd == client_fd){
		down_list *next = head->next;
		free(head);
		return next;
	}

	while (head->next->fd != client_fd)
		head = head->next;

	down_list * aux = head->next->next; 
	free(head->next);
	head->next=aux;

 return head_ret;
}

