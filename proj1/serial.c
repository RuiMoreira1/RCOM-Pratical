#include "serial.h"


int parseArgs(int argc, char **argv){
  int options;

  int res = SUCCESS;

  bool temp = (argc < 3 || ((strcmp(serialPort1, argv[1])) && (strcmp(serialPort2, argv[1]))) || ( (strcmp(serialPort1, argv[2])) && (strcmp(serialPort2, argv[2])) ) ) ;
  if (temp) {

    printf("Usage: application serialPort type <fileName>\nex: application /dev/ttyS10 0 ./path_to_file\n");
    res = ERROR;
  }

  int i = 0;
  while( (options = getopt(argc, argv, ":dh")) && i++ < argc ){
    printf("%c\n",options);
    switch (options) {
      case 'd':
        printf("sada");
        if(!temp){
          printf("Dasdas");
          fprintf(stdout, "Debug Version enabled, printing contents to output.txt and err.txt\n");
          freopen("output.txt","a+", stdout);
          freopen("err.txt","a+", stderr);
          return 2;
        }
        break;
      case 'h':
        fprintf(stdout, "/serialport /dev/ttySRX W ./path_to_image\n");
        fprintf(stdout, "/dev/ttySRX  RX-> serial port number\n");
        fprintf(stdout, "W -> [0] for sender [1] for receiver\n");
        fprintf(stdout, "./path_to_image, ./pinguim.gif by default\n");
        break;
      case '?':
        fprintf(stdout, "Unknown option\n");
        break;
    }
  }

  return res;
}


int execution(int argc, char **argv){

  int acc = 0;
  int res = parseArgs(argc, argv);
  if(res == ERROR) return ERROR;
  else if( res == 2 ) acc = 1;



  int fd, status = atoi(argv[2+acc]);
  char* filePath = argc > (3+acc) ? argv[3+acc] : "./pinguim.gif";

  if (status != SENDERID && status != RECEIVERID) {
    printf("application 2nd argument(type) should be either 0 or 1.\n0-Sender\n1-Receiver\n");
    return ERROR;
  }

  if ((fd = llopen(argv[1+acc], status)) < 0) {
    fprintf(stderr, "Error on llopen\n");
    return ERROR;
  }

  if (status == SENDERID) {
    if (sendFile(fd, filePath) < 0) {
      fprintf(stderr, "Error sending file\n");
      return ERROR;
    }

  }
  else {
    if (readFile(fd) == ERROR) {
      fprintf(stderr, "Error reading file\n");
      return ERROR;
    }
  }

  if (llclose(fd,status) == ERROR) {
    fprintf(stderr, "Error on llclose\n");
    return ERROR;
  }

  return SUCCESS;
}




int main(int argc, char **argv){

  if( execution(argc, argv) == ERROR ){
    fprintf(stdout, "File %s was not sent correctly\n", argc > 3 ? argv[3] : "./pinguim.gif");
    return ERROR;
  }
  else{
    fprintf(stdout, "File %s was sent correctly\n", argc > 3 ? argv[3] : "./pinguim.gif");
    return SUCCESS;
  }
}
