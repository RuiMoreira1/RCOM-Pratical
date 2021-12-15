#ifndef TEST_H
#define TEST_H


int* parseArgs(int argc, char **argv, char *id, char *file, char *serialPort);

void printHelpMessage();

int execution(int argc, char **argv);

int validateArgs(int argc, char **argv, int *id, char *file, char *identifier, char *serialPort );


#endif TEST_H