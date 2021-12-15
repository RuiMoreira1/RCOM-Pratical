#ifndef RECEIVER_H
#define RECEIVER_H

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include "common.h"
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define BAUDRATE B38400
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1

/**
 * @brief Opens the receptor for reading and sending data
 *
 * @param  filename           serial port file
 * @return        File descriptor of the serial port upon sucess, ERROR otherwise
 */
int openReceiver(char filename[]);

/**
 * @brief Acknowledge SET frame, transmitted by the sender, validate, and then
 * send the UA response (slide 7 && 14)
 *
 * @param  fd                  serial port file descriptor
 * @return    SUCCESS upon success, ERROR otherwise
 */
int receiveSetFrame(int fd);

/**
 * @brief Closes the Receiver side of the serial port (slide 14)
 *
 * @param  fd                  serial port file descriptor
 * @return    SUCCESS upon success, ERROR otherwise
 */
int closeReceiver(int fd);

/**
 * @brief Receiver acknowledges DISC frame, validates, resend the DISC frame,
 * and if the DISC frame is validated by the other side of the serial port,
 * acknowledges a UA frame, and consequently validates the frame (slide 14)
 *
 * @param  fd                  serial port file descriptor
 * @return    SUCCESS upon success, ERROR otherwise
 */
int receiverDisc(int fd);

/**
 * @brief Data Destuffing
 * @param  stuffedBuffer        stuffed buffer received from sender
 * @param  stuffedBufferSize    size of the stuffed buffer
 * @param  buffer               buffer after destuffing
 * @param  BCC2                 data BCC2
 * @return     destuffed buffer upon sucess, ERROR otherwise
 */
int dataDeStuffing(char *stuffedBuffer, int stuffedBufferSize, char *buffer, char *BCC2);

/**
 * @brief Receives stuffed data from sender
 * @param  fd                    file descriptor
 * @param  stuffedData           buffer containing stuffed data
 * @return            buffer size upon success, ERROR otherwise
 */
int receivedStuffedData(int fd, char *buffer);

/**
 * @brief State machine for receiving stuffed data frame
 * @param  state                 state machine state
 * @param  fd                    serial port file descriptor
 * @param  stuffedBuffer         stuffed buffer received from sender
 * @param  buffer                destuffed buffer
 * @return               buffer size upon success, ERROR otherwise
 */
int receivedStuffedDataSM(MACHINE_STATE *state, int fd, char *stuffedBuffer, char *buffer);



#endif
