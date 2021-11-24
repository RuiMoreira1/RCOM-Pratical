#ifndef SERIAL_H
#define SERIAL_H

#include "dataLayer.h"
#include "application.h"
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>

#define serialPort1 "/dev/ttyS10"
#define serialPort2 "/dev/ttyS11"



int parseArgs(int argc, char **argv);

int execution(int argc, char **argv);



#endif
