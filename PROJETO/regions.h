typedef struct REG{
	size_t size;
	void *message;
}REG;

typedef struct down_list_{
	int fd; 
	struct down_list_ *next;
}down_list; 

void regions_init(int fd);
void update_region( int client_fd, Smessage data, int data_size);
void send_region(int client_fd, Smessage data, int data_size);