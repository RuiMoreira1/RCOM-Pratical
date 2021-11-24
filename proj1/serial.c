#include "serial.h"

int main(int argc, char **argv){

  if (argc < 3 ||
    ((strcmp("/dev/ttyS10", argv[1])) && (strcmp("/dev/ttyS11", argv[1])))) {

    printf("Usage: application serialPort type <fileName>\nex: application /dev/ttyS10 0 ./imagem.gif\n<fileName> default value: ./pinguim.gif\n");
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
      exit(1);
    }

  } else {
    if (readFile(fd) < 0) {
      fprintf(stderr, "Error reading file!\n");
      exit(1);
    }
  }

  if (llclose(fd,status) < 0) {
    fprintf(stderr, "Error on llclose\n");
    exit(1);
  }

  return 0;
}
