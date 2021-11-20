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
    perror(filename);
    return ERROR;
  }

  if (tcgetattr(fd, &oldtiosreceiver) == -1){ /* save current port settings */
    perror("tcgetattr");
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
    perror("tcsetattr");
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
  if( DEBUG ) printf("Frame Sucessfully read from receiver\n");

  /* Send UA response to Sender */

  if( sendSupervisionFrame(fd, A_SR, C_UA) < 0 ) return ERROR;

  return SUCESS;
}



int main(int argc, char **argv)
{
  if ((argc < 2) ||
      ((strcmp("/dev/ttyS10", argv[1]) != 0) &&
       (strcmp("/dev/ttyS11", argv[1]) != 0)))
  {
    printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttyS11\n");
    exit(1);
  }

  printf("Running\n");

  openReceiver(argv[1]);


}
