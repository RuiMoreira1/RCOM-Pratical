/**
 * Non-Canonical Input Processing
 * From https://tldp.org/HOWTO/Serial-Programming-HOWTO/x115.html by Gary Frerking and Peter Baumann
**/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include "receiver.h"

#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define BAUDRATE B38400
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1

volatile int STOP_RUNNING = FALSE;

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
  char byte;
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

  int isRepeated, bufferSize, index;

  char frame_byte, receivedAdd, repeatedCtrl = C_FRAME_I(r), ctrl = C_FRAME_I(1-r), receivedCtrl, calcBCC, bcc2, calcBCC2;


  while(*state != STOP_){
    if(getBytefromFd(fd, &frame_byte) == ERROR){
      return ERROR;
    }

    switch (*state) {
      case START_:
        if (DEBUG == 1) fprintf(stdout,"Entered in START_\n");
        if( frame_byte == FLAG ) *state = FLAG_RCV;
        else *state = START_;
        break;
      case FLAG_RCV:
        if (DEBUG == 1) fprintf(stdout,"Entered in FLAG_RCV\n");
        if( frame_byte == FLAG ) continue;
        else if( frame_byte == A_SR ) {
          *state = A_RCV;
          isRepeated = 0;
          receivedAdd = frame_byte;
        }
        else *state = START_;
        break;
      case A_RCV:
        if (DEBUG == 1) fprintf(stdout,"Entered in A_RCV\n");
        if( frame_byte == repeatedCtrl ) isRepeated = 1;
        if( frame_byte == FLAG ) *state = FLAG_RCV;
        else if( frame_byte == ctrl || isRepeated ) {
          *state = C_RCV;
          receivedCtrl = frame_byte;
          calcBCC = BCC(receivedCtrl,receivedAdd);
        }
        else *state = START_;
        break;
      case C_RCV:
        if (DEBUG == 1) fprintf(stdout,"Entered in C_RCV\n");
        if( frame_byte == FLAG ) *state = FLAG_RCV;
        else if( frame_byte == calcBCC ) {
          *state = BCC_OK;
          index = 0;
        }
        else *state = START_;
        break;
      case BCC_OK:
        if (DEBUG == 1) fprintf(stdout,"Entered in BCC_OK\n");
        if( index >= STUFF_DATA_MAX ) *state = START_;
        else if( frame_byte == FLAG ) {
          bufferSize = dataDeStuffing(stuffedBuffer, index, buffer, &bcc2);
          calcBCC2 = createBCC2(buffer, bufferSize);
          /*Slide 15*/
          if( isRepeated ){
            if( sendSupervisionFrame(fd, A_SR, C_RR(1-r)) == ERROR ){
              fprintf(stderr, "Error sending supervision frame repeting frame\n");
              *state = START_;
              return ERROR;
            }
            *state = START_;
          }
          else if( calcBCC2 != bcc2 ){
            if( sendSupervisionFrame(fd, A_SR, C_REJ(1-r)) == ERROR ){
              fprintf(stderr, "Error sending supervision frame rejecting frame\n");
              *state = START_;
              return ERROR;
            }
            *state = START_;
          }
          else *state = STOP_;
        }
        else stuffedBuffer[index++] = frame_byte;
        break;
      default:
        if (DEBUG == 1) fprintf(stdout,"Default statement reached\n");
        break;
      }
    }

    return bufferSize;
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


int main(int argc, char **argv)
{
  //freopen("output.txt","a+", stdout); /* STDOUT to file */
  if ((argc < 2) ||
      ((strcmp("/dev/ttyS10", argv[1]) != 0) &&
       (strcmp("/dev/ttyS11", argv[1]) != 0)))
  {
    fprintf(stdout,"Usage:\tnserial SerialPort\n\tex: nserial /dev/ttyS11\n");
    exit(1);
  }

  fprintf(stdout,"Running\n");

  int fd = openReceiver(argv[1]);

  char buffer[STUFF_DATA_MAX];
  int size = receivedStuffedData(fd, buffer );
  printf("Out\n");

  for(int i = 0; i < size; i++) fprintf(stdout,"Byte -> %02x\n",buffer[i]);
  /*char stuff[9] = {0x7d,0x5d, 0x01,0x02,0x03, 0x04, 0x05, 0x7d, 0x5e};
  char buff[7];
  char bccc[1];
  int size = dataDeStuffing(stuff,9,buff,bccc);
  fprintf(stdout,"Leaving data stuffing\n");

  for(int i = 0; i < size; i++) fprintf(stdout,"Byte -> %02x\n",buff[i]);
  fprintf(stdout,"BCC2 -> %02x\n", bccc[0]);*/


  fprintf(stdout,"Sleeping\n");

  closeReceiver(fd);


}
