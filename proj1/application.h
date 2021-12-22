#ifndef APPLICATION_H
#define APPLICATION_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <libgen.h>

#include "dataLayer.h"
#include "macros.h"

#define PACKET_DATA_SIZE 200

// T BYTE
#define T_TYPE_SIZE 0
#define T_TYPE_NAME 1

// PACKETS CONSTANTS
#define C_DATA 1
#define C_START 2
#define C_END 3

// DATA PACKET CONSTANTS
#define ADD_FIELDS 4
#define D_REAL_SIZE (PACKET_DATA_SIZE - ADD_FIELDS)
#define N_MULT 256

#define min(a,b) (((a) < (b)) ? (a) : (b))




//

// T BYTE
#define T_FILE_SIZE 0
#define T_FILE_NAME 1


// PACKETS CONSTANTS
#define DATA_CTRL 1
#define START_CTRL 2
#define END_CTRL 3

// DATA PACKET CONSTANTS
#define NUM_DATA_ADDITIONAL_FIELDS 4
#define DATA_ACTUAL_SIZE (PACKET_DATA_SIZE - NUM_DATA_ADDITIONAL_FIELDS)
#define NUM_OCTETS_MULTIPLIER 256

#define min(a,b) (((a) < (b)) ? (a) : (b))


/**
* @brief Builds data packet, from a given data buffer
*/
void dataPacketBuilder(u_int8_t* dPacket,int dPacketSize, u_int8_t* fData, int fDataSize, u_int8_t sequence);

/**
* @brief Sends data packet
*
* @param fd                         File Descriptor
* @param fd1                        Pointer to file being trasnferred
* @param fSize                      file size
* @return               1 upon sucess, otherwise -1
*/
int sendData(int fd, FILE* fd1, long fSize);


/**
 * @brief creates and sends a control packet
 *
 * @param  fd                      file descriptor
 * @param  cField                  control field
 * @param  fSize                   file size
 * @param  fName                   file name
 * @return              1 upon sucess, otherwise -1 
 */
int sendControlPacket (int fd, u_int8_t cField, long fSize, char fName[]);

/**
 * @brief Sends the controlPackets and the data packet
 *
 * @param  fd                       File descriptor
 * @param  fPath                    File Path
 * @return          file Size upon success, otherwise -1
 */
int sendFile(int fd, char fPath[]);


/**
 * @brief Reads all packets and creates the file read
 *
 * @param  fd                       File descriptor
 * @return    file Size upon success, otherwise -1
 */
int readFile(int fd);

/**
 * @brief reads a control packet and alocates the file name and file size 
 *
 * @param  fd                       file descriptor
 * @param  cField                   control field
 * @param  buf                      buffer where the control packet is
 * @param  fName                    file name 
 * @param  fSize                    file size
 * @return              1 upon sucess, otherwise -1
 */
int readControlPacket(int fd, u_int8_t cField, u_int8_t buf[], char** fName, long* fSize);

#endif
