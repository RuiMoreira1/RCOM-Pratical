/*Non-Canonical Input Processing*/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> 

#define BAUDRATE B38400
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1

volatile int STOP=FALSE;

int main(int argc, char** argv)
{
    int fd,c, res;
    struct termios oldtio,newtio;
    char buf[255];

    if ( (argc < 2) || 
  	     ((strcmp("/dev/ttyS10", argv[1])!=0) && 
  	      (strcmp("/dev/ttyS11", argv[1])!=0) )) {
      printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttyS1\n");
      exit(1);
    }


  /*
    Open serial port device for reading and writing and not as controlling tty
    because we don't want to get killed if linenoise sends CTRL-C.
  */
  
    
    fd = open(argv[1], O_RDWR | O_NOCTTY );
    if (fd <0) {perror(argv[1]); exit(-1); }

    if ( tcgetattr(fd,&oldtio) == -1) { /* save current port settings */
      perror("tcgetattr");
      exit(-1);
    }

    bzero(&newtio, sizeof(newtio));
    newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
    newtio.c_iflag = IGNPAR;
    newtio.c_oflag = 0;

    /* set input mode (non-canonical, no echo,...) */
    newtio.c_lflag = 0;

    newtio.c_cc[VTIME]    = 0;   /* inter-character timer unused */
    newtio.c_cc[VMIN]     = 1;   /* blocking read until 5 chars received */



  /* 
    VTIME e VMIN devem ser alterados de forma a proteger com um temporizador a 
    leitura do(s) pr�ximo(s) caracter(es)
  */



    tcflush(fd, TCIOFLUSH);

    if ( tcsetattr(fd,TCSANOW,&newtio) == -1) {
      perror("tcsetattr");
      exit(-1);
    }

    printf("New termios structure set\n");

    FILE * fd1 = fdopen(fd,"r+");
    int i = 0;
    while (STOP==FALSE) {    
      char ch;  
      //res = read(fd, &ch,1);
      res = fread( &ch, 1,1,fd1 );

      //Averiguar se deu erro
      if(res == -1){
        printf("Error!\n");
        exit(1);
      } 

      //Colocar no buf
      buf[i]= ch;         
      i++;     

      //Averiguar se chegou ao final 
      if(ch == '\0'){
        STOP = TRUE;
      }
    }
    printf("received: %s\n", buf);



    int res2 = write(fd, buf, sizeof(buf));
    //int res2 = fwrite(buf,strlen(buf),strlen(buf), fd1);
    if (res2 == -1){
      printf("Error!\n");
      exit(1);
    }

    STOP = FALSE;
    printf("Sent!\n");
    
    char buf1[255];
    while (STOP==FALSE) {    
      char ch2;  
      //res = read(fd, &ch2,1);  
      res = fread( &ch2, 1,1,fd1 );
      //Averiguar se deu erro
      if(res == -1){
        printf("Error!\n");
        exit(1);
      } 

      //Colocar no buf
      buf1[i]= ch2;         
      i++;   

      printf("Trame sent: 0x%02x",ch2) ;
      //printf("received: %s\n", buf1);
      //Averiguar se chegou ao final 
      if(ch2 == 0x7e){
        STOP = TRUE;
      }
    }
    printf("received: %s\n", buf);


    sleep(1);
    tcsetattr(fd,TCSANOW,&oldtio);
    close(fd);
    return 0;
}
