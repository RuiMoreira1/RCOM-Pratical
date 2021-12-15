#ifndef STATEMACHINE_H
#define STATEMACHINE_H

#include "macros.h"
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

/*! State machine enumeration class */
typedef enum {
   START_,        /* Start state */
   FLAG_RCV,      /* 0x7E is validated */
   A_RCV,         /* A Field byte is validated*/
   C_RCV,         /* C Field byte is validated */
   BCC_OK,        /* BCC(A_FIELD, C_FIELD) byte is validated */
   STOP_          /* Flags validated correctly */
 } MACHINE_STATE;


/**
 * @brief State Machine for checking I/S/U Frame responses
 *
 * @param  state                the current SM state, passed by reference so it can be updated
 * @param  fd                   serial port file descriptor
 * @param  A_BYTE               A field of I/S/U Frame
 * @param  C_BYTE               A field of I/S/U Frame
 * @param  reject               Negative Acknowledgement
 * @return        SUCCESS upon success, ERROR otherwise
 */
int checkSupervisionFrame(MACHINE_STATE *state, int fd, char A_BYTE, char C_BYTE, char* reject);

/**
 * @brief Read the serial port file descriptor byte by byte. Helper function in the
 * project state machine, to verify and validade answers from one side to the otherwise
 *
 * @param  fd                            serial port file descriptor
 * @param  byte_to_be_read               byte read from the serial port
 * @return                 SUCCESS upon success, ERROR otherwise
 */
int getBytefromFd(int fd, char *byte_to_be_read);

/**
 * @brief Send UA, DISC, SET Frames from one side to the other of the serial port
 * @param  fd                   serial port file descriptor
 * @param  A_BYTE               A Field of the UA/DISC/SET Frame
 * @param  C_BYTE               C Field of the UA/DISC/SET Frame
 * @return        SUCCESS upon success, ERROR otherwise
 */
int sendSupervisionFrame(int fd, char A_BYTE, char C_BYTE);

/**
 * @brief Create Information Trame BCC2, consisisting in a xor of all the data field
 * bits, therefore creating BCC2
 *
 * @param  buffer                   Information camp
 * @param  bufferSize               Information camp size
 * @return            BCC2 upon success, '\0' otherwise
 */
int createBCC2(char *buffer, int bufferSize, char *bcc2);

/**
 * @brief Create errors at runtime, to ensure the implementation strength
 * 
 * @param data                      Buffer
 * @param size                      Index of data buffer that will be changed
 * @param probability               Probability of introducing error
 */
void insertError(char *data, int size, int probability);

#endif
