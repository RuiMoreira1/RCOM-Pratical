#include "stateMachine.h"


void updateStateMachine(stateMachine_st *cState, char *output, int identity){
  switch(cState->currentState){
    case START:
      if(output[0] == FLAG){ cState->currentState = FLAG_RCV; }
      break;
    case FLAG_RCV:
      if(output[0] == A_CE){ cState->currentState = A_RCV; }
      else if( output[0] != FLAG) { cState->currentState = START; }
      break;
    case A_RCV:
      if( output[0] == FLAG){ cState->currentState = FLAG_RCV; }
      if( ( identity == RECEIVER && output[0] == C_SET) || (identity == SENDER && output[0] == C_UA) ){
        cState->currentState = C_RCV;
        cState->C_FIELD = output[0];
      }
      else if( output[0] != FLAG){ cState->currentState = START;}
      break;
    case C_RCV:
      if(output[0] == (cState->A_FIELD ^ cState->C_FIELD)){ cState->currentState == BCC; }
      else if( output[0] == FLAG){ cState->currentState = FLAG_RCV; }
      else if( output[0] != FLAG){ cState->currentState = START; }
      break;
    case BCC:
      if(output[0] == FLAG) { cState->currentState = STOP; }
      break;
    default:
      break;
    }
}
