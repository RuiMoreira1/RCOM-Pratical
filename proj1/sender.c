#include "sender.h"


MACHINE_STATE senderState;

int conta = 0, flag = 1, s = 0;

struct termios oldtio;


void answerAlarm(){
	flag = 1; conta++;
	fprintf(stderr, "Timeout\n");
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
    fprintf(stderr,"%s\n",filename);
    return ERROR;
  }

  if (tcgetattr(fd, &oldtio) == -1){ /* save current port settings */
    fprintf(stderr,"tcgetattr\n");
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
    fprintf(stderr,"tcsetattr\n");
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
		if( conta == MAX_NO_ANSWER ){
				fprintf(stderr,"Communication between Receiver && Sender failed SET\n");
				return ERROR;
		}

		if( flag ){
			flag = 0; /* Disable message send flags */
			if( write(fd, setFrame, 5) == -1 ){
				fprintf(stderr,"Error writing to Serial Port SET trame\n");
				return ERROR;
			}
			setState = START_;
			alarm(ALARM_INTERVAL);
		}

		if( checkSupervisionFrame(&setState, fd, A_SR, C_UA, NULL) == ERROR) return ERROR;  /* Getting information byte by byte */
	}

	if(DEBUG) fprintf(stdout,"Sucessfully got UA response from receiver\n");

	alarm(0); /* Disconnect alarm */

	return SUCCESS;
}


int senderDisc(int fd){
	resetAlarmFlags();  /* Reset alarm flags */

	char frame[5] = {FLAG, A_SR, C_DISC, BCC(A_SR,C_DISC), FLAG};

	MACHINE_STATE senderState = START_;

	while( senderState != STOP_ ){
		if( conta == MAX_NO_ANSWER ){
			fprintf(stdout,"Communication between Receiver && Sender failed DISC\n");
			return ERROR;
		}

		if( flag ){
			flag = 0;
			if( write(fd, frame, 5) == ERROR ){
				fprintf(stderr,"Error writing to Serial Port DISC frame\n");
				return ERROR;
			}
			senderState = START_;
			alarm(ALARM_INTERVAL);
		}

		if( checkSupervisionFrame(&senderState, fd, A_SR, C_DISC, NULL) == ERROR) return ERROR;
	}

	alarm(0); /* Disable alarm, frame read correctly */

	if(sendSupervisionFrame(fd, A_SR, C_UA) == ERROR) return ERROR;

	return SUCCESS;

}


int closeSender(int fd){
	if( senderDisc(fd) == ERROR ) return ERROR;

	sleep(1);

	if (tcsetattr(fd, TCSANOW, &oldtio) == -1)
  {
    fprintf(stderr,"tcsetattr\n");
    return ERROR;
  }
  close(fd);
  return SUCCESS;
}


int dataStuffing(char* buffer, int dataSize, char BCC2, char* stuffedBuffer){
	int temp = 0; /*Temporary variable used to insert the stuffed data inside the buffer and then used to return has the stuffedBuffer size*/

	/*Data Stuffing before BCC2 */
	for(int i = 0; i < dataSize; i++){
		if( buffer[i] == FLAG ){
			stuffedBuffer[temp++] = ESCAPE;
			stuffedBuffer[temp++] = FLAG_ESCAPE_XOR;
		}
		else if( buffer[i] == ESCAPE ){
			stuffedBuffer[temp++] = ESCAPE;
			stuffedBuffer[temp++] = ESCAPE_XOR;
		}
		else stuffedBuffer[temp++] = buffer[i];
	}

	/*BCC2 Stuffing*/
	if( BCC2 == FLAG ){
		stuffedBuffer[temp++] = ESCAPE;
		stuffedBuffer[temp++] = FLAG_ESCAPE_XOR;
	}
	else if( BCC2 == ESCAPE ){
		stuffedBuffer[temp++] = ESCAPE;
		stuffedBuffer[temp++] = ESCAPE_XOR;
	}
	else stuffedBuffer[temp++] = BCC2;


	return temp;
}


int sendStuffedFrame(int fd, char* buffer, int bufferSize){
	resetAlarmFlags(); /* Resetting alarm variables to send stuffed byte */

	if( bufferSize > MAX_DATA_SIZE ){
		fprintf(stderr,"Buffer Size exceeded the var(MAX_DATA_SIZE) value\n");
		return ERROR;
	}

	char I_C_BYTE = C_FRAME_I(s);
	char BCC2;

	if( createBCC2(buffer, bufferSize, &BCC2) == ERROR ) {
		fprintf(stderr,"Error generating BCC2, data field problem\n");
		return ERROR;
	}

	char frameH[4] = {FLAG, A_SR, I_C_BYTE, BCC(A_SR,I_C_BYTE)};
	char frameT = FLAG;
	char stuffedBuffer[STUFF_DATA_MAX];

	int stuffedBufferSize = dataStuffing(buffer, bufferSize, BCC2, stuffedBuffer);
	//insertError(stuffedBuffer, 0, 2);
	//insertError(frameH,3,2);

	MACHINE_STATE stuffedBufferState = START_;

	while( stuffedBufferState != STOP_ ){
		if( conta == MAX_NO_ANSWER ){
			fprintf(stdout,"Communication between Receiver && Sender failed Stuffed frame\n");
			stuffedBufferState = STOP_;
			return ERROR;
		}

		if( flag ){
			flag = 0;

			if( write(fd , frameH, 4) == ERROR ){
				fprintf(stderr,"Error writing to Serial Port [Frame Header]\n");
				return ERROR;
			}

			if( write(fd, stuffedBuffer, stuffedBufferSize) == ERROR ){
				fprintf(stderr,"Error writing to Serial Port [Stuffed Data]\n");
				return ERROR;
			}

			if( write(fd, &frameT, 1) == ERROR ){
				fprintf(stderr,"Error writing to Serial Port [Frame Tail]\n");
				return ERROR;
			}

			stuffedBufferState = START_;
			alarm(ALARM_INTERVAL);
		}

		char rejB = C_REJ(s);

		int supervisionRes = checkSupervisionFrame(&stuffedBufferState, fd, A_SR, C_RR(1-s), &rejB );

		if(  supervisionRes == ERROR){
			fprintf(stderr,"Error receiving correct info from receiver\n");
			return ERROR;
		}
		else if ( supervisionRes > 0 ){
			flag = 1; conta++; /* Stuffed message wasn't acknowledged correctly resend frame */
		}

	}

	s = 1-s;

	alarm(0); /* Deactivate alarm message was sent and Acknowledgement was verified and validated correctly */

	return bufferSize;
}
