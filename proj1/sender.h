#ifndef SENDER_H
#define SENDER_H

#include <signal.h>
#include "macros.h"

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
 * @return Serial port fd upon sucess, ERROR otherwise
 */
int openSender(char filename[]);

/**
 * @brief Sending SET trame message, to setup the connection between Sender && Receiver,
 * expected to receive Unknow Acknowledgement from the Receiver
 *
 * @param  fd               [description]
 * @return  SUCESS upon sucess, ERROR otherwise
 */
int sendSetFrame(int fd);


int closeSender(int fd);

int senderDisc(int fd);

#endif
