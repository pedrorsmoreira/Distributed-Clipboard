#include "utils.h"

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>

extern int server_fd;
extern pthread_mutex_t mutex_writeUP;
extern pthread_rwlock_t regions_lock_rw[];
extern pthread_mutex_t wait_mutexes[];
extern pthread_cond_t wait_conditions[];
extern int pend_waits[];


void invalid_args_exit(){
	printf("invalid arguments\n");
	exit(-2);
}

/**
 * @brief      initializes the variables for 
 * 			   multi-threading locks
 */
void init_locks(int regions_nr){
	if(pthread_mutex_init(&mutex_writeUP, NULL) != 0){	
		perror("mutex init: ");
		exit(-1); 
	}
	for (int i = 0; i < regions_nr; i ++){
		if(pthread_rwlock_init(&regions_lock_rw[i], NULL) != 0){	
			perror("regions_lock_rw init: ");
			exit(-1); 
		}
		if(pthread_mutex_init(&wait_mutexes[i], NULL) != 0){	
			perror("regions_lock_rw init: ");
			exit(-1); 
		}
		if(pthread_cond_init(&wait_conditions[i], NULL) != 0){	
			perror("regions_lock_rw init: ");
			exit(-1); 
		}
		pend_waits[i] = 0;
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
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		perror("socket: ");
		exit(-1);
	}

	//connect with clipboard "server"
	if(connect(server_fd, (const struct sockaddr *) &server_addr, sizeof(server_addr))<0){
	printf("Error connecting with the server!!!\n");
	exit(-1);
	}

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
	int ret = 1024 + rand()%63715;
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
	if (pipe(fd)< 0){
		perror("pipe: ");
		exit(-1);
	}
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

