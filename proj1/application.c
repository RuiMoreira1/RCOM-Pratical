#include "application.h"

int sendFile(int fd, char fPath[]) {

  FILE *fd1;

  if ((fd1 = fopen(fPath, "rb")) == NULL) {
    fprintf(stderr, "Open file failed\n");
    return -1;
  }

  fseek(fd1, 0, SEEK_END);

  long fSize = ftell(fd1);
  rewind(fd1);

  if (sendControlPacket(fd, C_START, fSize, basename(fPath)) < 0){
    return -1;
  } 

  if (sendData(fd, fd1, fSize) < 0) {
    return -1;
  }

  if (sendControlPacket(fd, C_END, fSize, basename(fPath)) < 0){
    return -1;
  } 

  fclose(fd1);
  return fSize;
}

int sendControlPacket (int fd, u_int8_t cField, long fSize, char fName[]) {
  
  size_t fNSize = strlen(fName) + 1;

  if (fNSize > 0xff) {
    fprintf(stderr, "File is too big\n");
    return -1;
  }

  size_t pSize =  fNSize + sizeof(long) + 5;
  u_int8_t *controlPacket = malloc(pSize);

  controlPacket[0] = cField;
  controlPacket[1] = T_TYPE_SIZE;
  controlPacket[2] = (u_int8_t) sizeof(long);
  memcpy(controlPacket + 3, &fSize, sizeof(long));
  controlPacket[3 + sizeof(long)] = T_TYPE_NAME;
  controlPacket[4 + sizeof(long)] = (u_int8_t) fNSize;
  memcpy(controlPacket + 5 + sizeof(long), fName, fNSize);

  if (llwrite(fd, controlPacket, pSize, SENDERID) < 0) return -1;
  free(controlPacket);

  return 1;
}

int sendData(int fd, FILE* fd1, long fSize) {
  
  u_int8_t* data = malloc(fSize);
  fread(data, sizeof(u_int8_t), fSize, fd1);

  u_int8_t sequence = 0;
  for (int i = 0; i < fSize; i += D_REAL_SIZE) {

    int fDataSize = min(D_REAL_SIZE, fSize - i);
    u_int8_t* fData = malloc(fDataSize);
    memcpy(fData, data + i, fDataSize);

    int dPacketSize = fDataSize + ADD_FIELDS;
    u_int8_t* dPacket = malloc(dPacketSize);
    dataPacketBuilder(dPacket,dPacketSize, fData, fDataSize, sequence);

    if (llwrite(fd, dPacket, dPacketSize, SENDERID) < 0) {
      fprintf(stderr, "Error sending data packet\n");
      return -1;
    }

    free(fData); 
    free(dPacket);

    sequence = (sequence + 1) % 256;
  }

  free(data);
  return 1;
}

void dataPacketBuilder(u_int8_t* dPacket,int dPacketSize, u_int8_t* fData, int fDataSize, u_int8_t sequence) {
  
  u_int8_t l1 = fDataSize % N_MULT;
  u_int8_t l2 = fDataSize / N_MULT;
  
  dPacket[0] = C_DATA; 
  dPacket[1] = sequence;
  dPacket[2] = l2; 
  dPacket[3] = l1;

  memcpy(dPacket + ADD_FIELDS, fData, fDataSize);

}



int readControlPacket(int fd, u_int8_t cField, u_int8_t buf[], char** fName, long* fSize) {
  int size;
  
  if ((size = llread(fd, buf, RECEIVERID)) < 0) {
    fprintf(stderr, "Failed reading cPacket\n");
    return -1;
  }

  if (buf[0] != cField) {
    fprintf(stderr, "Error in cField\n");
    return -1;
  }

  int i = 1;
  u_int8_t type, length;
  while (i < size) {
    type = buf[i++];
    length = buf[i++];

    if(type == T_TYPE_SIZE){
      memcpy(fSize, buf+i, length);
        
    }
    else if(type == T_TYPE_NAME){
      *fName = malloc(length);
        memcpy(*fName, buf+i, length);
    }
    
    i += length;
  }

  return 1;
}

int readFile(int fd) {
  u_int8_t buf[MAX_DATA_SIZE];
  char* fName = NULL;
  long fSize;

  if (readControlPacket(fd, C_START, buf, &fName, &fSize) < 0){
    return -1;
  } 

  FILE *fd1;
  if ((fd1 = fopen(fName, "wb")) == NULL) {
    fprintf(stderr, "Open file failed\n");
    return -1;
  }

  int size;
  u_int8_t sequence = 0;

  while ((size = llread(fd, buf, RECEIVERID)) != ERROR) {

    if (buf[0] == C_END) {
      break;  
    }
    if (buf[0] != C_DATA) {
      fprintf(stderr, "data packet control byte read failed\n");
      return -1;
    }
    if (buf[1] != sequence) {
      fprintf(stderr, "sequence number in data packet error\n");
      return -1;
    }

    int dataFieldOctets = N_MULT * buf[2]   + buf[3];

    u_int8_t* dField = malloc(dataFieldOctets);

    memcpy(dField, buf + ADD_FIELDS, dataFieldOctets);

    fwrite(dField, sizeof(u_int8_t), dataFieldOctets, fd1);

    sequence = (sequence + 1) % 256;
    free(dField);
  }

  free(fName);
  fclose(fd1);
  if( size == ERROR ) return ERROR; 
  return fSize;
}
