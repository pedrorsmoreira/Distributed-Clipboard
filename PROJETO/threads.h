#define UNIX 0
#define INET 1

#include <sys/types.h>

typedef struct client_socket{
	int sock_fd;
	int family;
}client_socket;

void *server_init(void * family);
void *accept_clients(void * CS);
void client_handle(int client_fd);