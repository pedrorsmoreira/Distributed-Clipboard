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


void *clipboard_accept(){

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