#ifndef SENDER_H
#define SENDER_H

#include <signal.h>
#include "macros.h"
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
 * @brief Answer the alarm to if necessary resend the info from the sender,
 * upon not receiving any response from the receiver
 */
void answerAlarm();

/**
 * @brief Resetting alarm flags such as conta and flag to the original values,
 * if a message is correctly received by the sender
 */
void resetAlarmFlags();

/**
 * @brief Opens sender for sending data
 *
 * @param  filename             Serial port filename
 * @return Serial port fd upon success, ERROR otherwise
 */
int openSender(char filename[]);

/**
 * @brief Sending SET trame message, to setup the connection between Sender && Receiver, (slide 7 && 14)
 * expected to receive Unknow Acknowledgement from the Receiver
 *
 * @param  fd               serial port file descriptor
 * @return  SUCCESS upon success, ERROR otherwise
 */
int sendSetFrame(int fd);

/**
 * @brief Closes the Sender side of the serial port (slide 14)
 *
 * @param  fd               serial port file descriptor
 * @return    SUCCESS upon success, Error otherwise
 */
int closeSender(int fd);

/**
 * @brief Issue a DISC Frame, followed by a check of a DISC Frame sent by receiver side,
 *if the DISC frame of receiver side is valid, send a UA Frame to receiver (slide 14)
 *
 * @param  fd               serial port file descriptor
 * @return    SUCCESS upon success, ERROR otherwise
 */
int senderDisc(int fd);

/**
 * @brief Data stuffing mechanism, the stuffedBuffer is the result of the stuffint of buffer (slide 13)
 *
 * @param  buffer                      buffer before stuffing
 * @param  dataSize                    buffer size
 * @param  BCC2                        bcc2
 * @param  stuffedBuffer               buffer after being stuffed
 * @return        stuffedBuffer size upon success, ERROR otherwise
 */
int dataStuffing(char* buffer, int dataSize, char BCC2, char* stuffedBuffer);

/**
 * @brief Sends a Frame with the data stuffed to the other side of the serial Port (slide 14 && 7)
 *
 * @param  fd                       file descriptor of the serial port
 * @param  buffer                   data to be stuffed and then sent
 * @param  bufferSize               buffer size
 * @return            buffer size upon success, ERROR otherwise
 */
int sendStuffedFrame(int fd, char* buffer, int bufferSize);

#endif
