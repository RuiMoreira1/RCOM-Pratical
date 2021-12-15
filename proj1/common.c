#include "common.h"

int isRejected;

int checkSupervisionFrame(MACHINE_STATE *state, int fd, char A_BYTE, char C_BYTE, char* reject){
  char frame_byte;

  if( getBytefromFd(fd, &frame_byte) == ERROR ) return ERROR;

  switch(*state){
    case START_:
      if (DEBUG == 1) fprintf(stdout,"Entered in START_ | ");
      if( frame_byte == FLAG ) *state = FLAG_RCV;
      break;
    case FLAG_RCV:
      isRejected = 0;
      if (DEBUG == 1) fprintf(stdout,"Entered in FLAG_RCV | ");
      if( frame_byte == FLAG ) return SUCCESS;
      else if( frame_byte == A_BYTE ) *state = A_RCV;
      else *state = START_;
      break;
    case A_RCV:
      if (DEBUG == 1) fprintf(stdout,"Entered in A_RCV | ");
      if( reject != NULL && frame_byte == *reject ) {
        isRejected = 1;
      }
      if( frame_byte == FLAG ) *state = FLAG_RCV;
      else if( frame_byte == C_BYTE || isRejected ) *state = C_RCV;
      else *state = START_;
      break;
    case C_RCV:
      if (DEBUG == 1) fprintf(stdout,"Entered in C_RCV | ");
      if( frame_byte == FLAG ) *state = FLAG_RCV;
      else if( frame_byte == BCC(A_BYTE,C_BYTE)  ) *state = BCC_OK;
      else *state = START_;
      break;
    case BCC_OK:
      if (DEBUG == 1) fprintf(stdout,"Entered in BCC_OK\n");
      if( frame_byte == FLAG ) *state = STOP_;
      else *state = START_;
      break;
    default:
      if (DEBUG == 1) fprintf(stdout,"Default statement reached\n");
      break;
    }
    if( *state == STOP_ && isRejected ){
      *state = START_;
      return 1;
    }
    return SUCCESS;
}


int getBytefromFd(int fd, char *byte_to_be_read){
  int res = read(fd, byte_to_be_read, 1);
  if( res == ERROR){
    fprintf(stderr,"Error reading byte from fd\n");
    return ERROR;
  }
  if(DEBUG) fprintf(stdout,"Byte read: %02x , ", *byte_to_be_read);
  return SUCCESS;
}


int sendSupervisionFrame(int fd, char A_BYTE, char C_BYTE){
  char frame[5] = {FLAG, A_BYTE, C_BYTE, BCC(A_BYTE,C_BYTE), FLAG};

  int res = write(fd, frame, 5);

  if( res == ERROR ){
    fprintf(stderr,"Error writing to serial Port\n");
    return ERROR;
  }
  return SUCCESS;
}

int createBCC2(char *buffer, int bufferSize, char *bcc2){
  if(sizeof(buffer) > 0){
    char BCC2 = buffer[0];

    for(int i = 1; i < bufferSize; i++){
      BCC2 ^= buffer[i];
    }

    *bcc2 = BCC2;
    return SUCCESS;
  }
  else{
    fprintf(stderr,"Unable to create BCC2, buffer not allocated correctly\n");
    return ERROR;
  }
}

void insertError(char *data, int size, int probability){
  int r = rand() % 100;

  if( r < probability ) data[size] += 2;  
}
