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


int getBytefromFd(int fd, char *byte_to_be_read);


int sendSupervisionFrame(int fd, char A_BYTE, char C_BYTE);

#endif
