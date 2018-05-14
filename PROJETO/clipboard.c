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

int connected_clipboard_init(char *IP, char *port_){
	int port = atoi(port_);
	
	//set te connection paremeters
	struct sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);
	inet_aton(IP, &server_addr.sin_addr);
	
	//create the endpoint to connect
	int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (sock_fd == -1){
		perror("socket: ");
		exit(-1);
	}

	//connect with clipboard "server"
	if(connect(sock_fd, (const struct sockaddr *) &server_addr, sizeof(server_addr))<0){
	printf("Error connecting!!!\n");
	exit(-1);
	}

	
	regions_init(sock_fd);
 return sock_fd;
}

 
int main(int argc, char **argv){		
	int server_sock_fd;
	int *clients_sock_fd = NULL;
	client_socket CS_un;
	client_socket CS_in;

//INITIALIZE CLIPBOARDS
	//connected mode init
	if (argc == 4){
		if(strcmp(argv[1], "-c") == 0){
			//connect with clipboard "server"
			server_sock_fd = connected_clipboard_init(argv[2], argv[3]);
			//upload regions from clients_sock_fdclients_sock_fdthe clipboard "server"
			regions_init(server_sock_fd);
			//create threads to update the clipboard "server" regions
			ppppppppppppppppppppppppppppppppppppp
		}
		else{
		printf("invalid mode\n");
		exit(-2);
		}
	}
	//single mode init
	else if (argc == 1)
		regions_init(-1);
	else{
		printf("invalid number of arguments\n");
		exit(-2);
	}

//LAUNCH OTHER CLIPBOARD SERVERS AND APP SERVERS
	//launch the server to handle local(unix) apps connections
	pthread_t thread_id_un;
	if (pthread_create(&thread_id_un, NULL, server_init, (void *) UNIX) != 0){
		perror("pthread_create: ");
		exit(-1);
	}
	//launch the server to handle remote(inet) clipboards
	pthread_t thread_id_in;
	if (pthread_create(&thread_id_in, NULL, server_init, (void *) INET) != 0){
		perror("pthread_create: ");
		exit(-1);
	}

//HANDLE LOCAL APPS AND REMOTE COMMUNICATIONS
	//set parameters to handle local apps
	pthread_join(thread_id_un, (void **) &clients_sock_fd);
	CS_un.sock_fd = *clients_sock_fd;
	free (clients_sock_fd);
	CS_un.param = UNIX;
	//handle local apps (one thread per app)
	if (pthread_create(&thread_id_un, NULL, accept_clients, &CS_un) != 0){
		perror("pthread_create: ");
		exit(-1);
	}
	//set parameters to handle remote clipboards
	pthread_join(thread_id_in, (void **) &clients_sock_fd);
	free (clients_sock_fd);
	CS_in.sock_fd = *clients_sock_fd;
	CS_in.param = INET;
	//handle remote clipboards (one thread per clipboard)
	if (pthread_create(&thread_id_in, NULL, accept_clients, &CS_in) != 0){
		perror("pthread_create: ");
		exit(-1);
	}

	//temporary, just to keep main alive
	//think about it do it nice
	while(1);
}