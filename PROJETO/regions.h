#ifndef REGIONS 
#define REGIONS 

#define WRITE 0
#define LIST 1

typedef struct REG{
	size_t size;
	void *message;
}REG;

typedef struct down_list_{
	int fd; 
	struct down_list_ *next;
}down_list; 


void init_mutex(); 
int redundant_server(); 
int connected_clipboard_init(char *IP, char *port_); 
void regions_init(int fd);
void update_region( down_list **head, int fd, Smessage data, int data_size);
void send_up_region(int fd, Smessage data, int data_size); 
void send_region(int fd, Smessage data, int data_size); 

#endif 