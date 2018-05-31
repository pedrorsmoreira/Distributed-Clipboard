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

void system_error();
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
 * @brief      Generates a "random" number in a valid
 *             range for a computer port connection
 *
 * @return     The "random" number
 */
int rand_port_gen(); 

/**
 * @brief      creates the pipe for the redundant single mode 
 * 			   connection (local clipboard is its own server)
 *
 * @return     pipte reading endpoint
 */
int redundant_server(); 
/**
 * @brief      Adds a block to the linked list of connected
 * 			   clipboard "clients" (starts the list if it was empty)
 *
 * @param      head            pointer to the head of the list
 * @param[in]  client_fd	   file descriptor for the new client
 *
 * @return     pointer to the head of the list
 */
down_list *add_down_list(down_list *head, int client_fd_send); 
/**
 * @brief      Removes from the down_list the block of a 
 * 			   clipboard "client" which disconnected
 *
 * @param      head           pointer to the head of the list 
 * @param[in]  client_fd	  file descriptor of the disconnected client
 *
 * @return     pointer to the head of the list
 */
down_list *remove_down_list(down_list *head, int client_fd_send);



#endif 