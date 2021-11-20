#ifndef RECEIVER_H
#define RECEIVER_H


#include "common.h"

int openReceiver(char filename[]);

int receiveSetFrame(int fd);

int closeReceiver(int fd);

int receiverDisc(int fd);



#endif
