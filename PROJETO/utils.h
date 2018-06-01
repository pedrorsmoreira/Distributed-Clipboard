/******************************************************************************
 *
 * File Name: utils.h
 *	      (c) 2018 PSis
 * Authors: Pedro Moreira e Inês Gaspar
 *
 * DESCRIPTION
 *		auxiliary functions
 *****************************************************************************/

//protect multiple inclusions
#ifndef UTILS
#define UTILS

#define MIN_PORT 1024
#define MAX_PORT 64738

/**
 * linked list struct with the connected
 * clipboard "clients" file descriptors
 */
typedef struct down_list_{
	int fd; 
	struct down_list_ *next;
}down_list; 


/**
 * @brief      leaves the program correctly if it
 * 				fails a system call function
 *
 * @param      indicator of where the error occured
 */
void system_error(char *function);

/**
 * @brief      initializes the variables for 
 * 			   multi-threading locks
 */
void init_locks();

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
int connected_clipboard_init(char *IP, char *port_); 

/**
 * @brief      Generates a "random" number in a certain
 *             range for a computer port connection
 *
 * @return     The "random" number
 */
int rand_port_gen(); 

/**
 * @brief      creates the pipe for the redundant single mode 
 * 			   connection (local clipboard is its own server)
 *
 * @return     pipe reading endpoint
 */
int redundant_server(); 

/**
 * @brief      Adds a node to the linked list of connected
 * 			   clipboard "clients" (starts the list if it was empty)
 *
 * @param      head            pointer to the head of the list
 * @param[in]  client_fd	   file descriptor of the new client
 *
 * @return     pointer to the head of the list
 */
down_list *add_down_list(down_list *head, int client_fd_send); 

/**
 * @brief      Removes from the list the node of a certain 
 * 			   clipboard "client" (indicated by client_fd)
 *
 * @param      head           pointer to the head of the list 
 * @param[in]  client_fd	  file descriptor the certain node
 *
 * @return     pointer to the head of the list
 */
down_list *remove_down_list(down_list *head, int client_fd_send);

#endif 