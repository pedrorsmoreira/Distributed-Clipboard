#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <arpa/inet.h>


int main(){
 
 	struct sockaddr_un local_addr;
	struct sockaddr_un client_addr;
	socklen_t size_addr;

	//create a socket stream for internet comunication
	int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (sock_fd == -1){
		perror("socket: ");
		exit (-1);
	}
	local_addr.sin_family = AF_INET;
	local_addr.sin_port=htons(3000);
	local_addr.sin_addr.s_addr=INADDR_ANY;

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
		//nao  interessa
	int data_size = DATA_SIZE;
	char *bytestream = (char *) malloc(data_size);
	if ( bytestream == NULL){
		printf ("malloc failure\n");
		exit (-1);
	}

	while(1){
		printf(".\n");
		//stablish connection with client
		int client_fd = accept(sock_fd, (struct sockaddr *) &client_addr, &size_addr);
		if (client_fd == -1){
		perror("accept: ");
		exit (-1);
		}

		int err_read;
		while ( (err_read = read(, client_fd, data_size)) > 0){
			if (err_read == -1 ){
				perror("read: ");
				exit(-1);
			}
	}




}
