#ifndef THREADS
#define THREADS

#define UNIX 0
#define INET 1
#define DOWN 0
#define UP 1
#define MAX_PORT 63714

/**
 * struct with info to accpet clients via socket-stream
 */

typedef struct client_socket{
	int sock_fd;
	int family;
}client_socket;

void *server_init(void * family);
void *accept_clients(void * CS);
void connection_handle(int fd, int reference);

#endif