#include "receiver.h"

int r = 1;

struct termios oldtiosreceiver;

int openReceiver(char filename[]){
  int fd;

  struct termios newtio;

  fd = open(filename, O_RDWR | O_NOCTTY);
  if (fd < 0){
    fprintf(stderr,"%s\n",filename);
    return ERROR;
  }

  if (tcgetattr(fd, &oldtiosreceiver) == -1){ /* save current port settings */
    fprintf(stderr,"tcgetattr\n");
    return ERROR;
  }

  bzero(&newtio, sizeof(newtio));
  newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
  newtio.c_iflag = IGNPAR;
  newtio.c_oflag = 0;

  /* set input mode (non-canonical, no echo,...) */
  newtio.c_lflag = 0;

  newtio.c_cc[VTIME] = 0; /* inter-character timer unused */
  newtio.c_cc[VMIN] = 1;  /* reading 1 byte at time */

  /*
    VTIME e VMIN devem ser alterados de forma a proteger com um temporizador a
    leitura do(s) pr�ximo(s) caracter(es)
  */

  tcflush(fd, TCIOFLUSH);

  if (tcsetattr(fd, TCSANOW, &newtio) == -1){
    fprintf(stderr,"tcsetattr\n");
    return ERROR;
  }

  if( receiveSetFrame(fd) < 0 ) return ERROR;

  return fd;
}


int receiveSetFrame( int fd ){
  MACHINE_STATE receiverState = START_;

  while( receiverState != STOP_){
    if( checkSupervisionFrame(&receiverState, fd, A_SR, C_SET, NULL) < 0 ) return ERROR;  /*Check if set frame was read correctly*/

  }
  if( DEBUG ) fprintf(stdout,"Frame Sucessfully read from receiver\n");

  /* Send UA response to Sender */

  if( sendSupervisionFrame(fd, A_SR, C_UA) < 0 ) return ERROR;

  return SUCCESS;
}

int closeReceiver(int fd){
  if( receiverDisc(fd) == ERROR ) return ERROR;

  sleep(1); // Avoid changing config before sending data (transmission error)

  tcsetattr(fd,TCSANOW,&oldtiosreceiver);
  close(fd);
  return SUCCESS;
}


int receiverDisc(int fd){
  MACHINE_STATE receiverState = START_;

  if(DEBUG) fprintf(stdout,"Receiving DISC\n");
  while( receiverState != STOP_ ) {
    if( checkSupervisionFrame(&receiverState, fd, A_SR, C_DISC, NULL) == ERROR) return ERROR;
  }

  if( sendSupervisionFrame(fd, A_SR, C_DISC) == ERROR) return ERROR;

  if(DEBUG) fprintf(stdout,"Receiving UA\n");
  receiverState = START_;
  while( receiverState != STOP_ ) {
    if( checkSupervisionFrame(&receiverState, fd, A_SR, C_UA, NULL) == ERROR) return ERROR;
  }

  return SUCCESS;
}


int dataDeStuffing(char *stuffedBuffer, int stuffedBufferSize, char *buffer, char *BCC2){
  char destuffedBuffer[MAX_DATA_SIZE+1]; /* +1 because of trailing '\0' char */

  int temp = 0; /* Auxiliary int */

  for( int i = 0; i < stuffedBufferSize; i++){
    if( stuffedBuffer[i] == ESCAPE ){
      char nextByte = stuffedBuffer[++i];
      if( nextByte == FLAG_ESCAPE_XOR ) destuffedBuffer[temp++] = FLAG;
      else if( nextByte == ESCAPE_XOR ) destuffedBuffer[temp++] = ESCAPE;
      else {
        fprintf(stderr,"Escape byte violation\n");
        return ERROR;
      }
    }
    else destuffedBuffer[temp++] = stuffedBuffer[i];
  }


  *BCC2 = destuffedBuffer[--temp];

  for(int j = 0; j < temp; j++ ) buffer[j] = destuffedBuffer[j];

  return temp;
}


int receivedStuffedDataSM(MACHINE_STATE *state, int fd, char *stuffedBuffer, char *buffer){

  u_int8_t receivedAddress, receivedControl, calculatedBCC,
          ctrl = C_FRAME_I(1-r), repeatedCtrl = C_FRAME_I(r), calculatedBCC2, bcc2;

  int currentDataIdx, isRepeated, dataSize;
  u_int8_t stuffed_data[STUFF_DATA_MAX];

  while (*state != STOP_) {
    u_int8_t byte;

    if( getBytefromFd(fd, &byte) == ERROR );

    switch (*state) {
      case START_:
        if (DEBUG == 1) fprintf(stdout,"Entered in START_ | ");
        if (byte == FLAG) *state = FLAG_RCV;
        break;

      case FLAG_RCV:
        if (DEBUG == 1) fprintf(stdout,"Entered in FLAG_RCV | ");
        if (byte == FLAG) continue;
        else if (byte == A_SR) {
          isRepeated = 0;
          receivedAddress = byte;
          *state = A_RCV;
        }
        else *state = START_;
        break;

      case A_RCV:
        if (DEBUG == 1) fprintf(stdout,"Entered in A_RCV | ");
        if (byte == repeatedCtrl) isRepeated = 1;
        if (byte == FLAG) *state = FLAG_RCV;
        else if (byte == ctrl || isRepeated) {
          receivedControl = byte;
          calculatedBCC = receivedAddress ^ receivedControl;
          *state = C_RCV;
        }
        else *state = START_;
        break;

      case C_RCV:
        if (DEBUG == 1) fprintf(stdout,"Entered in C_RCV | ");
        if (byte == FLAG) *state = FLAG_RCV;
        else if (calculatedBCC == byte) {
          *state = BCC_OK;
          currentDataIdx = 0;
        }
        else *state = START_;
        break;

      case BCC_OK:
        if (DEBUG == 1) fprintf(stdout,"Entered in BCC_OK\n");
        if (currentDataIdx >= STUFF_DATA_MAX) *state = START_;
        else if (byte == FLAG) {
          dataSize = dataDeStuffing(stuffed_data, currentDataIdx, buffer, &bcc2);
          if( createBCC2(buffer, dataSize, &calculatedBCC2) == ERROR ){
            fprintf(stderr,"Error generating BCC2, data field problem\n");
		        return ERROR;
          }
          /*Slide 15*/
          if (isRepeated) {
            if (sendSupervisionFrame(fd, A_SR, C_RR(1 - r)) == ERROR) {
              fprintf(stderr, "Error sending supervision frame repeting frame\n");
              return -1;
            }
            *state = START_;
          }
          else if (calculatedBCC2 != bcc2) {
            if (sendSupervisionFrame(fd, A_SR, C_REJ(1 - r)) == ERROR) {
              fprintf(stderr, "Error sending supervision frame rejecting frame\n");
              return ERROR;
            }
            *state = START_;
          }
          else *state = STOP_;
        }
        else stuffed_data[currentDataIdx++] = byte;
        break;

      default:
        break;
    }
  }

  return dataSize;
}


int receivedStuffedData(int fd, char *buffer){
  MACHINE_STATE state = START_;

  char stuffedBuffer[STUFF_DATA_MAX];

  int size = receivedStuffedDataSM(&state, fd, stuffedBuffer, buffer);
  if( size == ERROR ){
    fprintf(stderr, "Error verifying stuffed buffer integrity\n");
    return ERROR;
  }

  if( sendSupervisionFrame(fd, A_SR, C_RR(r)) == ERROR ){
    fprintf(stderr, "Error sending supervision frame for all data received well\n");
    return ERROR;
  }

  r = 1 - r;

  return size;
}
