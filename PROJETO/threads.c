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

void * thread_code(void * fd_){



}




void thread_accept(int sock_fd, struct sockaddr_un client_addr, socklen_t size_addr){

int client_fd = accept(sock_fd, (struct sockaddr *) &client_addr, &size_addr);
		if (client_fd == -1){
		perror("accept: ");
		exit (-1);
		}

pthread_t * thread_id;
thread_id=(pthread_t*)malloc(sizeof(pthread_t));
pthread_create(thread_id, NULL, thread_code,(void* client_fd));

thread_accept(sock_fd, client_addr, size_addr);
}
