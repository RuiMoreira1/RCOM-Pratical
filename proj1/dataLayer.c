#include "dataLayer.h"

int llopen(char *filename, int id){

  int fd;

  if( id == RECEIVERID ) fd = openReceiver(filename);
  else if( id == SENDERID ) fd = openSender(filename);
  else{
    fprintf(stderr, "Invalid id given, either (0) for sender or (1) for receiver\n");
    return ERROR;
  }

  return fd;
}


int llclose(int fd, int id){
  int result;

  if( id == RECEIVERID ) result = closeReceiver(fd);
  else if( id == SENDERID ) result = closeSender(fd);
  else{
    fprintf(stderr, "Invalid id given, either (0) for sender or (1) for receiver\n");
    return ERROR;
  }

  if( result == ERROR ){
    fprintf(stderr, "Error closing %s", id == 1 ? "receiver" : "sender");
    return ERROR;
  }

  fprintf(stdout,"Closing %s\n", id == 1 ? "Receiver" : "Sender");
  return SUCCESS;
}


int llwrite(int fd, char *buffer, int size, int id){
  if( id == RECEIVERID ){
    fprintf(stderr,"The receiver is unable send data\n");
    return ERROR;
  }
  return sendStuffedFrame(fd, buffer, size);
}


int llread(int fd, char *buffer, int id){
  if( id == SENDERID ){
    fprintf(stderr, "The sender is unable to read data\n");
    return ERROR;
  }

  return receivedStuffedData(fd, buffer);
}
