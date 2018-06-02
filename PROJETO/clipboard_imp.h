/******************************************************************************
 *
 * File Name: clipboard_imp.h
 *	      (c) 2018 PSis
 * Authors: Pedro Moreira e Inês Gaspar
 *
 * DESCRIPTION
 *		hides the message struct implementation from the user 
 *****************************************************************************/

//protect multiple inclusions
#ifndef CLIP_IMP
#define CLIP_IMP

#define REGIONS_NR 10
#define COPY 0
#define PASTE 1
#define WAIT 2
#define SOCK_ADDRESS "./CLIPBOARD_SOCKET"

#include <sys/types.h>

/**
 * COMMENT THIS LINE TO WRITE/READ MESSAGES WITHOUT 
 * A CYCLE TO ASSURE THE WHOLE MESSAGE IS PASSED
 */
#define MESSAGE_FLUX_CYCLE

/**
 * 	To send buffers with a big number of bytes,
 * 	only onde write/read might not be enough
 */
#ifdef MESSAGE_FLUX_CYCLE
size_t writeall(int fd, void *buf, size_t len);
size_t readall(int fd, void *buf, size_t len);
#endif


/**
 *  struct for messages needed info
 */
typedef struct Smessage{
	int region;
	size_t message_size;
	int order;
}Smessage;

#endif