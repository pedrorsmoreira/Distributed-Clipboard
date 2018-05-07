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
	//bytestream holds the struct info to be sent through the socket
	int data_size = DATA_SIZE;
	char *bytestream = (char *) malloc(data_size);
	if ( bytestream == NULL){
		printf ("malloc failure\n");
		exit (-1);
	}
//actions cicle
	while(1){
		printf(".\n");
		//stablish connection with client
		int client_fd = accept(sock_fd, (struct sockaddr *) &client_addr, &size_addr);
		if (client_fd == -1){
		perror("accept: ");
		exit (-1);
		}
		//read data
		int err_read;
		while ( (err_read = read(client_fd, bytestream, data_size)) > 0){
			if (err_read == -1 ){
				perror("read: ");
				exit(-1);
			}
			if (memcpy((void *) &data, (void *) bytestream, data_size) == NULL){
				perror("memcpy: ");
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

				int bytes_read = 0;
				//isto é necessário? o read e o write tem "Limite"?
				while ( bytes_read < data.message_size){
				if ( ( err_read = read(client_fd, regions[data.region][bytes_read], data.message_size) ) == -1){
					perror("read: ");
					exit(-1);
				}
				bytes_read += err_read;
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
				if (memcpy((void *) bytestream,(void *) &data, data_size) == NULL){
					perror("memcpy: ");
					exit(-1);
				}

				if ( write(client_fd, bytestream, data_size) == -1){
					perror("write: ");
					exit(-1);
				}

				if (data.region == -1)	continue;

				int bytes_w = 0;
				int err_w;
				while (bytes_w < data.message_size){
					if ( ( err_w = write(client_fd, regions[data.region][bytes_w], data.message_size) ) == -1){
						perror("write: ");
						exit(-1);
					}
				bytes_w += err_w;
				}
			}
			else exit(-2);
		}
	}
	//final clean
	/*for (int i = 0; i <REGIONS_NR; i++) free(regions[i]);
	free(bytestream);
	unlink(file_name_out);
	close(fifo_out);
	unlink(file_name_in);
	close(fifo_in);
	exit(0);*/	
}