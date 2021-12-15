#include "serial.h"


int* parseArgs(int argc, char **argv, char *id, char *file, char *serialPort){
  int *flagFrame; /* Debug, Help, Identity ... */
  flagFrame = malloc(4* sizeof *flagFrame);
  int index, c;

  opterr = 0;

  while ((c = getopt (argc, argv, "dhi:f:")) != -1)
    switch (c){
      case 'd':
        flagFrame[0] = 1;
        break;
      case 'h':
        flagFrame[1] = 1;
        break;
      case 'i':
        flagFrame[2] = 1;
        strcpy(id,optarg);
        break;
      case 'f':
        flagFrame[3] = 1;
        strcpy(file,optarg);
        break;
      case '?':
        if (optopt == 'c') fprintf(stderr, "Option -%c requires an argument.\n", optopt);
        else if (isprint (optopt)) fprintf(stderr, "Unknown option `-%c'.\n", optopt);
        else fprintf(stderr,"Unknown option character `\\x%x'.\n",optopt);
        return NULL;
      default:
        abort();
      }

  if(argv[optind] == NULL && (flagFrame[1] != 1) ) {
    printf("here\n");
    return NULL;
  }
  if(argv[optind] != NULL) strcpy(serialPort, argv[optind]);
  
  return flagFrame;
} 

void printHelpMessage(){
  fprintf(stdout, "Usage: serialport [ID]... [OPTION] ... [FILE]...\n");
  fprintf(stdout, "Establish serial port connection to send [FILE] or receive [FILE]\n\n");
  fprintf(stdout, "If file path is not given, pinguim.gif will be sent/received by default, if available on execution root folder\n\n");
  fprintf(stdout, "\t-d\t\t\t enable debug mode, output written to files stdout.txt stderr.txt, respectively\n");
  fprintf(stdout, "\t-i\t\t\t serial port side identifier, sender || receiver\n" );
  fprintf(stdout, "\t-h\t\t\t serial port help message\n\n");
  fprintf(stdout, "Examples:\n");
  fprintf(stdout, "\tserialport -i sender -d -f ./file.txt\t\t\t Serial port sender side, debug mode active sending file.txt\n");
  fprintf(stdout, "\tserialport -d -i receiver\t\t\t\t Serial port receiver side, debug mode active receiving pinguim.gid\n");
}

int validateArgs(int argc, char **argv, int *id, char *file, char *identifier, char *serialPort ){
  int *res = parseArgs(argc, argv, identifier,file,serialPort);
  if( res == NULL ){
    fprintf(stderr,"No matching call\n");
    return ERROR;
  }
  if( res[1] == 1 ){
    printHelpMessage();
    return 3;
  }
  if( res[2] == 1 ){
    *id = -1;
    if( (strcmp(identifier,"emissor") != 0) && (strcmp(identifier,"receiver") != 0) ) return -1;
    else {
      //fprintf(stdout,"%s Side\n", identifier);
      *id = (strcmp(identifier,"emissor") == 0) ? 0 : 1;
    }
  }
  if( res[3] != 1 ) 
  {
    strcpy(file,"./pinguim.gif");
  }
  if( res[0] == 1 ) {
    fprintf(stdout,"Debug mode enabled\n");
  }
  return SUCCESS;
}

int execution(int argc, char **argv){
  int *i = malloc(sizeof(int));
  char *id = (char *) malloc(15*sizeof(char));
  char *file = (char *) malloc(100*sizeof(char));
  char *serialPort = (char*) malloc(20*sizeof(char));
  int res = validateArgs(argc, argv, i, file, id, serialPort);
  if( res == ERROR ){
    fprintf(stdout,"Bad input provided, try using serialport -h\n");
    return ERROR;
  }
  else if( res == 3 ) return SUCCESS;

   if ( ((strcmp("/dev/ttyS10", serialPort)) != 0) && ((strcmp("/dev/ttyS11", serialPort)) != 0) ) {

    printf("Usage: application serialPort type <fileName>\nex: application /dev/ttyS0 0 ./imagem.gif\n<fileName> default value: ./pinguim.gif\n");
    exit(1);
  }


  int fd, status = *i;

  if (status != SENDERID && status != RECEIVERID) {
    printf("application 2nd argument(type) should be either 0 or 1.\n0-Sender\n1-Receiver\n");
    return ERROR;
  }

  if ((fd = llopen(serialPort, status)) < 0) {
    fprintf(stderr, "Error on llopen\n");
    return ERROR;
  }

  if (status == SENDERID) {
    if (sendFile(fd, file) < 0) {
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

int main(int argc, char** argv) {
  /*if (argc < 3 ||
    ((strcmp("/dev/ttyS10", argv[1])) && (strcmp("/dev/ttyS11", argv[1])))) {

    printf("Usage: application serialPort type <fileName>\nex: application /dev/ttyS0 0 ./imagem.gif\n<fileName> default value: ./pinguim.gif\n");
    exit(1);
  }

  int fd, status = atoi(argv[2]);
  char* filePath = argc > 3 ? argv[3] : "./pinguim.gif";

  if (status != SENDERID && status != RECEIVERID) {
    printf("application 2nd argument(type) should be either 0 or 1.\n0-Emissor\n1-Receptor\n");
    exit(1);
  }

  if ((fd = llopen(argv[1], status)) < 0) {
    fprintf(stderr, "Failed to connect on llopen. The receiver is probably disconnected\n");
    exit(1);
  }

  if (status == SENDERID) {
    if (sendFile(fd, filePath) < 0) {
      fprintf(stderr, "Error sending file\n");
      //llclose(fd,SENDERID);
      exit(1);
    }
  } else {
    if (readFile(fd) < 0) {
      fprintf(stderr, "Error reading file!\n");
      //llclose(fd,status);
      exit(1);
    }
  }

  if (llclose(fd,status) < 0) {
    fprintf(stderr, "Error on llclose\n");
    exit(1);
  }*/

  int res = execution(argc,argv);

  return res;
}
