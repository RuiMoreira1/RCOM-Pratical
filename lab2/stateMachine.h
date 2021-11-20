#ifndef STATEMACHINE_H
#define STATEMACHINE_H

#include "macros.h"

typedef enum { START_, FLAG_RCV, A_RCV, C_RCV, BCC_OK, STOP_ } MACHINE_STATE

/*
 @brief State Machine for both the sender and the receiver
 */
void updateStateMachine(MACHINE_STATE state, int id, char flag);

#endif
