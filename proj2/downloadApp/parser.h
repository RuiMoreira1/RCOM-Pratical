#ifndef PARSE_H
#define PARSE_H

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define PROTOCOL_SIZE 6
#define MAX_USER_SIZE 256
#define MAX_PWD_SIZE 256
#define MAX_HOST_SIZE 512
#define MAX_PATH_SIZE 1024
#define MAX_RESPONSE_SIZE 4096

#define SUCCESS 0
#define ERROR  -1

typedef enum parserState
{
  PROTOCOL,
  USER,
  PASS,
  HOST,
  PATH
} parserState;

/**
 * @brief Function to parse input given from the console
 * 
 * @param consoleInput - Input given in the console for download application command
 * @param username     - Username provided to establish the FTP connection
 * @param password     - Password provided to establish the FTP connection associated to the Username
 * @param hostname     - FTB hostname
 * @param path         - Path inside the FTP server specifing the file we want to download
 * @return int         Return SUCCESS upon success, ERROR otherwise
 */
int parseConsoleInput(char *consoleInput, char *username, char *password, char *hostname, char *path);

/**
 * @brief Get the port number to access ftp server (par1 *256) + par2
 * 
 * @param response     - Passive mode data being received with the port information last two fields, with port information
 * @param port         - Passive mode port after being converted to real port number  
 * @return int         Return SUCCESS upon success, ERROR otherwise
 */
int parsePort(char *response, int *port);

/**
 * @brief Get where index of the console command where the hostname and path starts, right after @ identifier
 * 
 * @param input        - Console Input data 
 * @return int         Return -1 in case no user and no password is provided, otherwise the index where the hostname starts
 */
int getHostHelper(char *input);

/**
 * @brief Auxiliary function to reverse a string, and calculate the console port number
 * 
 * @param str          - String to be reversed to calculate port number
 * @return char*       Returns the port number in a string correctly reversed
 */
char *reverseString(char *str);

/**
 * @brief Given the file path, retrieve the filename wanted from the ftp server
 * 
 * @param path         - Console trimmed string containing the path to ftb server file
 * @param fileName     - Filename wanted from the ftp server correctly trimmed
 */
void parseFileName(char *path, char *fileName);

/**
 * @brief After getting a 213 success response from ftp server get the filesize of the file we want to download
 * 
 * @param text 
 * @return size_t = 0 if error 
 */
size_t parseFileSize(char *text);

#endif
