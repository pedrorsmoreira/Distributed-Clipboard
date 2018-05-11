

#include <sys/types.h>

typedef struct REG{
	size_t size;
	void *message;
}REG;

typedef struct client_socket{
	int sock_fd;
	struct sockaddr_un addr;
	socklen_t size;
}client_socket;

void regions_init();
void * app_thread(void * CS);
void app_handle(int client_fd);