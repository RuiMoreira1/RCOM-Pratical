#ifndef RECEIVER_H
#define RECEIVER_H


#include "common.h"

int openReceiver(char filename[]);

int receiveSetFrame(int fd);

int closeReceiver(int fd);

int receiverDisc(int fd);

int dataDeStuffing(char *stuffedBuffer, int stuffedBufferSize, char *buffer, char *BCC2);

int receivedStuffedData(int fd, char *stuffedData);



#endif
