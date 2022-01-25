#ifndef CONNECTION_H
#define CONNECTION_H

#include "parser.h"

#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "parser.h"

#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define FTP_SERVER_PORT 21
#define MULTI_LINE_SYMBOL '-'
#define LAST_LINE_SYMBOL ' '
#define CARRIAGE_RETURN 13

#define PSV_RESPONSE_MAXSIZE 1000
#define FILE_BUFFER_SIZE 256

typedef enum serverResponse
{
  STATUS_CODE,
  IS_MULTI_LINE,
  READ_MULTI_LINE,
  READ_LINE
} serverResponse;

/**
 * The struct hostent (host entry) with its terms documented

    struct hostent {
        char *h_name;    // Official name of the host.
        char **h_aliases;    // A NULL-terminated array of alternate names for the host.
        int h_addrtype;    // The type of address being returned; usually AF_INET.
        int h_length;    // The length of the address in bytes.
        char **h_addr_list;    // A zero-terminated array of network addresses for the host.
        // Host addresses are in Network Byte Order.
    };

    #define h_addr h_addr_list[0]	The first address in h_addr_list.
*/

/**
 * @brief Gettip function provided by code examples in class, using the function gethostname to fill the hostent struct with the correct information
 * 
 * @param hostname              - Hostname 
 * @return struct hostent*      Returns the hostent struct with the data correctly filled upon success, exits with code 1 upon failure.
 */
struct hostent *getip(char hostname[]);

/**
 * @brief Connect the ftp server socket, open the ftp server file descriptor
 * 
 * @param addr      - The ip address of the client
 * @param port      - The port oppened by the ftp server where data will be sent from
 * @return int      The socket file descriptor upon SUCCESS, ERROR otherwise.
 */
int connectSocket(char *addr, int port);

/**
 * @brief Get the Response from the ftp server, after a request has been sent
 * 
 * @param socketFd  - File descriptor of the ftp server
 * @param code      - Response status code
 * @param text      - Arguments given by the ftp server
 * @return int      The socket file descriptor upon SUCCESS, ERROR otherwise.
 */
int getReply(int socketFd, char *code, char *text);

/**
 * @brief Send commands to the ftp server, commands can be sent with or without arguments, depending on its nature
 * 
 * @param sockfd    - File descriptor of the ftp server
 * @param cmd       - Command to be sent to the ftp server
 * @param argument  - Argument that goes after the command, can be null if the command takes no arguments
 * @return int      Returns SUCCESS upon Success, ERROR otherwise.
 */
int sendRequest(int sockfd, char *cmd, char *argument);

/**
 * @brief Function to deal with userLogin fraction of the ftp server connection, handling the server responses for the username and password, respectively
 * 
 * @param sockfd    - File descriptor of the ftp server
 * @param user      - Username parsed from the command line
 * @param pass      - Password parsed from the command line
 * @return int      Returns SUCCESS upon Success, ERROR otherwise.
 */
int userLogin(int sockfd, char *user, char *pass);

/**
 * @brief Get the Port object from the ftp server, if the server acknowledges the request without any errors, parse the port value
 * 
 * @param sockfd    - File descriptor of the ftp server
 * @param port      - FTP port to send the files
 * @return int      Returns SUCCESS upon Success, ERROR otherwise.
 */
int serverPort(int sockfd, int *port);

/**
 * @brief Download the file from the ftp server, sending rtrv request to ackownledge the download request
 * 
 * @param sockfd    - File descriptor of the ftp server
 * @param downloadFd - File descriptor of the file to be downloaded
 * @param path      - File download internal ftp path
 * @return int      Returns SUCCESS upon Success, ERROR otherwise.
 */
int downloadFile(int sockfd, int downloadFd, char *path);

/**
 * @brief Save the file being sent by the ftp server through the port
 * 
 * @param downloadFd - File descriptor of the file to be downloaded 
 * @param filename   - Filename parsed by the console input
 * @param fileSize   - Filesize received by the server response acknowledgment after the SIZE request
 * @return int       Returns SUCCESS upon Success, ERROR otherwise.
 */
int saveFile(int downloadFd, char *filename, size_t fileSize);

/**
 * @brief Handle the client server communication, send the request interpret the reply
 * 
 * @param sockfd    - File descriptor of the ftp server
 * @param cmd       - Command to be sent to the ftp server
 * @param argument  - Argument to be sent to the ftp server can be null if the cmd takes no arguments
 * @param text      - Server reply 
 * @return int      Returns SUCCESS upon Success, ERROR otherwise.
 */
int handleCommunication(int sockfd, char *cmd, char *argument, char *text);

/**
 * @brief Show the download progress bar
 * 
 * @param percentage  - Download percentage
 */
void percentagePrint(size_t percentage);
#endif

