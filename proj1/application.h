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
void buildDataPacket(u_int8_t* dataPacket, int dataPacketSize, u_int8_t* frameData, int frameDataSize, u_int8_t sequenceNum);

/**
* @brief Sends data packet
* @param fd File Descriptor
* @param ptr Pointer to file being trasnferred
* @param fileSize Size of the file in bytes
* @return Positive if succeeded, negative otherwise
*/
int sendData(int fd, FILE* ptr, long fileSize);

/**
 * [sendControlPacket  description]
 * @param  fd                         [description]
 * @param  controlField               [description]
 * @param  fileSize                   [description]
 * @param  fileName                   [description]
 * @return              [description]
 */
int sendControlPacket (int fd, u_int8_t controlField, long fileSize, char fileName[]);

/**
 * [sendFile description]
 * @param  fd                     [description]
 * @param  filePath               [description]
 * @return          [description]
 */
int sendFile(int fd, char filePath[]);


/**
 * [readFile description]
 * @param  fd               [description]
 * @return    [description]
 */
int readFile(int fd);

/**
 * [readControlPacket description]
 * @param  fd                         [description]
 * @param  controlField               [description]
 * @param  buffer                     [description]
 * @param  fileName                   [description]
 * @param  fileSize                   [description]
 * @return              [description]
 */
int readControlPacket(int fd, int controlField, u_int8_t buffer[], char** fileName, long* fileSize);

#endif
