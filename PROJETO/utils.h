#ifndef UTILS
#define UTILS

#include "regions.h"

int rand_port_gen(); 
down_list *add_down_list(down_list *head, int client_fd_send); 
down_list *remove_down_list(down_list *head, int client_fd_send);



#endif 