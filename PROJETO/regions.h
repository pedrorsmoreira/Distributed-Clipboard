#ifndef REGIONS 
#define REGIONS 

#include "utils.h"

/**
 * clipboard regions structure
 */
typedef struct REG{
	size_t size;
	void *message;
}REG;

void regions_init(int fd);
void update_region( down_list **head, int fd, Smessage data, int data_size);
void send_up_region(int fd, Smessage data, int data_size); 
void send_region(int fd, Smessage data, int data_size, int order); 

#endif 