#ifndef DATALAYER_H
#define DATALAYER_H

#include "macros.h"

/**
 * @brief Open file
 *
 * @param  filename               file path to be transmmited
 * @param  identity               Receiver -> 1 | Sender -> 0
 * @return          file descriptor upon sucess, ERROR otherwise
 */
int llopen(char *filename, int identity);

/**
 * @brief Closes file connection
 * @param  fd               File descriptor of filename(llopen) file
 * @return
 */
int llclose(int fd);

/**
 * @brief Sends a packet with the data , the data can't extend the specified max length
 * @param  fd                   File descriptor of the RS-232 port
 * @param  buffer               Data to be written to serial port
 * @param  size                 Buffer size
 * @return        Data size upon sucess, ERROR otherwise
 */
int llwrite(int fd, char *buffer, int size);

/**
 * @brief Receives packet with the data, sent throw the serial port
 * @param  fd                   File descriptor of the RS-232 serial port
 * @param  buffer               Buffer with data read from the serial port
 * @return        Data size upon sucess, ERROR otherwise
 */
int llread(int fd, char *buffer);



#endif
