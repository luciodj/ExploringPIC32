/*
**
**  PS2CN.h
**
**  PS/2 keyboard input module using Change Notification
*/

extern volatile int KBDReady;
extern volatile unsigned char KBDCode;

void initKBD( void);
