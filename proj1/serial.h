#ifndef SERIAL_H
#define SERIAL_H

#include "dataLayer.h"
#include "application.h"
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>


#define serialPort1 "/dev/ttyS10"
#define serialPort2 "/dev/ttyS11"



int* parseArgs(int argc, char **argv, char *id, char *file, char *serialPort);

void printHelpMessage();

int execution(int argc, char **argv);

int validateArgs(int argc, char **argv, int *id, char *file, char *identifier, char *serialPort );;




#endif
