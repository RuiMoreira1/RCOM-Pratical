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
void updateStateMachine(MACHINE_STATE *state, int id, char flag);

#endif
