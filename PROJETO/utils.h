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
void init_locks(int regions_nr);
int connected_clipboard_init(char *IP, char *port_); 
int rand_port_gen(); 
int redundant_server(); 
down_list *add_down_list(down_list *head, int client_fd_send); 
down_list *remove_down_list(down_list *head, int client_fd_send);



#endif 