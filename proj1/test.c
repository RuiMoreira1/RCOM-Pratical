#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


int* parseArgs(int argc, char **argv){
  int *flagFrame; /* Debug, Help, Identity ... */
  flagFrame = malloc(4* sizeof *flagFrame);
  int index, c;
  char *id = NULL;

  opterr = 0;

  while ((c = getopt (argc, argv, "dhi:b:")) != -1)
    switch (c){
      case 'd':
        flagFrame[0] = 1;
        break;
      case 'h':
        flagFrame[1] = 1;
        break;
      case 'i':
        flagFrame[2] = 1;
        id = optarg;
        break;
      case 'b':
        flagFrame[3] = 1;
      case '?':
        if (optopt == 'c') fprintf(stderr, "Option -%c requires an argument.\n", optopt);
        else if (isprint (optopt)) fprintf(stderr, "Unknown option `-%c'.\n", optopt);
        else fprintf(stderr,"Unknown option character `\\x%x'.\n",optopt);
        return NULL;
      default:
        abort();
      }

  printf ("dflag = %d, hflag = %d, cvalue = %s\n",flagFrame[0], flagFrame[1], id);

  if( (argc - optind) > 1 ) {
    fprintf(stderr, "Error inserting arguments!\n");
    return NULL;
  }
  for (index = optind; index < argc; index++)
    printf ("Non-option argument %s\n", argv[index]);
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
  fprintf(stdout, "\tserialport -i sender -d file.txt\t\t\t Serial port sender side, debug mode active sending file.txt\n");
  fprintf(stdout, "\tserialport -d -i receiver\t\t\t Serial port receiver side, debug mode active receiving pinguim.gid\n");
}

int main (int argc, char **argv){
  printHelpMessage();
  int *res = parseArgs(argc, argv);
  if( res == NULL ){
   printf("Aqui\n");
   exit(1);
  }
  printf("%d\n",res[0]);
  return 0;
}