#ifndef SENDER_H
#define SENDER_H

#include <signal.h>
#include "macros.h"


void answerAlarm();

void resetAlarmFlags();

int openSender();

int sendSetFrame();


#endif
