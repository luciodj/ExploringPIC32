/*
** PS2KBD.h
**
*/
// special function characters
#define TAB     0x9
#define BKSP    0x8
#define ENTER   0xd
#define ESC     0x1b

#define NUM     0x0
#define L_CTRL  0x0
#define L_SHFT  0x12 
#define CAPS    0x58
#define R_SHFT  0x12
#define F1      0x0
#define F2      0x0
#define F3      0x0
#define F4      0x0
#define F5      0x0
#define F6      0x0
#define F7      0x0
#define F8      0x0
#define F9      0x0
#define F10     0x0
#define F11     0x0
#define F12     0x0

extern volatile int KBDReady;
extern volatile unsigned char KBDCode;

void initKBD( void);
char getcKBD( void);

