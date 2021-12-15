#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>


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

  //printf ("dflag = %d, hflag = %d, cvalue = %s\n",flagFrame[0], flagFrame[1], id);

  /*if( (argc - optind) > 1 ) {
    fprintf(stderr, "Error inserting arguments!\n");
    return NULL;
  }*/
  if(argv[optind] == NULL ) return NULL;

  strcpy(serialPort, argv[optind]);
  
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
    return -1;
  }
  if( res[1] == 1 ){
    printHelpMessage();
    return 0;
  }
  if( res[2] == 1 ){
    *id = -1;
    if( (strcmp(identifier,"emissor") != 0) && (strcmp(identifier,"receiver") != 0) ) return -1;
    else {
      fprintf(stdout,"%s Side\n", identifier);
      *id = (strcmp(identifier,"emissor") == 0) ? 0 : 1;
    }
  }
  if( res[3] != 1 ) file = "./pinguim.gif";
  if( res[0] == 1 ) {
    fprintf(stdout,"Debug mode enabled\n");
  }
  return 0;
}

int execution(int argc, char **argv){
  int *i = malloc(sizeof(int));
  char *id = (char *) malloc(15*sizeof(char));
  char *file = (char *) malloc(100*sizeof(char));
  char *serialPort = (char*) malloc(20*sizeof(char));
  if( validateArgs(argc, argv, i, file, id, serialPort) == -1 ){
    fprintf(stdout,"Bad input provided, try using serialport -h\n");
    return -1;
  }
  fprintf(stdout, "%s\n",file);
  fprintf(stdout, "Sp=> %s\n",serialPort);

  int fd, status = *i;
  

  fprintf(stdout, "%d\n",status);
  fprintf(stdout, "Sp=> %s\n",serialPort);
  return 0;
}


int main (int argc, char **argv){
  execution(argc,argv);
  
  /*int *res = parseArgs(argc, argv, idE);
  if( res == NULL ){
   printf("Aqui\n");
   exit(1);
  }
  else if( res[1] == 1 ){
    printHelpMessage();
  }
  printf("%d\n",res[0]);
  printf("%s\n", idE);*/
  return 0;
}