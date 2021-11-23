#include "application.h"

int makeControlPacket(int fd, long fSize,char fName[], char controlField){
    
    size_t fNameSize = strlen(fName) +1;

    if(fNameSize > 0xff){
        perror("File name is too big\n");
        return -1;
    }

    size_t pSize = 5 + sizeof(long) + fNameSize;
    uint8_t* cPacket = malloc(pSize);

    cPacket[0] = controlField;
    cPacket[1] = T_F_SIZE;
    cPacket[2] = (uint8_t) sizeof(long);
    memcpy(cPacket +3, &fSize,sizeof(long));
    cPacket[3+ sizeof(long)] = T_F_NAME;
    cPacket[4+ sizeof(long)] = (uint8_t) fNameSize;
    memcpy(cPacket +5 +sizeof(long), fName,fNameSize);

    if(llwrite(fd,cPacket,pSize,SENDERID) == -1) return -1;

    free(cPacket);
    return 0;

}

int readControlPacket(int fd, long* fSize,char** fName, char controlField, uint8_t buf[]){

    int size;
    if(size = llread(fd,buf,RECEIVERID) == -1){
        printf("llread failed\n");
        return -1;
    }

    if(buf[0] != controlField){
        perror("Control Field failed\n");
        return -1;
    }

    uint8_t t_type;
    uint8_t length;
    int i = 1;
    while( i < size){
        t_type = buf[i];
        i++;
        length = buf[i];
        i++;

        if( t_type == T_F_SIZE){
            memcpy(fSize, buf + i, length);
        }
        else if(t_type == T_F_NAME){
            *fName = malloc(length);
            memcpy(*fName, buf + i,length);
        }

        i += length;
    }

    return 0;
}

void makeDataPacket(uint8_t* fData ,int fDataSize ,uint8_t* dPacket,int dPacketSize,uint8_t sequence){

    int8_t l1 = fDataSize % 256;
    int8_t l2 = fDataSize / 256;

    dPacket[0] = CF_DATA;
    dPacket[1] = sequence;
    dPacket[2] = l2;
    dPacket[3] = l1;
    memcpy(dPacket + 4,fData,fDataSize);

}

int sendDataPacket(int fd, long fSize, FILE* fd1){
    
    uint8_t* data = malloc(fSize);
    fread(data,sizeof(uint8_t), fSize,fd1);

    uint8_t sequence = 0;

    int i = 0;
    while(i< fSize){

        int fDataSize = 0;
        if(fSize -i < 200){
            fDataSize = fSize -i;
        }
        else{
            fDataSize = 200;
        }

        uint8_t* fData = malloc(fDataSize);
        memcpy(fData,data + i,fDataSize);

        uint8_t* dPacket = malloc(fDataSize + 4);
        makeDataPacket(fData,fDataSize,dPacket,fDataSize +4,sequence);

        if(llwrite(fd,dPacket, fDataSize +4, SENDERID)){
            printf("llwrite failed \n");
            return -1;
        }

        free(fData);
        free(dPacket);

        sequence = (sequence +1) % 256;
        i += 200;

    }

    free(data);

    return 0;
}

int sendFile(int fd,char path[]){
    FILE* fd1;
    fd1 = fopen(path,"rb");

    if(fd1 == NULL){
        perror("fopen failed\n");
        return -1;
    }

    if(fseek(fd1, 0, SEEK_END) == -1){
        perror("fseek end failed\n");
        return -1;
    }

    long fSize = ftell(fd1) +1;


    if(fseek(fd1,0,SEEK_SET) == -1){
        perror("fseek set failed\n");
        return -1;
    }

    //SEND CONTROL PACKET
    if(makeControlPacket(fd,fSize,basename(path),CF_START) == -1) return -1;

    //SEND DATA
    if(sendDataPacket(fd,fSize,fd1) == -1) return -1;

    //SEND CONTROL PACKET
    if(makeControlPacket(fd,fSize,basename(path),CF_END) == -1) return -1;

    fclose(fd1);
    return fSize;
}

int readFile(int fd){


    char* fName = NULL;
    char buf[255];
    long fSize;
    

    //Read Control Packet
    if(readControlPacket(fd,&fSize,&fName,CF_START,buf) == -1) return -1;

    
    FILE* fd1;
    fd1= fopen(fName, "wb");

    if(fd1 == NULL){
        perror("fopen failed\n");
        return -1;
    }

    // loop llread
    int size;
    uint8_t sequence;
    while ((size = llread(fd,buf,RECEIVERID)) != -1)
    {
        if(buf[0] == CF_END) break;

        if(buf[0] != CF_DATA){
            printf("No control data\n");
            return -1;
        }

        if(buf[1] != sequence){
            printf("Invalid sequence\n");
            return -1;
        }

        int dfSize =  256 * buf[2]  + buf[3];
        uint8_t* dField = malloc(dfSize);
        memcpy(dField,buf + 4 , dfSize);

        fwrite(dField,sizeof(uint8_t),dfSize,fd1);

        sequence = (sequence +1) % 256;
        free(dField);
    }

    free(fName);
    fclose(fd1);
    return fSize;


}

