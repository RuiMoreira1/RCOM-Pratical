/**
 * Non-Canonical Input Processing
 * From https://tldp.org/HOWTO/Serial-Programming-HOWTO/x115.html by Gary Frerking and Peter Baumann
**/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include "common.h"
#include "sender.h"


#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define BAUDRATE B38400
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1

volatile int STOP_RUNNING = FALSE;

MACHINE_STATE senderState;

int conta = 0, flag = 1;

struct termios oldtio;


void answerAlarm(){
	flag = 1; conta++;
}

void resetAlarmFlags(){
	flag = 1; conta = 0;
}

int openSender(char filename[]){
	int fd;
	struct termios newtio;


	(void)signal(SIGALRM, answerAlarm); /*Setup alarm for checking interval of non-response by the receiver*/

	/*
    Open serial port device for reading and writing and not as controlling tty
    because we don't want to get killed if linenoise sends CTRL-C.
  */

  fd = open(filename, O_RDWR | O_NOCTTY);
  if (fd < 0){
    perror(filename);
    return ERROR;
  }

  if (tcgetattr(fd, &oldtio) == -1){ /* save current port settings */
    perror("tcgetattr");
    return ERROR;
  }

  bzero(&newtio, sizeof(newtio));
  newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
  newtio.c_iflag = IGNPAR;
  newtio.c_oflag = 0;

  /* set input mode (non-canonical, no echo,...) */
  newtio.c_lflag = 0;

  newtio.c_cc[VTIME] = 1; /* unblocks after 0.1s and after 1 char is read */
  newtio.c_cc[VMIN] = 0;


  tcflush(fd, TCIOFLUSH);

  if (tcsetattr(fd, TCSANOW, &newtio) == -1)
  {
    perror("tcsetattr");
    return ERROR;
  }

	if( sendSetFrame(fd) < 0 ) return ERROR;

  return fd;
}


int sendSetFrame(int fd){
		resetAlarmFlags(); /*Upon sending the SET FRAME reset the alarm flags, upon checking for receiver timeout*/


		MACHINE_STATE setState = START_;

		char setFrame[5] = {FLAG, A_SR, C_SET, BCC(A_SR,C_SET),FLAG};

		while(setState != STOP_){
			if( conta == 3 ){
				printf("Entered coms\n");
				perror("Communication between Receiver && Sender failed\n");
				exit(1);
			}

			if( flag ){
				printf("Entered write to fd\n");
				flag = 0; /* Disable message send flags */
				if( write(fd, setFrame, 5) == -1 ){
					perror("Error writing to Serial Port SET trame\n");
					return ERROR;
				}
				setState = START_;
				alarm(ALARM_INTERVAL);
			}

			if( checkSupervisionFrame(&setState, fd, A_SR, C_UA, NULL) < 0) return ERROR;  /* Getting information byte by byte */
		}

		if(DEBUG) printf("Sucessfully got UA response from receiver\n");

		alarm(0); /* Disconnect alarm */

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

	printf("Sender active\n");
  openSender(argv[1]);


}
