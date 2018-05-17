#ifndef UTILS
#define UTILS

typedef struct down_list_{
	int fd; 
	struct down_list_ *next;
}down_list; 

int rand_port_gen(); 
down_list *add_down_list(down_list *head, int client_fd_send); 
down_list *remove_down_list(down_list *head, int client_fd_send);



#endif 