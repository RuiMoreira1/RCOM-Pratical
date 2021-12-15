#ifndef DATALAYER_H
#define DATALAYER_H

#include "sender.h"
#include "receiver.h"


/**
 * @brief Open serial port connection through a file descriptor, in a specific
 * side of the serial port, given by the id
 *
 * @param  filename               file path to be transmmited
 * @param  id                     Receiver -> 1 | Sender -> 0
 * @return          file descriptor upon sucess, ERROR otherwise
 */
int llopen(char *filename, int id);

/**
 * @brief Closes the serial port connection, in a specific
 * side of the serial port, given by the id
 *
 * @param  fd               File descriptor of filename(llopen) file
 * @param  id               Receiver -> 1 | Sender -> 0
 * @return          SUCCESS upon success, ERROR otherwise
 */
int llclose(int fd, int id);

/**
 * @brief Sends a packet with the data , the data can't extend the specified max length
 *
 * @param  fd                   File descriptor of the RS-232 port
 * @param  buffer               Data to be written to serial port
 * @param  size                 Buffer size
 * @param  id                   Receiver -> 1 | Sender -> 0
 * @return        Data size upon sucess, ERROR otherwise
 */
int llwrite(int fd, char *buffer, int size, int id);

/**
 * @brief Receives packet with the data, sent throw the serial port
 *
 * @param  fd                   File descriptor of the RS-232 serial port
 * @param  buffer               Buffer with data read from the serial port
 * @param  id                   Receiver -> 1 | Sender -> 0
 * @return        Data size upon sucess, ERROR otherwise
 */
int llread(int fd, char *buffer, int id);



#endif
