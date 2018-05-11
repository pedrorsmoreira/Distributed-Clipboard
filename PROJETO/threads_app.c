#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <pthread.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "clipboard.h"
#include "threads.h"

REG regions[REGIONS_NR];

//COMMENT THIS CODE WHEN BOTH CASES ARE WRITTEN
void regions_init(){
	// if (working alone)
		for (int i = 0; i <REGIONS_NR; i++)
			regions[i].message = NULL;
	// else .....
}

void *app_init(){
	struct sockaddr_un local_addr;
	
	//assure there was no previous socket with the same name
	unlink(SOCK_ADDRESS);

	//create a socket stream for app_server_addr comunitacion
	int *sock_fd = (int *) malloc(sizeof(int));
	*sock_fd = socket(AF_UNIX, SOCK_STREAM, 0);
	if (*sock_fd == -1){
		perror("socket: ");
		exit (-1);
	}

	//set the local communication parameters
	local_addr.sun_family = AF_UNIX;
	strcpy(local_addr.sun_path, SOCK_ADDRESS);

	//adress the socket (own it)
	if ( bind(*sock_fd, (struct sockaddr *) &local_addr, sizeof(local_addr)) < 0){
		perror("bind: ");
		exit (-1);
	}

	//get ready to act as a server
	if (listen (*sock_fd, 2) == -1){
		perror("listen: ");
		exit (-1);
	}
 return (void *) sock_fd;
}

void *app_accept(void * CS){
	client_socket *CS_ = (client_socket *) CS;
	
	//stablish connection with the app
	int client_fd = accept( CS_->sock_fd, (struct sockaddr *) &(CS_->addr), &(CS_->size));
	if (client_fd == -1){
		perror("accept: ");
		exit (-1);
	}
	
	//create new thread for next app connection
	pthread_t thread_id;
	if (pthread_create(&thread_id, NULL, app_thread, CS) != 0){
		perror("pthread_create: ");
		exit(-1);
	}
	
	//answer current app requests
	app_handle(client_fd);
	//HAD TO PUT SOMETHING ON RETURN - MEANINGLESS - CHANGE IT AFTERWARDS
	return CS;
}


void app_handle(int client_fd){
	Smessage data;
	int data_size = sizeof(Smessage);

	while ( read(client_fd, &data, data_size) > 0){
			if ( (data.region < 0) || (data.region > REGIONS_NR))	
				exit(-2);
			
			if (data.order == COPY){	
				// if something is already copied in this region, replace it
				if ( regions[data.region].message != NULL)
					free(regions[data.region].message);

				regions[data.region].size = data.message_size;
				regions[data.region].message = (void *) malloc (data.message_size);
				if ( regions[data.region].message == NULL){
					printf ("malloc failure\n");
					exit (-1);
				}

				//read the message and copy it
				if ( read(client_fd, regions[data.region].message, data.message_size) < 0){
					perror("read: ");
					exit(-1);
				}
				
				//TEMPORARY PRINT FOR TESTING - TO BE DELETED
				printf("copied %s to region %d\n", (char *) regions[data.region].message, data.region);	
			}else if (data.order == PASTE){
				//check if there's anything to paste
				if (regions[data.region].message == NULL){
					printf("nothing to paste in region %d \n", data.region);
					data.region = -1;
				}
				else
					data.message_size = regions[data.region].size;
				
				//enviar de volta a estrutura
				if ( write(client_fd, &data, data_size) < 0){
					perror("write: ");
					exit(-1);
				}

				if (data.region == -1)	
					continue;
				
				//send the message requested
				if ( write(client_fd, regions[data.region].message, data.message_size) < 0){
					perror("write: ");
					exit(-1);
				}
		
			}
			else exit(-2);
		}
}
