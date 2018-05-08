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
#include "clipboard.h"
 
int main(){
	struct sockaddr_un local_addr;
	struct sockaddr_un client_addr;
	socklen_t size_addr = sizeof(struct sockaddr);

	unlink(SOCK_ADDRESS);

	//create a socket stream for local comunitacion
	int sock_fd = socket(AF_LOCAL, SOCK_STREAM, 0);
	if (sock_fd == -1){
		perror("socket: ");
		exit (-1);
	}
	local_addr.sun_family = AF_UNIX;
	strcpy(local_addr.sun_path, SOCK_ADDRESS);

	if (bind(sock_fd, (struct sockaddr *) &local_addr, sizeof(local_addr)) ){
		perror("bind: ");
		exit (-1);
	}

	//EU SOU SERVIDOR
	if (listen (sock_fd, 2) == -1){
		perror("listen: ");
		exit (-1);
	}

//declare and initialize variables for the actions cicle
	Smessage data;
	char *regions[REGIONS_NR];
	for (int i = 0; i <REGIONS_NR; i++) regions[i] = NULL;
	int data_size = DATA_SIZE;
	
//actions cicle
	while(1){
		printf(".\n");
		//stablish connection with client
		int client_fd = accept(sock_fd, (struct sockaddr *) &client_addr, &size_addr);
		if (client_fd == -1){
		perror("accept: ");
		exit (-1);
		}
		
		//chamar a thread accept aquiiiii 
		thread_accept(sock_fd, client_addr, size_addr);
		//read data
		int err_read;

		while ( (err_read = read(client_fd, &data, data_size)) > 0){
			if (err_read == -1 ){
				perror("read: ");
				exit(-1);
			}
			
			if ( (data.region < 0) || (data.region > REGIONS_NR))	exit(-2);
			if (data.order == COPY){
				// if something is already copied in this region, replace it
				if ( regions[data.region] != NULL) free(regions[data.region]);

				// copy the message
				regions[data.region] = (char *) malloc (data.message_size);
				if ( regions[data.region] == NULL){
					printf ("malloc failure\n");
					exit (-1);
				}

			
				if ( ( err_read = read(client_fd, regions[data.region], data.message_size) ) == -1){
					perror("read: ");
					exit(-1);
				}
				
				//print
				printf("copied %s to region %d\n", regions[data.region], data.region);	
			}else if (data.order == PASTE){
				//check if there anything to paste
				if (regions[data.region] == NULL){
					printf("nothing to paste in region %d \n", data.region);
					data.region = -1;
				}
				else
				data.message_size = sizeof (regions[data.region]);
				
				//enviar de volta a estrutura
				
				if ( write(client_fd, &data, data_size) == -1){
					perror("write: ");
					exit(-1);
				}

				if (data.region == -1)	continue;

				int err_w;
				
					if ( ( err_w = write(client_fd, regions[data.region], data.message_size) ) == -1){
						perror("write: ");
						exit(-1);
					}
		
			}
			else exit(-2);
		}
	}
	//final clean
	/*for (int i = 0; i <REGIONS_NR; i++) free(regions[i]);
	unlink(file_name_out);
	close(fifo_out);
	unlink(file_name_in);
	close(fifo_in);
	exit(0);*/	
}