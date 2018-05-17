#include "utils.h"
#include "clipboard_imp.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <pthread.h>

/**
 * @brief      Generates a random number in a valid
 *             range for a computer port connection
 *
 * @return     The random number
 */
int rand_port_gen(){
	srand(pthread_self());
	int ret = 1024 + rand()%63715;
	return ret;
}

down_list *add_down_list(down_list *head, int client_fd_send){
	down_list *new_head = (down_list*) malloc (sizeof(down_list));
	new_head->fd = client_fd_send;
	new_head->next = head;
	return new_head;
}

down_list *remove_down_list(down_list *head, int client_fd_send){
	if (head->fd == client_fd_send){
		down_list *next = head->next;
		free(head);
		return next;
	}

	down_list *head_ret = head;
	while (head->next->fd != client_fd_send)
		head = head->next;

	down_list * aux = head->next->next; 
	free(head->next);
	head->next=aux;

 return head_ret;
}

