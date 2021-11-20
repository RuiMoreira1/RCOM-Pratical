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


  MACHINE_STATE state = START_;
  char c;
  int bytes_received = 0;
  while (STOP == FALSE){
    res = read(fd, &c, 1);
    bytes_received++;


    if( res == -1 ){
      printf("Error reading from buffer\n");
      exit(1);
    }

    printf("Trame received (receptor)-> %02x\n", c);

    updateStateMachine(state, 1, c);

    if (state == STOP_ ) {
      STOP = TRUE;
    }

  }

  printf("Bytes received from sender: %d\n",bytes_received);
  printf("Sending the string to emissor\n");

  char receiver_trame[6] = {FLAG, A_SR, C_UA, BCC(A_SR,C_UA), FLAG, '\0'};

  int res_w;
  res_w = write(fd,receiver_trame,6);
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
