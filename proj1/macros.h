#ifndef MACROS_N
#define MACROS_h

#define ALARM_INTERVAL 3 /* Time interval between triggering the signal */

#define SENDERID 1
#define RECEIVERID 0

#define DEBUG 0  /* Debug falg */


#define BIT(n) 1 << n   /* Bit mask */

#define FLAG 0x7E  /* Start and Stop trame byte */
#define A_SR 0x03  /* A byte for Sender to Receiver */
#define A_RS 0x01  /* A byte for Receiver to sender */
#define C_UA 0x07  /* Ua Trame Cfield */
#define C_SET 0x03  /* Set Trame CField */
#define C_DISC 0x0B /* Disc Trame CField */
#define C_FRAME_I(n) ( BIT(6*n) & 0x40) /* I frame stuffing logical and. 0x40 in case s->1 0x00 in case s->0 */
#define C_RR(n) (BIT(7*n) | 0x05) /* S/U Frame RR field logical and 0x85 in case n->1 0x05 in case n->0 */
#define C_REJ(n) (BIT(7*n) | 0x01) /* S/U Frame REJ field logical and 0x81 in case n->1 0x01 in case n->0 */
#define ESCAPE 0x7D /* Escape Byte */
#define FLAG_ESCAPE_XOR 0x5E /* FLAG ^ 0x20 */
#define ESCAPE_XOR 0x5D /* Escape ^ 0x20 */





#define BCC(a,c) (a ^ c) /* Bcc flag is generated by A Byte XOR with C byte */

#define MAX_DATA_SIZE 200
#define STUFF_DATA_MAX ( MAX_DATA_SIZE * 2+ 2 ) /* Data + frameH + BCC2 */

#define ERROR -1  /* Error return type */
#define SUCCESS 0 /* Success return type */

#endif
