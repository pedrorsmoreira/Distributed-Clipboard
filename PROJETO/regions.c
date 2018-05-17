#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <pthread.h>
#include <arpa/inet.h>

#include "clipboard.h"
#include "regions.h"
#include "threads.h"
#include "utils.h"

int server_fd_send;
REG regions[REGIONS_NR];
pthread_mutex_t mutex_writeUP;

void init_mutex(){
	if(pthread_mutex_init(&mutex_writeUP, NULL) != 0){	
		perror("mutex init: ");
		exit(-1); 
	}
}

int redundant_server(){
	int fd[2];
	if (pipe(fd)< 0){
		perror("pipe: ");
		exit(-1);
	}
	server_fd_send = fd[1];
 
 return fd[0];
}

int connected_clipboard_init(char *IP, char *port_){
	int port = atoi(port_);
	int server_fd_receive;
	
	//set the connection paremeters
	struct sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);
	inet_aton(IP, &server_addr.sin_addr);
	
	//create the endpoints to connect
	if ((server_fd_receive = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		perror("socket: ");
		exit(-1);
	}
	if ((server_fd_send = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		perror("socket: ");
		exit(-1);
	}

	//connect with clipboard "server" to send up
	if(connect(server_fd_send, (const struct sockaddr *) &server_addr, sizeof(server_addr))<0){
	printf("Error connecting1!!!\n");
	exit(-1);
	}

	//receive the second endpoint the to connect
	if ( read(server_fd_send, &port, sizeof(int)) < 0){
		perror("write: ");
		exit(-1);
	}
	server_addr.sin_port = htons(port);
	//connect with clipboard "server" to receive updates
	if(connect(server_fd_receive, (const struct sockaddr *) &server_addr, sizeof(server_addr))<0){
	printf("Error connecting2!!!\n");
	exit(-1);
	}

	//upload regions from the clipboard "server"
	regions_init(server_fd_send);

 return server_fd_receive;
}

/**
 * @brief      initializes the local clipboard regions
 *
 * @param[in]  fd    file descriptor for the remote 
 * 					 clipboard to pull the regions from 
 * 					 (fd < 0  indicates single mode)
 */
void regions_init(int fd){
	int i;
	if (fd < 0){
		for (i = 0; i <REGIONS_NR; i++)
			regions[i].message = NULL;
	}
	else{
		for (i = 0; i <REGIONS_NR; i++)
			regions[i].size = clipboard_paste(fd, i, regions[i].message, -1);
	}
}

/**
 * @brief      updates a clipboard region
 *
 * @param[in]  fd         file descriptor to recieve the message
 * @param[in]  data   //handle remote clipboards (one thread per clipboard) (recv)
	if (pthread_create(&thread_id_in, NULL, accept_clients, &CS_in) != 0){
		perror("pthread_create: ");
		exit(-1);
	}    struct with message info
 * @param[in]  data_size  message size in bytes
 */
void update_region( down_list **head, int fd, Smessage data, int data_size){
	if ( regions[data.region].message != NULL)
		free(regions[data.region].message);

	regions[data.region].size = data.message_size;
	regions[data.region].message = (void *) malloc (data.message_size);
	if ( regions[data.region].message == NULL){
		printf ("malloc failure\n");
		exit (-1);
	}

	//read the message and copy it
	if ( read(fd, regions[data.region].message, data.message_size) < 0){
		perror("read: ");
		exit(-1);
	}
	
	//TEMPORARY PRINT FOR TESTING - TO BE DELETED
	printf("copied %s to region %d\n", (char *) regions[data.region].message, data.region);

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
}

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

	//lock the mutex 
	if (pthread_mutex_lock(&mutex_writeUP)!=0){
		perror("mutex lock:");
		exit(-1);
	}

	////////CRITICAL REGION 
	//send up the message info
	if ( write(server_fd_send, &data, data_size) < 0){
		perror("write: ");
		exit(-1);
	}

	//send up the message
	if ( write(server_fd_send, buf, data.message_size) < 0){
		perror("write: ");
		exit(-1);
	}
	////////END OF CRITICAL REGION

	//unlock the mutex 
	if (pthread_mutex_unlock(&mutex_writeUP)!=0){
		perror("mutex unlock:");
		exit(-1);
	}
 free(buf);
}

/**
 * @brief      Sends a cilpboard region message
 *
 * @param[in]  fd         file descriptor to send the message
 * @param[in]  data       struct with the message info
 * @param[in]  data_size  message size in bytes
 */
void send_region(int fd, Smessage data, int data_size){
	//check if there's anything to paste
	if (regions[data.region].message == NULL){
		printf("nothing to paste in region %d \n", data.region);
		data.region = -1;
	}
	else
		data.message_size = regions[data.region].size;
	
	//send the message info
	if ( write(fd, &data, data_size) < 0){
		perror("write: ");
		exit(-1);
	}

	if (data.region == -1)	
		return;
	
	//send the message requested
	if ( write(fd, regions[data.region].message, data.message_size) < 0){
		perror("write: ");
		exit(-1);
	}
	//TEMPORARY PRINT FOR TESTING - TO BE DELETED
	printf("pasted %s from region %d\n", (char *) regions[data.region].message, data.region);
}
