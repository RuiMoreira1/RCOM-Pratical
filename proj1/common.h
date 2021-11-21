#ifndef STATEMACHINE_H
#define STATEMACHINE_H

#include "macros.h"
#include <stdio.h>

typedef enum { START_, FLAG_RCV, A_RCV, C_RCV, BCC_OK, STOP_ } MACHINE_STATE;


/*
 @brief State Machine for both the sender and the receiver
 *
 * @param *state - MACHINE_STATE enum, to update the state machine state, when receiving trame information
 * @param id - Receiver state MACHINE id -> 0 Sender state Machine id -> 1
 * @param flag - Flag received from the file descriptor
*/
int checkSupervisionFrame(MACHINE_STATE *state, int fd, char A_BYTE, char C_BYTE, char* reject);

/**
 * @brief Read the serial port file descriptor byte by byte. Helper function in the
 * project state machine, to verify and validade answers from one side to the otherwise
 *
 * @param  fd                            serial port file descriptor
 * @param  byte_to_be_read               byte read from the serial port
 * @return                 SUCCESS upon success, ERROR otherwise
 */
int getBytefromFd(int fd, char *byte_to_be_read);

/**
 * @brief Send UA, DISC, SET Frames from one side to the other of the serial port
 * @param  fd                   serial port file descriptor
 * @param  A_BYTE               A Field of the UA/DISC/SET Frame
 * @param  C_BYTE               C Field of the UA/DISC/SET Frame
 * @return        SUCCESS upon success, ERROR otherwise
 */
int sendSupervisionFrame(int fd, char A_BYTE, char C_BYTE);

/**
 * @bried Create Information Trame BCC2, consisisting in a xor of all the data field
 * bits, therefore creating BCC2
 *
 * @param  buffer                   Information camp
 * @param  bufferSize               Information camp size
 * @return            BCC2 upon success, '\0' otherwise
 */
char createBCC2(char *buffer, int bufferSize);

#endif
