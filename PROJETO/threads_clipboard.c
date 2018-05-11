#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <pthread.h>
#include <sys/un.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


void *clipboard_init(){

	struct sockaddr_un local_addr;

	int *sock_fd = (int *) malloc(sizeof(int));

	//create a socket stream for internet comunication
	*sock_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (*sock_fd == -1){
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

	 return (void *) sock_fd;
}

void *clipboard_accept(void * CS){
	client_socket *CS_ = (client_socket *) CS;
	
	//stablish connection with the app
	int client_fd = accept( CS_->sock_fd, (struct sockaddr *) &(CS_->addr), &(CS_->size));
	if (client_fd == -1){
		perror("accept: ");
		exit (-1);
	}
	
	//create new thread for next app connection
	pthread_t thread_id;
	if (pthread_create(&thread_id, NULL, clipboard_accept, CS) != 0){
		perror("pthread_create: ");
		exit(-1);
	}
	
	//answer current app requests
	app_handle(client_fd);
	//HAD TO PUT SOMETHING ON RETURN - MEANINGLESS - CHANGE IT AFTERWARDS
	return CS;
}




