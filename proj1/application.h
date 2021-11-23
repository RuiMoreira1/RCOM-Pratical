#ifndef APPLICATION_H
#define APPLICATION_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <libgen.h>
#include <stdint.h>
#include "dataLayer.h"
#include "macros.h"

//Control field

#define CF_DATA 1
#define CF_START 2
#define CF_END 3

//T PARAMETER

#define T_F_SIZE 0
#define T_F_NAME 1

int makeControlPacket(int fd, long fSize,char fName[], char controlField);

int readControlPacket(int fd, long* fSize,char** fName, char controlField, uint8_t buf[]);

void makeDataPacket(uint8_t* fData ,int fDataSize ,uint8_t* dPacket,int dPacketSize,uint8_t sequence);

int sendDataPacket(int fd, long fSize, FILE* fd1);

int sendFile(int fd,char path[]);

int readFile(int fd);

#endif