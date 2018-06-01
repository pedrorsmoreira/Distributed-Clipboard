//protect multiple inclusions
#ifndef THREADS
#define THREADS

#define UNIX 0
#define INET 1
#define DOWN 0
#define UP 1

/**
 * struct with info to accpet clients via socket-stream
 */
typedef struct client_socket{
	int sock_fd;
	int family;
}client_socket;

/**
 * @brief      Initializes a socket-stream server
 *
 * @param[in]  family	Stream type (UNIX or INET)
 *
 * @return     struct with info to accpet the clients
 */
void *server_init(void * family);

/**
 * @brief      Recursive function - accepts a socket stream 
 *             connection every function call and launches 
 *             the function to process the new connection
 *
 * @param[in]  CS_   pointer to a struct with info 
 * 					 to accpet the clients
 * 
 * @return     useless
 */
void *accept_clients(void * CS);

/**
 * @brief      handles client requests until it closes the connection
 *
 * @param[in]  fd         connection file descriptor
 * @param[in]  reference  server (UP) or client(DOWN)
 */
void connection_handle(int fd, int reference);

#endif