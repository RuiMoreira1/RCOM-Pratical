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

MACHINE_STATE senderState;

int conta = 0, flag = 1;

void atende(){
	printf("alarme # %d\n", conta+1);
	flag=1;
	conta++;
}

int main(int argc, char **argv)
{
  int fd, res;
  struct termios oldtio, newtio;
  char buf[255];

  int i;

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

  //fgets(buf,255,stdin);

  char transmmiter_trame[5] = {FLAG, A_SR, C_SET, BCC(A_SR,C_SET), FLAG};


  res = write(fd, transmmiter_trame, 5);
  if( res == -1 ){
    printf("Error writing to fd\n");
    exit(1);
  }

  printf("%d bytes written\n", res);

  printf("Receiving info from serial port\n");

  char c;
  int res2 = 0;
  MACHINE_STATE senderState = START_;

  while (STOP == FALSE){
    if( conta == 3 ){
      printf("Communication failed\n");
      break;
    }

    if( flag ){
      if( write(fd, transmmiter_trame, 5) == -1 ){
        printf("Error writing to fd\n");
        exit(1);
      }
      flag = 0;
      alarm(3);
    }
    

    if( res2 == -1 ){
      printf("Error reading from buffer\n");
      exit(1);
    }


    printf("Trame received (emissor)-> %02x\n",c);

    updateStateMachine(&senderState, SENDERID, c);

    if (senderState == STOP_) STOP = TRUE;
  }


  /* Aguardar um pouco que esteja escrito tudo antes de mudar
    a config do terminal.  */
  sleep(1);
  if (tcsetattr(fd, TCSANOW, &oldtio) == -1)
  {
    perror("tcsetattr");
    exit(-1);
  }

  close(fd);
  return 0;
}
