#include "application.h"

int sendFile(int fd, char filePath[]) {
  FILE *ptr;
  if ((ptr = fopen(filePath, "rb")) == NULL) {
    fprintf(stderr, "Error opening file\n");
    return -1;
  }

  fseek(ptr, 0, SEEK_END);
  long fileSize = ftell(ptr);
  rewind(ptr);
  if (sendControlPacket(fd, START_CTRL, fileSize, basename(filePath)) < 0) return -1;

  if (sendData(fd, ptr, fileSize) < 0) {
    return -1;
  }

  if (sendControlPacket(fd, END_CTRL, fileSize, basename(filePath)) < 0) return -1;

  fclose(ptr);
  return fileSize;
}

int sendControlPacket (int fd, u_int8_t controlField, long fileSize, char fileName[]) {
  size_t fileNameSize = strlen(fileName) + 1;
  if (fileNameSize > 0xff) {
    fprintf(stderr, "File name can only be up to 255 chars long\n");
    return -1;
  }

  size_t packetSize = 5 + sizeof(long) + fileNameSize;
  u_int8_t *controlPacket = malloc(packetSize);

  controlPacket[0] = controlField;

  controlPacket[1] = T_FILE_SIZE;
  controlPacket[2] = (u_int8_t) sizeof(long);
  memcpy(controlPacket + 3, &fileSize, sizeof(long));

  controlPacket[3 + sizeof(long)] = T_FILE_NAME;
  controlPacket[4 + sizeof(long)] = (u_int8_t) fileNameSize;
  memcpy(controlPacket + 5 + sizeof(long), fileName, fileNameSize);


  if (llwrite(fd, controlPacket, packetSize, SENDERID) < 0) return -1;

  free(controlPacket);

  return 1;
}

int sendData(int fd, FILE* ptr, long fileSize) {
  u_int8_t sequenceNum = 0;

  u_int8_t* data = malloc(fileSize);
  fread(data, sizeof(u_int8_t), fileSize, ptr);

  for (int idx = 0; idx < fileSize; idx += DATA_ACTUAL_SIZE) {
    int frameDataSize = min(DATA_ACTUAL_SIZE, fileSize - idx);
    u_int8_t* frameData = malloc(frameDataSize);
    memcpy(frameData, data + idx, frameDataSize);

    int dataPacketSize = frameDataSize + NUM_DATA_ADDITIONAL_FIELDS;
    u_int8_t* dataPacket = malloc(dataPacketSize);
    buildDataPacket(dataPacket, dataPacketSize, frameData, frameDataSize, sequenceNum);

    if (llwrite(fd, dataPacket, dataPacketSize, SENDERID) < 0) {
      fprintf(stderr, "Error sending data packet\n");
      return -1;
    }

    free(frameData); free(dataPacket);

    sequenceNum = (sequenceNum + 1) % 256;
  }

  free(data);
  return 1;
}

void buildDataPacket(u_int8_t* dataPacket, int dataPacketSize, u_int8_t* frameData, int frameDataSize, u_int8_t sequenceNum) {
  u_int8_t l2 = frameDataSize / NUM_OCTETS_MULTIPLIER;
  u_int8_t l1 = frameDataSize % NUM_OCTETS_MULTIPLIER;
  dataPacket[0] = DATA_CTRL; dataPacket[1] = sequenceNum; dataPacket[2] = l2; dataPacket[3] = l1;
  memcpy(dataPacket + NUM_DATA_ADDITIONAL_FIELDS, frameData, frameDataSize);
}



int readControlPacket(int fd, int controlField, u_int8_t buffer[], char** fileName, long* fileSize) {
  int size, currIdx = 1;
  if ((size = llread(fd, buffer, RECEIVERID)) < 0) {
    fprintf(stderr, "Error reading Control Packet\n");
    return -1;
  }
  if (buffer[0] != controlField) {
    fprintf(stderr, "Invalid control byte\n");
    return -1;
  }

  u_int8_t type, length;
  while (currIdx < size) {
    type = buffer[currIdx++];
    length = buffer[currIdx++];

    switch (type) {
      case T_FILE_SIZE:
        memcpy(fileSize, buffer+currIdx, length);
        break;
      case T_FILE_NAME:
        *fileName = malloc(length);
        memcpy(*fileName, buffer+currIdx, length);
      default:
        break;
    }
    currIdx += length;
  }

  return 1;
}

int readFile(int fd) {
  u_int8_t buffer[MAX_DATA_SIZE];
  char* fileName = NULL;
  long fileSize;

  if (readControlPacket(fd, START_CTRL, buffer, &fileName, &fileSize) < 0) return -1;

  FILE *ptr;
  if ((ptr = fopen(fileName, "wb")) == NULL) {
    fprintf(stderr, "Error opening file\n");
    return -1;
  }

  int size;
  u_int8_t sequenceNum = 0;
  while ((size = llread(fd, buffer, RECEIVERID)) > 0) {
    if (buffer[0] == END_CTRL) {
      break;  // Reached END control packet
    }
    if (buffer[0] != DATA_CTRL) {
      fprintf(stderr, "Invalid control byte for data packet\n");
      return -1;
    }
    if (buffer[1] != sequenceNum) {
      fprintf(stderr, "Invalid sequence number received in data packet!\n");
      return -1;
    }
    int dataFieldOctets = buffer[2] * NUM_OCTETS_MULTIPLIER + buffer[3];
    u_int8_t* dataField = malloc(dataFieldOctets);
    memcpy(dataField, buffer + NUM_DATA_ADDITIONAL_FIELDS, dataFieldOctets);

    // Write to opened file
    fwrite(dataField, sizeof(u_int8_t), dataFieldOctets, ptr);

    sequenceNum = (sequenceNum + 1) % 256;
    free(dataField);
  }

  free(fileName);
  fclose(ptr);
  return fileSize;
}
