#include "stateMachine.h"

void updateStateMachine(MACHINE_STATE state, int id, char flag){
  switch(state){
    case START_:
      if( flag == FLAG ) state = FLAG_RCV;
      else state = START_;
      break;
    case FLAG_RCV:
      if( flag == FLAG) state = FLAG_RCV;
      else if( flag == A_SR ) state = A_RCV;
      else state = START_;
      break;
    case A_RCV:
      if( flag == FLAG ) state = FLAG_RCV;
      else if( (flag == C_SET && id == RECEIVERID) || (flag = C_UA && id == SENDERID) ) state = C_RCV;
      else state = START_;
      break;
    case C_RCV:
      if( flag == FLAG ) state = FLAG_RCV;
      else if( (flag == BCC(A_SR,C_SET) && id == RECEIVERID) || (flag == BCC(A_SR, C_UA) && id == SENDERID) ) state = BCC_OK;
      else state = START_;
      break;
    case BCC_OK:
      if( flag == FLAG ) state = STOP_;
      else state = START_;
      break;
    default:
      printf("Default statement reached\n");
      break;
    }
}
