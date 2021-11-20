#include "common.h"


int checkSupervisionFrame(MACHINE_STATE *state, int fd, char A_BYTE, char C_BYTE, char* reject){
  int isRejected; char frame_byte;

  if( getBytefromFd(fd, &frame_byte) < 0 ) return ERROR;

  switch(*state){
    case START_:
      if (DEBUG == 1) printf("Entered in START_\n");
      if( frame_byte == FLAG ) *state = FLAG_RCV;
      else *state = START_;
      break;
    case FLAG_RCV:
      isRejected = 0;
      if (DEBUG == 1) printf("Entered in FLAG_RCV\n");
      if( frame_byte == FLAG) *state = FLAG_RCV;
      else if( frame_byte == A_BYTE ) *state = A_RCV;
      else state = START_;
      break;
    case A_RCV:
      if( reject != NULL && frame_byte == *reject ) isRejected = 1;
      if (DEBUG == 1) printf("Entered in A_RCV\n");
      if( frame_byte == FLAG ) *state = FLAG_RCV;
      else if( frame_byte == C_BYTE || isRejected ) *state = C_RCV;
      else *state = START_;
      break;
    case C_RCV:
      if (DEBUG == 1) printf("Entered in C_RCV\n");
      if( frame_byte == FLAG ) *state = FLAG_RCV;
      else if( frame_byte == BCC(A_BYTE,C_BYTE)  ) *state = BCC_OK;
      else *state = START_;
      break;
    case BCC_OK:
      if (DEBUG == 1) printf("Entered in BCC_OK\n");
      if( frame_byte == FLAG ) *state = STOP_;
      else *state = START_;
      break;
    default:
      if (DEBUG == 1) printf("Default statement reached\n");
      break;
    }
    if( *state == STOP_ && isRejected ) return 1;
    return 0;
}


int getBytefromFd(int fd, char *byte_to_be_read){
  int res = read(fd, byte_to_be_read, 1);
  if( res == ERROR){
    perror("Error reading byte from fd\n");
    return ERROR;
  }
  if(DEBUG) printf("Byte read: %02x\n", *byte_to_be_read);
  return SUCESS;
}


int sendSupervisionFrame(int fd, char A_BYTE, char C_BYTE){
  char frame[5] = {FLAG, A_BYTE, C_BYTE, BCC(A_BYTE,C_BYTE), FLAG};

  int res = write(fd, frame, 5);

  if( res == ERROR ){
    perror("Error writing to serial Port\n");
    return ERROR;
  }
  return SUCESS;
}
