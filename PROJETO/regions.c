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
	if (pthread_rwlock_wrlock(&regions_lock_rw[data.region]) != 0){
		printf("write lock faliure\n");
		exit(-1);
	}
		//set the region to receive the new message
		if ( regions[data.region].message != NULL)
			free(regions[data.region].message);
		regions[data.region].size = data.message_size;
		regions[data.region].message = (void *) malloc (data.message_size);
		if ( regions[data.region].message == NULL){
			printf ("malloc failure\n");
			exit (-1);
		}

		//read the message and copy it to its region
		if ( read(fd, regions[data.region].message, data.message_size) < 0){
			perror("read: ");
			exit(-1);
		}
	//unlock the critical region access
	if (pthread_rwlock_unlock(&regions_lock_rw[data.region]) != 0){
		printf("write unlock faliure\n");
		exit(-1);
	}

	//notice the pending waits if any
	if (pthread_cond_broadcast(&(wait_conditions[data.region])) != 0){
		printf("pthread_cond_broadcast failure\n");
		exit(-1);
	}

	//TEMPORARY PRINT FOR TESTING - TO BE DELETED-------------------------------------------------------------
	printf("copied %s to region %d\n", (char *) regions[data.region].message, data.region);

	//don't update while a clipboard is being initialized
	if (pthread_mutex_lock(&mutex_init) != 0){
		printf("mutex writeUP lock failure\n");
		exit(-1);
	}
		//update clipboard "clients"
		down_list *aux = *head;
		down_list *aux_next;
		while(aux != NULL){
			aux_next = aux->next;
			if (write(aux->fd, &data, data_size) < 0){printf("remv list1\n");
				*head = remove_down_list(*head, aux->fd);
			}
			else if ( write(aux->fd, regions[data.region].message, data.message_size) < 0){printf("remv list2\n");
				*head = remove_down_list(*head, aux->fd);
			}
		 aux = aux_next;
		}
	if (pthread_mutex_unlock(&mutex_init)!=0){
		printf("mutex writeUP unlock failure\n");
		exit(-1);
	}
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
	if ( buf == NULL){
		printf ("malloc failure\n");
		exit (-1);
	}

	//read the message
	if ( read(fd, buf, data.message_size) < 0){
		perror("read: ");
		exit(-1);
	}

	//lock the writes to clipboard "server"
	if (pthread_mutex_lock(&mutex_writeUP) != 0){
		printf("mutex writeUP lock failure\n");
		exit(-1);
	}
		//send up the message info
		if ( write(server_fd, &data, data_size) < 0){
			perror("write: ");
			exit(-1);
		}

		//send up the message
		if ( write(server_fd, buf, data.message_size) < 0){
			perror("write: ");
			exit(-1);
		}
	//unlock the writes to clipboard "server"
	if (pthread_mutex_unlock(&mutex_writeUP)!=0){
		printf("mutex writeUP unlock failure\n");
		exit(-1);
	}
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
		if (pthread_mutex_lock(&(wait_mutexes[data.region])) != 0){
			printf("wait_mutex%d lock failure", data.region);
			exit(-1);
		}
			if (pthread_cond_wait( &(wait_conditions[data.region]), &(wait_mutexes[data.region])) != 0){
				printf("cond_wait failure\n");
				exit(-1);
			}
		if (pthread_mutex_unlock( &(wait_mutexes[data.region])) < 0){
			printf("wait_mutex%d unlock failure", data.region);
			exit(-1);
		}
	}

	//lock the critical region for modifications
	if (pthread_rwlock_rdlock(&regions_lock_rw[data.region]) != 0){
		printf("read lock failure\n");
		exit(-1);
	}
		//check if there's anything to paste
		if (regions[data.region].message == NULL){
			printf("nothing to paste in region %d \n", data.region);
			//no message will be sent, unlock the critical region
			if (pthread_rwlock_unlock(&regions_lock_rw[data.region]) != 0){	
				printf("read unlock failure (when message = NULL)\n");
				exit(-1);
			}
			data.region = -1;
		}
		else
			data.message_size = regions[data.region].size;
		
		//send the message info
		if ( write(fd, &data, data_size) < 0){
			perror("write: ");
			exit(-1);
		}

		//if there was nothing to paste, leaves
		if (data.region == -1)
			return;

		//send the message requested
		if ( write(fd, regions[data.region].message, data.message_size) < 0){
			perror("write: ");
			exit(-1);
		}
	//unlock the critical region
	if (pthread_rwlock_unlock(&regions_lock_rw[data.region]) != 0){
		printf("read unlock failure\n");
		exit(-1);
	}

	//TEMPORARY PRINT FOR TESTING - TO BE DELETED--------------------------------------------------------------
	printf("pasted %s from region %d\n", (char *) regions[data.region].message, data.region);
}