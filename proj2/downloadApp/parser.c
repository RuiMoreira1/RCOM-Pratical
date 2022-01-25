#include "parser.h"

int parseConsoleInput(char *consoleInput, char *username, char *password, char *hostname, char *path)
{
    int i = 0, j = 0;
    char protocol[PROTOCOL_SIZE + 1];
    memset(protocol, 0, PROTOCOL_SIZE + 1);

    parserState state = PROTOCOL;

    int hostIdx = getHostHelper(consoleInput);
    if (hostIdx == -1){ // Anonymous user
        strcpy(username, "anonymous");
        strcpy(password, "anonymous");
    }

    while (1)
    {
        char c = consoleInput[i++];

        if (c == '\0' && state != PATH){
            fprintf(stderr, "Wrong format:\nftp://[<user>:<password>@]<host>/<url-path>\n");
            return ERROR;
        }

        switch (state){
            case PROTOCOL:
                protocol[j++] = c;

                if (j == PROTOCOL_SIZE){
                    if (strcmp(protocol, "ftp://") != 0){
                        fprintf(stderr, "Please use ftp protocol://\n");
                        return ERROR;
                    }

                    if (hostIdx > -1) state = USER;
                    else state = HOST;
                    j = 0;
                }

                break;
            case USER:
                if (c == ':'){
                    state = PASS;
                    j = 0;
                    break;
                }

                username[j++] = c;

                if (j > MAX_USER_SIZE){
                    fprintf(stderr, "Username is too large! Max: %d\n", MAX_USER_SIZE);
                    return ERROR;
                }

                break;
            case PASS:
                if (i == hostIdx + 1){
                    state = HOST;
                    j = 0;
                    break;
                }

                password[j++] = c;

                if (j > MAX_PWD_SIZE){
                    fprintf(stderr, "Password is too large! Max: %d\n", MAX_PWD_SIZE);
                    return ERROR;
                }

                break;
            case HOST:
                if (c == '/'){
                    state = PATH;
                    j = 0;
                    break;
                }

                hostname[j++] = c;

                if (j > MAX_HOST_SIZE){
                    fprintf(stderr, "Hostname is too large! Max: %d\n", MAX_HOST_SIZE);
                    return ERROR;
                }

                break;
            case PATH:
                if (c == '\0')
                    return 0;

                path[j++] = c;

                if (j > MAX_PATH_SIZE){
                    fprintf(stderr, "File path is too large! Max: %d\n", MAX_PATH_SIZE);
                    return ERROR;
                }
                break;
            default:
                fprintf(stderr,"Args badly parsed from the console!");
                break;
        }
    }

    return SUCCESS;
}


char *reverseString(char *str)
{
    if (!str || !*str)
        return str;

    int i = strlen(str) - 1, j = 0;

    while (i > j)
    {
        char c = str[i];
        str[i--] = str[j];
        str[j++] = c;
    }

    return str;
}

int parsePort(char *response, int *port)
{
    size_t i = strlen(response) - 1, currIdx = 0;
    char first[4], second[4];
    memset(first, 0, 4);
    memset(second, 0, 4);

    if (response[i--] != ')')
    {
        if (response[i + 1] != '.' || response[i--] != ')')
        {
            fprintf(stderr, "Wrong response format: %s\n", response);
            return ERROR;
        }
    }

    while (response[i] != ',')
        second[currIdx++] = response[i--];
    i--;
    currIdx = 0;
    while (response[i] != ',')
        first[currIdx++] = response[i--];

    int p1 = atoi(reverseString(first));
    int p2 = atoi(reverseString(second));

    *port = (p1 * 256) + p2;
    return SUCCESS;
}

int getHostHelper(char *input)
{
    size_t len = strlen(input);

    int hostIdx = -1;

    for (int i = 0; i < len; i++)
    {
        if (input[i] == '@')
        {
            hostIdx = i;
        }
    }

    return hostIdx;
}

void parseFileName(char *path, char *fileName)
{
    size_t pathSize = strlen(path);

    int idx = 0;
    for (int i = pathSize - 1; i >= 0; i--)
    {
        char c = path[i];
        if (c == '/')
            break;
        fileName[idx] = c;
        idx++;
    }

    fileName[idx] = '\0';
    reverseString(fileName);
}

size_t parseFileSize(char *text)
{
    size_t textLen = strlen(text);

    size_t fileSize = 0;
    int i = textLen - 9;
    int counter = 1;
    while (text[i] != '(')
    {
        int digit = text[i] - '0';

        if (digit < 0 || digit > 9)
            return 0;

        fileSize += digit * counter;
        counter *= 10;
        i--;
    }

    return fileSize;
}
