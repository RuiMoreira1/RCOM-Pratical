#include "connection.h"

struct hostent *getip(char hostname[])
{
    printf("Getting IP Address from Host Name...\n");
    struct hostent *h;

    if ((h = gethostbyname(hostname)) == NULL)
    {
        herror("Failed to get host by name");
        exit(1);
    }

    return h;
}

int connectSocket(char *addr, int port)
{
    printf("Connecting to Server Socket...\n");

    int sockfd;
    struct sockaddr_in server_addr;

    /*server address handling*/
    bzero((char *)&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(addr); /* 32 bit Internet address network byte ordered */
    server_addr.sin_port = htons(port);            /* server TCP port must be network byte ordered */

    /* open a TCP socket */
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        fprintf(stderr,"socket()");
        return ERROR;
    }

    /* connect to the server */
    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        fprintf(stderr,"connect()");
        return ERROR;
    }

    fprintf(stdout,"Server Socket Connected\n");

    return sockfd;
}

int getReply(int socketFd, char *code, char *text)
{
    char c;
    int i = 0;
    serverResponse state = STATUS_CODE;

    /*State Machine to get the status code and the rest of the arguments provided by the server side*/
    while (1){
        switch (state){
            case STATUS_CODE:
                if (read(socketFd, code, 3) < 0){
                    fprintf(stderr, "Error reading status code\n");
                    return ERROR;
                }

                fprintf(stdout,"Status Code -> %s", code);
                state = IS_MULTI_LINE;
                break;
            
            case IS_MULTI_LINE:
                if (read(socketFd, &c, 1) < 0){
                    fprintf(stderr, "Error reading response\n");
                    return ERROR;
                }

                fprintf(stdout,"%c", c);

                if (c == MULTI_LINE_SYMBOL) state = READ_MULTI_LINE;
                else state = READ_LINE;
                break;
            
            case READ_MULTI_LINE:
                int idxCounter = 0;
                char str[4];
                str[3] = '\0';

                while (1){
                    if (read(socketFd, &c, 1) < 0){
                        fprintf(stderr, "Error reading response\n");
                        return -1;
                    }

                    fprintf(stdout,"%c", c);
                    if (c == '\n') break;

                    if (idxCounter <= 2)str[idxCounter] = c;

                    if (idxCounter == 3 && strcmp(str, code) == 0 && c == LAST_LINE_SYMBOL){
                        state = READ_LINE;
                        break;
                    }

                    /*if the symbol given by the server is different from carriadge or the text is null insert the char read to the server response*/
                    if (idxCounter > 3 && text != NULL && c != CARRIAGE_RETURN) text[i++] = c;

                    idxCounter++;
                }

                break;

            case READ_LINE:
                while (1){
                    if (read(socketFd, &c, 1) < 0){
                        fprintf(stderr, "Error reading response\n");
                        return ERROR;
                    }

                    fprintf(stdout,"%c", c);
                    if (c == '\n') break;
                    
                    /*if the symbol given by the server is different from carriadge or the text is null insert the char read to the server response*/
                    if (text != NULL && c != CARRIAGE_RETURN) text[i++] = c;
                }

                if (text != NULL) text[i++] = '\0';
                return 0;

            default:
                break;
        }
    }
}

int sendRequest(int sockfd, char *cmd, char *argument){
    size_t cmd_len = strlen(cmd);

    if (write(sockfd, cmd, cmd_len) != cmd_len){
        fprintf(stderr, "Error while sending command\n");
        return ERROR;
    }

    /* Before sending the argument to the server, use a ' ' to help distinguish the blocks status ' ' argument*/
    if (argument != NULL){
        size_t arg_len = strlen(argument);
        char c = ' ';

        if (write(sockfd, &c, 1) != 1){
            fprintf(stderr, "Error while sending command\n");
            return ERROR;
        }

        if (write(sockfd, argument, arg_len) != arg_len){
            fprintf(stderr, "Error while sending command argument\n");
            return ERROR;
        }
    }

    /*Write a new line to the end of the command to end the request*/
    char c = '\n';
    if (write(sockfd, &c, 1) != 1){
        fprintf(stderr, "Error while sending command\n");
        return ERROR;
    }

    return SUCCESS;
}

int userLogin(int sockfd, char *user, char *pass){
    fprintf(stdout,"Sending username to the ftp server %s...\n",user);
    int res = handleCommunication(sockfd, "user", user, NULL); /* Sending the username to the ftp server, receiving a response */
    if (res < 0){
        fprintf(stderr, "Error while sending username to the ftp server!\n");
        return ERROR;
    }

    if (res == 1){
        fprintf(stderr,"Sending password to the ftp server...\n");

        if (handleCommunication(sockfd, "pass", pass, NULL) < 0){
            fprintf(stderr, "Error while sending password to the ftp server!\n");
            return ERROR;
        }
    }

    return SUCCESS;
}

int serverPort(int sockfd, int *port){
    char responseCode[4];
    memset(responseCode, 0, 4);

    char response[PSV_RESPONSE_MAXSIZE];

    fprintf(stdout,"Sending pasv...\n");

    if (handleCommunication(sockfd, "pasv", NULL, response) < 0){
        fprintf(stderr, "Error while sending pasv\n");
        return ERROR;
    }

    if (parsePort(response, port) < 0){
        fprintf(stderr, "Error parsing port\n");
        return ERROR;
    }

    return SUCCESS;
}


int downloadFile(int sockfd, int downloadFd, char *path)
{
    char responseCode[4];
    memset(responseCode, 0, 4);
    char fileName[MAX_PATH_SIZE];

    char cmdResponse[MAX_RESPONSE_SIZE];

    int res = handleCommunication(sockfd, "retr", path, cmdResponse);

    size_t fileSize = parseFileSize(cmdResponse);

    if (res < 0){
        fprintf(stderr, "Error while sending retr\n");
        return ERROR;
    }
    else if (res != 2){
        fprintf(stderr, "Server refused to transfer file\n");
        return ERROR;
    }

    parseFileName(path, fileName);
    if (saveFile(downloadFd, fileName, fileSize) < 0) return ERROR;

    if (getReply(sockfd, responseCode, NULL) < 0){
        fprintf(stderr, "Failed to confirm file transfer\n");
        return ERROR;
    }

    return SUCCESS;
}

int saveFile(int downloadFd, char *fileName, size_t fileSize)
{
    FILE *file = fopen(fileName, "wb");

    uint8_t buf[FILE_BUFFER_SIZE];
    int bytes;
    size_t readBytes = 0;
    size_t percentage = -1;
    while ((bytes = read(downloadFd, buf, FILE_BUFFER_SIZE)) > 0)
    {
        if (bytes < 0)
        {
            fprintf(stderr, "Error while reading file\n");
            return ERROR;
        }
        fwrite(buf, bytes, 1, file);

        if (fileSize > 0)
        {
            readBytes += bytes;
            size_t newPercentage = (readBytes * 100) / fileSize;

            fflush(stdout);
            if (newPercentage != percentage)
            {
                printf("\33[2K\r");
                percentagePrint(newPercentage);
            }
            percentage = newPercentage;
        }
    }
    fprintf(stdout,"\n");

    fclose(file); /*Close the file descriptor after the file being succesfully downloaded*/
    return SUCCESS;
}

int handleCommunication(int sockfd, char *cmd, char *argument, char *text)
{

    char responseCode[4];
    memset(responseCode, 0, 4);
    /*Send a request to the ftp server*/
    if (sendRequest(sockfd, cmd, argument)){
        fprintf(stderr, "Error while sending username\n"); 
        return ERROR;
    }

    /*Acknowledge the reply from the server*/
    int code;
    if (getReply(sockfd, responseCode, text) < 0) return ERROR;
    /*Get the status code from the reply*/
    code = responseCode[0] - '0';

    switch (code){
        /* Expecting another reply  */
        case 1:
            return 2;

        /* positive completion reply */
        case 2:
            break;

        /* waiting for more information */
        case 3:
            return 1;

        /* resend the command */
        case 4:
            if (handleCommunication(sockfd, cmd, argument, text) < 0)
                return ERROR;
            break;

        /* permanent negative completion reply */
        case 5:
            fprintf(stderr, "Command wasn't accepted\n");
            return ERROR;
    }

    return SUCCESS;
}

void percentagePrint(size_t percentage){
    if(percentage < 5) fprintf(stdout, "Progress: [###>                    ] (%lu%%)", percentage);
    else if(percentage < 10) fprintf(stdout, "Progress: [#######>                  ] (%lu%%)", percentage);
    else if(percentage < 15) fprintf(stdout, "Progress: [#########>                 ] (%lu%%)", percentage);
    else if(percentage < 20) fprintf(stdout, "Progress: [##########>                ] (%lu%%)", percentage);
    else if(percentage < 25) fprintf(stdout, "Progress: [###########>               ] (%lu%%)", percentage);
    else if(percentage < 30) fprintf(stdout, "Progress: [############>              ] (%lu%%)", percentage);
    else if(percentage < 35) fprintf(stdout, "Progress: [#############>             ] (%lu%%)", percentage);
    else if(percentage < 40) fprintf(stdout, "Progress: [##############>            ] (%lu%%)", percentage);
    else if(percentage < 45) fprintf(stdout, "Progress: [###############>           ] (%lu%%)", percentage);
    else if(percentage < 50) fprintf(stdout, "Progress: [################>          ] (%lu%%)", percentage);
    else if(percentage < 55) fprintf(stdout, "Progress: [#################>         ] (%lu%%)", percentage);
    else if(percentage < 60) fprintf(stdout, "Progress: [##################>        ] (%lu%%)", percentage);
    else if(percentage < 65) fprintf(stdout, "Progress: [###################>       ] (%lu%%)", percentage);
    else if(percentage < 70) fprintf(stdout, "Progress: [####################>      ] (%lu%%)", percentage);
    else if(percentage < 75) fprintf(stdout, "Progress: [#####################>     ] (%lu%%)", percentage);
    else if(percentage < 80) fprintf(stdout, "Progress: [######################>    ] (%lu%%)", percentage);
    else if(percentage < 85) fprintf(stdout, "Progress: [#######################>   ] (%lu%%)", percentage);
    else if(percentage < 90) fprintf(stdout, "Progress: [########################>  ] (%lu%%)", percentage);
    else if(percentage < 95) fprintf(stdout, "Progress: [##########################> ] (%lu%%)", percentage);
    else fprintf(stdout, "Progress: [###############################] (%lu%%)", percentage);
}

