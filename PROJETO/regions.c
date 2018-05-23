#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

#include "clipboard.h"
#include "regions.h"

REG regions[REGIONS_NR];

int server_fd;
pthread_mutex_t mutex_init;
pthread_mutex_t mutex_writeUP;
pthread_rwlock_t regions_lock_rw[REGIONS_NR];
pthread_mutex_t wait_mutexes[REGIONS_NR];
pthread_cond_t wait_conditions[REGIONS_NR];

/**
 * @brief      initializes the local clipboard regions
 *
 * @param[in]  fd    file descriptor for the remote 
 * 					 clipboard to pull the regions from 
 * 					 (fd < 0  indicates single mode)
 */
void regions_init_local(int fd){
	for (int i = 0; i < REGIONS_NR; i++){
		regions[i].message = NULL;
		regions[i].size = 0;
	}

	if (fd > 0){
		void *aux = NULL;
		Smessage data;
		int data_size = sizeof(Smessage);
		do {
			if ( read(fd, &data, data_size) > 0){//VER ERRO DISSSOO
				if (data.message_size > 0)
					update_region(&aux, fd, data, data_size);
			}
		}while(data.region < REGIONS_NR - 1);
	}
}

/**
 * @brief      initializes a new connected clipboard regions
 *
 * @param[in]  fd    file descriptor for the new connected
 * 					 clipboard to push the regions to
 */
void regions_init_client(int fd){ //VER ERRRROOOOOSSSSS
for (int i = 0; i < REGIONS_NR; i++)
	clipboard_copy(fd , i, regions[i].message, regions[i].size);
}

/**
 * @brief      updates a clipboard region
 *
 * @param      head       passed by reference pointer to the 
 * 						  list of clipboard "clients"
 * @param[in]  fd         file descriptor to get the message from
 * @param[in]  data       structure with mesaage info
 * @param[in]  data_size  size of data (bytes)
 */
void update_region( down_list **head, int fd, Smessage data, int data_size){
	//lock the critical region access
	if (pthread_rwlock_wrlock(&regions_lock_rw[data.region]) != 0)
		system_error();
	
		//set the region to receive the new message
		if ( regions[data.region].message != NULL)
			free(regions[data.region].message);
		regions[data.region].size = data.message_size;
		regions[data.region].message = (void *) malloc (data.message_size);
		if ( regions[data.region].message == NULL)
			system_error();

		//read the message and copy it to its region
		if ( read(fd, regions[data.region].message, data.message_size) < 0)
			system_error();
		
	//unlock the critical region access
	if (pthread_rwlock_unlock(&regions_lock_rw[data.region]) != 0)
		system_error();

	//notice the pending waits if any
	if (pthread_cond_broadcast(&(wait_conditions[data.region])) != 0)
		system_error();

	//TEMPORARY PRINT FOR TESTING - TO BE DELETED-------------------------------------------------------------
	printf("copied %s to region %d\n", (char *) regions[data.region].message, data.region);

	//don't update while a clipboard is being initialized
	if (pthread_mutex_lock(&mutex_init) != 0)
		system_error();
		//update clipboard "clients"
		down_list *aux = *head;
		down_list *aux_next;
		while(aux != NULL){
			aux_next = aux->next;
			if (write(aux->fd, &data, data_size) <= 0){
				*head = remove_down_list(*head, aux->fd);
			}
			else if ( write(aux->fd, regions[data.region].message, data.message_size) <= 0){
				*head = remove_down_list(*head, aux->fd);
			}
		 aux = aux_next;
		}
	if (pthread_mutex_unlock(&mutex_init)!=0)
		system_error();
}

/**
 * @brief      Sends a received messsage to the clipboard "server"
 *
 * @param[in]  fd         endpoint for the clipboard "server"
 * @param[in]  data       structure with the message info
 * @param[in]  data_size  size of data (bytes)
 */
void send_up_region(int fd, Smessage data, int data_size){
	void *buf = (void *) malloc(data.message_size);
	if ( buf == NULL)
		system_error();

	//read the message
	if ( read(fd, buf, data.message_size) < 0)
		system_error();

	//lock the writes to clipboard "server"
	if (pthread_mutex_lock(&mutex_writeUP) != 0)
		system_error();
	
		//send up the message info
		if ( write(server_fd, &data, data_size) < 0)
			system_error();

		//send up the message
		if ( write(server_fd, buf, data.message_size) < 0)
			system_error();

	//unlock the writes to clipboard "server"
	if (pthread_mutex_unlock(&mutex_writeUP)!=0)
		system_error();

 free(buf);
}

/**
 * @brief      Sends a clipboard region message
 *
 * @param[in]  fd         file descriptor to send the message
 * @param[in]  data       struct with the message info
 * @param[in]  data_size  message size in bytes
 */
void send_region(int fd, Smessage data, int data_size, int order){
	//only leaves the if when the region is modified
	if (order == WAIT)	{
		if (pthread_mutex_lock(&(wait_mutexes[data.region])) != 0)
			system_error();
		
			if (pthread_cond_wait( &(wait_conditions[data.region]), &(wait_mutexes[data.region])) != 0)
				system_error();
			
		if (pthread_mutex_unlock( &(wait_mutexes[data.region])) < 0)
			system_error();
		
	}

	//lock the critical region for modifications
	if (pthread_rwlock_rdlock(&regions_lock_rw[data.region]) != 0)
		system_error();
	
		//check if there's anything to paste
		if (regions[data.region].message == NULL){
			printf("nothing to paste in region %d \n", data.region);
			//no message will be sent, unlock the critical region
			if (pthread_rwlock_unlock(&regions_lock_rw[data.region]) != 0)
				system_error();
			
			data.region = -1;
		}
		else
			data.message_size = regions[data.region].size;
		
		//send the message info
		if ( write(fd, &data, data_size) < 0)
			system_error();

		//if there was nothing to paste, leaves
		if (data.region == -1)
			return;

		//send the message requested
		if ( write(fd, regions[data.region].message, data.message_size) < 0)
			system_error();
		
	//unlock the critical region
	if (pthread_rwlock_unlock(&regions_lock_rw[data.region]) != 0)
		system_error();
	

	//TEMPORARY PRINT FOR TESTING - TO BE DELETED--------------------------------------------------------------
	printf("pasted %s from region %d\n", (char *) regions[data.region].message, data.region);
}