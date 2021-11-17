/* Macros header to improve code readability*/

#define BIT(n) (1 << n)

#define FLAG 0x7E          //SET message beginning
#define WAITING_MAX 3     //Maximum waiting time, before sender receives no answer from the receiver, before another SET is sent

/* Address A field */

#define A_EC 0x03         //Commands issued by the sender and registered by the receiver
#define A_CE 0x01         //Commands issued by the receiver and registered by the sender

/* Control Field C */

//Commands Tramas
#define C_SET 0x03        //Setup
#define C_DISC 0x0B       //Disconnect

//Answer Tramas

#define C_UA 0x07                     //Unnumbered ACK
#define C_RR(n) (n << 7 | 0x05)       //(0x05 or 0x85)receiver ready / positive ACK
#define C_REJ(n) (n << 7 | 0x01)      //(0x81 or 0x01)reject / negative ACK


/* Header Protection Field */

#define BCC(x,y) (x ^ y)         //



#define RECEIVER 1
#define SENDER 0
