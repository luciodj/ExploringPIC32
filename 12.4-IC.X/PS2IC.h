/*
**
**  PS2IC.h
**
**  PS/2 keyboard input library using input capture
*/

extern volatile int KBDReady;
extern volatile unsigned char KBDCode;

void initKBD( void);
