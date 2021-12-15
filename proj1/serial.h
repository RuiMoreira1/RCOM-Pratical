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

/**
 * @brief Function to parse arguments and options, given by the terminal
 * 
 * @param argc              terminal number of arguments
 * @param argv              terminal arguments
 * @param id                1->Receiver 0->Sender
 * @param file              file being received/sent by the serial port
 * @param serialPort        serial port  
 * @return int*             Flag frame, with options disabled/enabled        
 */
int* parseArgs(int argc, char **argv, char *id, char *file, char *serialPort);

/**
 * @brief Function to print to stdout serialport usage and running mode
 * 
 */
void printHelpMessage();


/**
 * @brief Program flow control
 * 
 * @param argc              terminal number of arguments
 * @param argv              terminal arguments
 * @return int              SUCCESS upon success, ERROR otherwise
 */
int execution(int argc, char **argv);

/**
 * @brief Validate the inputs given in terminal by the user
 * 
 * @param argc              terminal number of arguments
 * @param argv              terminal arguments 
 * @param id                1->Receiver 0->Sender
 * @param file              file being received/sent by the serial port
 * @param identifier        Emissor || Receiver inputs from terminal
 * @param serialPort        serial port 
 * @return int              SUCCESS upon success, ERROR otherwise
 */
int validateArgs(int argc, char **argv, int *id, char *file, char *identifier, char *serialPort );;




#endif
