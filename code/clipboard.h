/******************************************************************************
 *
 * File Name: clipboard.h
 *	      (c) 2018 PSis
 * Authors: Pedro Moreira e Inês Gaspar
 *
 * DESCRIPTION
 *		main program starts all the routines 
 *		and launches the local clipboard functionalities 
 *****************************************************************************/

//protect multiple inclusions
#ifndef CLIPBOARD
#define CLIPBOARD

#include "clipboard_imp.h"

/**
 * @brief      creates the connection with the local clipboard
 *
 * @param[in]  clipboard_dir  directory of the local clipboard
 *
 * @return     returns the clipboard identifier to interact
 *             with the clipboard, or -1 in case of error
 */
int clipboard_connect(char * clipboard_dir);

/**
 * @brief      stores data in a certain region of the clipboard
 *
 * @param[in]  clipboard_id  Clipboard identifier - file descriptor
 * @param[in]  region        Clipboard region to store the data
 * @param[in] buf	         Buffer which the data is pointed by
 * @param[in]  count         Number of bytes of data to be copied
 *
 * @return     returns the number of bytes copied
 *             or returns 0 in case of error
 */
int clipboard_copy(int clipboard_id, int region, void *buf, size_t count);

/**
 * @brief      Gets data from a certain region of the clipboard
 *
 * @param[in]  clipboard_id  clipboard identifier - file descriptor
 * @param[in]  region        Clipboard region to copy the data
 * @param[out] buf           Buffer to store the message
 * @param[in]  count         Max number of bytes to be stored in the buffer
 * 							 (if count < 0 no limit specified and alloc buffer)
 *
 * @return     returns the number of bytes stored in the buffer
 *             or returns 0 in case of error
 */
int clipboard_paste(int clipboard_id, int region, void *buf, size_t count);

/**
 * @brief      Waits on a certain clipboard region to change
 *             and sends the new data
 *
 * @param[in]  clipboard_id  clipboard identifier - file descriptor
 * @param[in]  region        Clipboard region to copy the data
 * @param[in]  buf           Buffer to store the message
 * @param[in]  count         Max number of bytes to be stored in the buffer
 *
 * @return     returns the number of bytes stored in the buffer
 *             or returns 0 in case of error
 */
int clipboard_wait(int clipboard_id, int region, void *buf, size_t count);

/**
 * @brief      closes the connection with the local clipboard
 *
 * @param[in]  clipboard_id  clipboard identifier - file descriptor
 */
void clipboard_close(int clipboard_id);

#endif