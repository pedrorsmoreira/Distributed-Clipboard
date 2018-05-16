#define UNIX 0
#define INET 1
#define DOWN 0
#define UP 1
#define WRITE 0
#define LIST 1

#include <sys/types.h>

#ifndef THREADS
#define THREADS

typedef struct client_socket{
	int sock_fd;
	int family;
}client_socket;

int rand_port_gen();
void *server_init(void * family);
void *accept_clients(void * CS);
void connection_handle(int fd, int reference)

#endif