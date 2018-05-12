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
#include "threads.h"
#include "regions.h"

 
int main(int argc, char **argv){		
	
	if (argc == 4){
		if(strcmp(argv[1], "-c") == 0)
		{
			int port=atoi(argv[3]);
			struct sockaddr_in server_addr;
			int sock_fd= socket(AF_INET, SOCK_STREAM, 0);
			if (sock_fd == -1){
				perror("socket: ");
				exit(-1);
			}

			server_addr.sin_family = AF_INET;
			server_addr.sin_port= htons(port);
			inet_aton(argv[2], &server_addr.sin_addr);
		
			if(connect(sock_fd, (const struct sockaddr *) &server_addr, sizeof(server_addr))<0){
			printf("Error connecting!!!\n");
			exit(-1);
			}

			regions_init(sock_fd);
		}
		else{
		printf("invalid arguments1\n");
		exit(-2);
		}
	}
	else if (argc == 1)
		regions_init(-1);
	else{
		printf("invalid arguments2\n");
		exit(-2);
	}

	//launche the server to handle local apps
	pthread_t thread_id_un;
	if (pthread_create(&thread_id_un, NULL, server_init, (void *) UNIX) != 0){
		perror("pthread_create: ");
		exit(-1);
	}

	//launche the server to handle remote clipboard
	pthread_t thread_id_in;
	if (pthread_create(&thread_id_in, NULL, server_init, (void *) INET) != 0){
		perror("pthread_create: ");
		exit(-1);
	}

//////////////////////////UNIX
	int *sock_fd_un = NULL;
	pthread_join(thread_id_un, (void **) &sock_fd_un);

	//struct with the client adress info to send to the thread
	client_socket CS_un;
	CS_un.sock_fd = *sock_fd_un;
	free(sock_fd_un);
	CS_un.family = UNIX;

	//handle local apps
	if (pthread_create(&thread_id_un, NULL, accept_clients, &CS_un) != 0){
		perror("pthread_create: ");
		exit(-1);
	}
/////////////////////////////INET
	int *sock_fd_in = NULL;
	pthread_join(thread_id_in, (void **) &sock_fd_in);

	client_socket CS_in;
	CS_in.sock_fd = *sock_fd_in;
	free(sock_fd_in);
	CS_in.family = INET;

	//handle remote clipboards
	if (pthread_create(&thread_id_in, NULL, accept_clients, &CS_in) != 0){
		perror("pthread_create: ");
		exit(-1);
	}
	
	//temporary, just to keep main alive
	//think about it do it nice
	while(1);
}