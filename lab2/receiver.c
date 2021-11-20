/**
 * Non-Canonical Input Processing
 * From https://tldp.org/HOWTO/Serial-Programming-HOWTO/x115.html by Gary Frerking and Peter Baumann
**/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include "stateMachine.h"

#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define BAUDRATE B38400
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1

void updateState(MACHINE_STATE *state, int id, char flag){
  switch(*state){
    case START_:
      printf("Entered in START_\n");
      if( flag == FLAG ) *state = FLAG_RCV;
      else *state = START_;
      break;
    case FLAG_RCV:
      printf("Entered in FLAG_RCV\n");
      if( flag == FLAG) *state = FLAG_RCV;
      else if( flag == A_SR ) *state = A_RCV;
      else *state = START_;
      break;
    case A_RCV:
      printf("Entered in A_RCV\n");
      if( flag == FLAG ) *state = FLAG_RCV;
      else if( (flag == C_SET && id == RECEIVERID) || (flag = C_UA && id == SENDERID) ) *state = C_RCV;
      else *state = START_;
      break;
    case C_RCV:
      printf("Entered in C_RCV\n");
      if( flag == FLAG ) *state = FLAG_RCV;
      else if( (flag == BCC(A_SR,C_SET) && id == RECEIVERID) || (flag == BCC(A_SR, C_UA) && id == SENDERID) ) *state = BCC_OK;
      else *state = START_;
      break;
    case BCC_OK:
      printf("Entered in BCC_OK\n");
      if( flag == FLAG ) *state = STOP_;
      else *state = START_;
      break;
    default:
      printf("Default statement reached\n");
      break;
    }
}

volatile int STOP = FALSE;

int main(int argc, char **argv)
{
  int fd, res;
  struct termios oldtio, newtio;
  char buf[255];

  if ((argc < 2) ||
      ((strcmp("/dev/ttyS10", argv[1]) != 0) &&
       (strcmp("/dev/ttyS11", argv[1]) != 0)))
  {
    printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttyS11\n");
    exit(1);
  }

  /*
    Open serial port device for reading and writing and not as controlling tty
    because we don't want to get killed if linenoise sends CTRL-C.
  */

  fd = open(argv[1], O_RDWR | O_NOCTTY);
  if (fd < 0)
  {
    perror(argv[1]);
    exit(-1);
  }

  if (tcgetattr(fd, &oldtio) == -1)
  { /* save current port settings */
    perror("tcgetattr");
    exit(-1);
  }

  bzero(&newtio, sizeof(newtio));
  newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
  newtio.c_iflag = IGNPAR;
  newtio.c_oflag = 0;

  /* set input mode (non-canonical, no echo,...) */
  newtio.c_lflag = 0;

  newtio.c_cc[VTIME] = 0; /* inter-character timer unused */
  newtio.c_cc[VMIN] = 5;  /* blocking read until 5 chars received */

  /*
    VTIME e VMIN devem ser alterados de forma a proteger com um temporizador a
    leitura do(s) pr�ximo(s) caracter(es)
  */

  tcflush(fd, TCIOFLUSH);

  if (tcsetattr(fd, TCSANOW, &newtio) == -1)
  {
    perror("tcsetattr");
    exit(-1);
  }

  printf("New termios structure set\n");


  MACHINE_STATE receiver_state = START_;
  char c;

  while (STOP == FALSE){
    res = read(fd, &c, 1);

    if( res == -1 ){
      printf("Error reading from buffer\n");
      exit(1);
    }

    printf("Trame received (receptor)-> %02x\n", c);

    updateStateMachine(&receiver_state, RECEIVERID, c);

    if ( receiver_state == STOP_ ) {
      STOP = TRUE;
    }

  }


  printf("Sending the string to emissor\n");

  char receiver_trame[5] = {FLAG, A_SR, C_UA, BCC(A_SR,C_UA), FLAG};

  int res_w = write(fd,receiver_trame,5);
  if( res_w == -1 ){
    printf("Error writing to serial port\n");
    exit(1);
  }
  printf("Bytes writen to fd: %d\n",res_w);
  printf("Message sent successfully\n");



  sleep(1);

  tcsetattr(fd, TCSANOW, &oldtio);
  close(fd);
  return 0;
}
