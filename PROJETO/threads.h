#define UNIX 0
#define INET_RECV 1
#define INET_SEND 2
#define DOWN 0
#define UP 1

#include <sys/types.h>

#ifndef THREADS
#define THREADS

typedef struct client_socket{
	int sock_fd;
	int family;
	int port;
}client_socket;

void *server_init(void * family);
void *accept_clients(void * CS);
void connection_handle(int fd, int reference);

#endif