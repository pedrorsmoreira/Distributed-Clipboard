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

	//recieve the initialization from the server
	//(server is running regions_init_client)
	if (fd > 0){
		void *aux = NULL;
		Smessage data;
		int data_size = sizeof(Smessage);
		do {
			if (read(fd, &data, data_size) != data_size)
				return;

			if (data.message_size > 0)
				update_region(&aux, fd, data, data_size);
		}while(data.region > 0);
	}
}

/**
 * @brief      initializes a new connected clipboard regions
 *
 * @param[in]  fd    file descriptor for the new connected
 * 					 clipboard to push the regions to
 */
void regions_init_client(int fd){	
for (int i = 0; i < REGIONS_NR; i++){
	//lock the critical region for modifications
	if (pthread_rwlock_rdlock(&regions_lock_rw[i]) != 0)
		system_error("regions_lock_rw lock in regions_init_client");

		if (regions[i].size > 0)
			clipboard_copy(fd , i, regions[i].message, regions[i].size);
	
	if (pthread_rwlock_unlock(&regions_lock_rw[i]) != 0)
	system_error("regions_lock_rw unlock in regions_init_client");
}

//inform client that the initialization is over
Smessage data;
data.region = -1;
data.message_size = 0;
if ( write(fd, &data, sizeof(Smessage)) != sizeof(Smessage) )
	close(fd);
}

/**
 * @brief      updates a clipboard region
 *
 * @param      head       passed by reference pointer to the 
 * 						  list of clipboard "clients"
 * @param[in]  fd         file descriptor to get the message from
 * @param[in]  data       structure with the message info
 * @param[in]  data_size  size of struct data (bytes)
 */
void update_region( down_list **head, int fd, Smessage data, int data_size){
	//read the message to be saved
	void *buf = (void *) malloc (data.message_size);
	if(buf == NULL)
		system_error("malloc in update");
	if (read(fd, buf, data.message_size) != data.message_size){
		free(buf);
		return;
	}

	//lock the critical region access
	if (pthread_rwlock_wrlock(&regions_lock_rw[data.region]) != 0)
		system_error("write_lock in update");
	
		//update region with new message
		if ( regions[data.region].message != NULL)
			free(regions[data.region].message);
		regions[data.region].message = buf;
		regions[data.region].size = data.message_size;
		
	if (pthread_rwlock_unlock(&regions_lock_rw[data.region]) != 0)
		system_error("write_unlock in update");

	//notice the pending waits if any
	if (pthread_cond_broadcast(&(wait_conditions[data.region])) != 0)
		system_error("broadcast in update");

	//don't update while a clipboard is being initialized
	if (pthread_mutex_lock(&mutex_init) != 0)
		system_error("mutex_init lock in update");
		//update clipboard "clients"
		down_list *aux = *head;
		down_list *aux_next;
		while(aux != NULL){
			aux_next = aux->next;
			if (write(aux->fd, &data, data_size) != data_size)
				*head = remove_down_list(*head, aux->fd);
			else if ( write(aux->fd, regions[data.region].message, data.message_size) != data.message_size)
				*head = remove_down_list(*head, aux->fd);

		 	aux = aux_next;
		}

	if (pthread_mutex_unlock(&mutex_init)!=0)
		system_error("mutex_init unlock in update");
}

/**
 * @brief      Sends a received messsage to the clipboard "server"
 *
 * @param[in]  fd         endpoint for the clipboard "server"
 * @param[in]  data       structure with the message info
 * @param[in]  data_size  size of struct data (bytes)
 */
void send_up_region(int fd, Smessage data, int data_size){
	void *buf = (void *) malloc(data.message_size);
	if ( buf == NULL)
		return;

	//read the message
	if ( read(fd, buf, data.message_size) != data.message_size){
		free(buf);
		return;
	}

	//lock the writes to clipboard "server"
	if (pthread_mutex_lock(&mutex_writeUP) != 0)
		system_error("mutex_writeUP lock in send_up");
	
		clipboard_copy(server_fd, data.region, buf, data.message_size);
		
	if (pthread_mutex_unlock(&mutex_writeUP)!=0)
		system_error("mutex_writeUP unlock in send_up");

 free(buf);
}

/**
 * @brief      sends a clipboard region to a client
 *
 * @param[in]  fd         client file descriptor
 * @param[in]  data       structure with the message info
 * @param[in]  data_size  size of struct data (bytes)
 * @param[in]  order      to distuinguish pastes from waits
 */
void send_region(int fd, Smessage data, int data_size, int order){
	//only leaves the if when the region is modified
	if (order == WAIT)	{
		if (pthread_mutex_lock(&(wait_mutexes[data.region])) != 0)
			system_error("wait_mutexes lock in send");
		
			if (pthread_cond_wait( &(wait_conditions[data.region]), &(wait_mutexes[data.region])) != 0)
				system_error("cond_wait in send");
			
		if (pthread_mutex_unlock( &(wait_mutexes[data.region])) != 0)
			system_error("wait_mutexes unlock in send");
		
	}

	//lock the critical region for modifications
	if (pthread_rwlock_rdlock(&regions_lock_rw[data.region]) != 0)
		system_error("regions_lock_rw lock in send");
	
		//check if there's anything to paste
		if (regions[data.region].message == NULL || regions[data.region].size > data.message_size){
			if (pthread_rwlock_unlock(&regions_lock_rw[data.region]) != 0)
				system_error("regions_lock_rw (no message) unlock in send");
			
			data.region = -1;
		}
		else
			data.message_size = regions[data.region].size;
		
		//send the message info
		if ( write(fd, &data, data_size) != data_size){
			if (pthread_rwlock_unlock(&regions_lock_rw[data.region]) != 0)
				system_error("regions_lock_rw (no message) unlock in send");

			return;
		}
		
		//if there was nothing to paste, leaves
		if (data.region == -1)
			return;

		//send the message requested
		if (write(fd, regions[data.region].message, data.message_size) != data.message_size){
			if (pthread_rwlock_unlock(&regions_lock_rw[data.region]) != 0)
				system_error("regions_lock_rw (no message) unlock in send");

			return;
		}

	//unlock the critical region
	if (pthread_rwlock_unlock(&regions_lock_rw[data.region]) != 0)
		system_error("regions_lock_rw unlock in send");
}