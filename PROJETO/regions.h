#ifndef REGIONS 
#define REGIONS 

#include "utils.h"

/**
 * clipboard regions structure
 */
typedef struct REG{
	size_t size;
	void *message;
}REG;


/**
 * @brief      initializes the local clipboard regions
 *
 * @param[in]  fd    file descriptor for the remote 
 * 					 clipboard to pull the regions from 
 * 					 (fd < 0  indicates single mode)
 */
void regions_init_local(int fd);
/**
 * @brief      initializes a new connected clipboard regions
 *
 * @param[in]  fd    file descriptor for the new connected
 * 					 clipboard to push the regions to
 */
void regions_init_client(int fd);
/**
 * @brief      updates a clipboard region
 *
 * @param      head       passed by reference pointer to the 
 * 						  list of clipboard "clients"
 * @param[in]  fd         file descriptor to get the message from
 * @param[in]  data       structure with mesaage info
 * @param[in]  data_size  size of data (bytes)
 */
void update_region( down_list **head, int fd, Smessage data, int data_size);
/**
 * @brief      Sends a received messsage to the clipboard "server"
 *
 * @param[in]  fd         endpoint for the clipboard "server"
 * @param[in]  data       structure with the message info
 * @param[in]  data_size  size of data (bytes)
 */
void send_up_region(int fd, Smessage data, int data_size); 
/**
 * @brief      Sends a clipboard region message
 *
 * @param[in]  fd         file descriptor to send the message
 * @param[in]  data       struct with the message info
 * @param[in]  data_size  message size in bytes
 */
void send_region(int fd, Smessage data, int data_size, int order); 

#endif 