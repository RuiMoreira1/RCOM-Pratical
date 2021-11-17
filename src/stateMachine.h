#ifndef STATEMACHINE_H
#define STATEMACHINE_H


#include "macros.h"


enum stateMachine {START, FLAG_RCV, A_RCV, C_RCV, BCC, STOP};

typedef struct {
  enum stateMachine currentState;
  char A_FIELD;
  char C_FIELD;

} stateMachine_st;

void updateStateMachine(stateMachine_st *cState, char *buf, int identity);


#endif
